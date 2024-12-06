#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library


#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define	BLACK   0x0000
#define WHITE   0xFFFF
#define	RED     0xF800
#define	BLUE    0x001F

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_FT6206 ts;

class Event {
public:
  virtual ~Event() = default;
};

class TouchEvent : public Event {
public:
  ~TouchEvent() override = default;
};

class Window {
public:
  Window(int x, int y, int cx, int cy) :
      x_(x), y_(y), cx_(cx), cy_(cy) {}

  virtual ~Window() = default;

  // Update is used to allow a window to update it's state (e.g. by reading
  // external inputs.) A window should not draw during Update();
  virtual void Update() {}

  // Draw is invoked to refresh the Window's display on the screen.
  virtual void Draw(Adafruit_GFX* gfx) {
    // Window is valid after drawing.
    Validate();
  } 

  virtual void onTouch(const TouchEvent& event) = 0;
  virtual bool NeedsRefresh() const {
    // Default behavior is that we only need refresh if invalid.
    return !isValid();
  }

  int x() const { return x_; }
  int y() const { return y_; }
  int cx() const { return cx_; }
  int cy() const { return cy_; }

  Window* parent() const { return parent_; }
  void setParent(Window* w) { parent_ = w; }

  Window* nextSibling() const { return next_sibling_; }
  void setNextSibling(Window* w) { next_sibling_ = w; }

  bool isValid() const { return is_valid_; }
  void Invalidate() { is_valid_ = false; }
  void Validate() { is_valid_ = true; }

protected:
  int x_;
  int y_;
  int cx_;
  int cy_;

  Window* parent_ = nullptr;
  Window* next_sibling_ = nullptr;
  bool is_valid_ = false;
};


// A Panel is a container window that holds other windows.
class Panel : public Window {
public:
  Panel(int x, int y, int cx, int cy) : Window(x, y, cx, cy) {}
  ~Panel() override = default;

  void Update() override;
  void Draw(Adafruit_GFX* gfx) override;

  void onTouch(const TouchEvent& event) override {}
  bool NeedsRefresh() const override { return true; } 

  void AddChild(Window* w);

protected:
  Window* first_child_ = nullptr;
};

void Panel::AddChild(Window* w) {
  w->setParent(this);
  if (!first_child_) {
    first_child_ = w;
  } else {
    Window* last_child = first_child_;
    while (last_child->nextSibling() != nullptr) {
      last_child = last_child->nextSibling();
    }
    last_child->setNextSibling(w);
  }
}

void Panel::Update() {
  for (Window* w = first_child_; w != nullptr; w = w->nextSibling()) {
    w->Update();
  }
  Window::Update();
}

void Panel::Draw(Adafruit_GFX* gfx) {
  for (Window* w = first_child_; w != nullptr; w = w->nextSibling()) {
    if (w->NeedsRefresh()) {
      w->Draw(gfx);
    }
  }
  Window::Draw(gfx);
}

class Button : public Window {
public:
  Button(int x, int y, int cx, int cy, const char* text)
      : Window(x, y, cx, cy), text_(text) {}
  ~Button() override = default;

  void Draw(Adafruit_GFX* gfx) override;
  void onTouch(const TouchEvent& event) override {}

  const char* getText() const { return text_; }

protected:
  const char* text_;
};

void Button::Draw(Adafruit_GFX* gfx) {
  gfx->fillRect(x(), y(), cx(), cy(), BLUE);
  int16_t tx, ty;
  uint16_t bx, by;
  gfx->getTextBounds(getText(), x(), y(), &tx, &ty, &bx, &by);
  Serial.print("TEXT BOUNDS: ");
  Serial.print(bx);
  Serial.print(",");
  Serial.println(by);
  gfx->setTextColor(WHITE);
  gfx->setCursor(x() + (cx() - bx) / 2, y() + (cy() - by) / 2);
  gfx->print(getText());
  Window::Draw(gfx);
}

class Label : public Window {
public:
  Label(int x, int y, int cx, int cy, const char* text,
        int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : Window(x, y, cx, cy), text_(text), fg_(fg), bg_(bg), size_(size) {}
  ~Label() override = default;

  void Draw(Adafruit_GFX* gfx) override;
  void onTouch(const TouchEvent& event) override {}

  const char* getText() const { return text_; }
  void setText(const char* text) {
    text_ = text;
    Invalidate();
  }

protected:
  const char* text_;
  int16_t fg_ = WHITE;
  int16_t bg_ = BLACK;
  int size_ = 1;
};

void Label::Draw(Adafruit_GFX* gfx) {
  Serial.print("Drawing Label@");
  Serial.println((unsigned long)(void*)this, HEX);

  gfx->fillRect(x(), y(), cx(), cy(), bg_);
  gfx->setCursor(x(), y());
  gfx->setTextColor(fg_);
  gfx->setTextSize(size_);
  gfx->print(getText());
  Window::Draw(gfx);
}

class SmoothLabel : public Label {
public:
  SmoothLabel(int x, int y, int cx, int cy, const char* text,
        int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : Label(x, y, cx, cy, text, fg, bg, size) {}
  ~SmoothLabel() override = default;

  void Draw(Adafruit_GFX* gfx) override;
};

void SmoothLabel::Draw(Adafruit_GFX* gfx) {
  Serial.print("Drawing SmoothLabel@");
  Serial.println((unsigned long)(void*)this, HEX);

  for (int y = 0; y < cy(); y++) {
    GFXcanvas16 canvas(cx(), 1);
    canvas.fillScreen(bg_);
    canvas.setCursor(0, 0 - y);
    canvas.setTextColor(fg_);
    canvas.setTextSize(size_);
    canvas.print(getText());
    gfx->drawRGBBitmap(x(), this->y() + y, canvas.getBuffer(),
        canvas.width(), canvas.height());
  }
  Window::Draw(gfx);
}

class TimeLabel : public SmoothLabel {
public:
  TimeLabel(int x, int y, int cx, int cy, const char* text,
            int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : SmoothLabel(x, y, cx, cy, text, fg, bg, size) {}
  ~TimeLabel() override = default;

  virtual void Update() override;

protected:
  char display_time_[16];
};

void TimeLabel::Update() {
  unsigned long now = millis();
  sprintf(display_time_, "%ld.%03ld", now / 1000, now % 1000);
  setText(display_time_);
}

void setup() {
  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(3);

  ts.begin();

  unsigned long start = micros();
  tft.fillScreen(BLACK);
  Serial.print(F("CLRSCR took "));
  Serial.println(micros() - start);

}

void DispatchProc(Window* mainWindow) {
  mainWindow->Update();
  if (mainWindow->NeedsRefresh()) {
    mainWindow->Draw(&tft);
  }
}


int main() {
  init();

  tft.reset();

  Serial.begin(115200);
  while (!Serial) delay(10);

  Panel panel(10, 10, tft.width(), tft.height());
  Label lbl(70, 80, 170, 15, "HELLO, WORLD!!", WHITE, 0xCE79, 2);
  Label lbl2(100, 120, 100, 10, "Isn't this cool", RED);
  Label lbl3(10, 220, 100, 10, "Current Time:", BLUE);
  TimeLabel time(120, 220, 100, 10, "");

  panel.AddChild(&lbl);
  panel.AddChild(&lbl2);
  panel.AddChild(&lbl3);
  panel.AddChild(&time);

  Button button(1, 1, 80, 40, "Hello");
  panel.AddChild(&button);

  setup();

  for (;;) {
    DispatchProc(&panel);
  }
}