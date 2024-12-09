#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library

#include "ui.h"

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_FT6206 ts;


enum class RedrawMode {
  RedrawRed,
  RedrawBlue,
  RedrawPattern,
};

class BitmapWindow : public Window {
 public:
  BitmapWindow(int x, int y, int cx, int cy) : Window(x, y, cx, cy) {}

  void Draw(Adafruit_GFX*) override;
  bool onTouch(const TouchEvent& event) override { return false; }

  void SetRedrawMode(RedrawMode mode) { redraw_mode_ = mode; }

 protected:
  RedrawMode redraw_mode_ = RedrawMode::RedrawRed;
};

class ControlButton : public Button {
public:
  ControlButton(int x, int y, int cx, int cy, const char* text, BitmapWindow* bitmap_window, RedrawMode mode) :
    Button(x, y, cx, cy, text), bitmap_window_(bitmap_window), mode_(mode) {}

  void onPress(const TouchEvent&) override {
    Serial.println("In ControlButton::onPress");
    bitmap_window_->SetRedrawMode(mode_);
  }

 protected:
  BitmapWindow* bitmap_window_;
  RedrawMode mode_ = RedrawMode::RedrawRed;
};

void BitmapWindow::Draw(Adafruit_GFX* gfx) {
  Serial.println("Starting draw");
  auto start_time = micros();

  switch (redraw_mode_) {
  case RedrawMode::RedrawRed:
    {
      gfx->fillRect(x(), y(), cx(), cy(), RED);
    }
    break;

  case RedrawMode::RedrawBlue:
    {
      gfx->fillRect(x(), y(), cx(), cy(), BLUE);
    }
    break;

  case RedrawMode::RedrawPattern:
    {
      for (int iy = 0; iy < cy(); iy++) {
        GFXcanvas16 canvas(cx(), 1);
        for (int ix = 0; ix < cx(); ix++) {
          int16_t color = RGB((ix & 0x1F), 0, (iy & 0x1F));
          canvas.drawPixel(ix, 0, color);
        }
        gfx->drawRGBBitmap(x(), y() + iy, canvas.getBuffer(), cx(), 1);
      }
    }
    break;
  }

  Serial.print("draw took ");
  Serial.println(micros() - start_time);
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
    return;
  }

  tft.begin(identifier);
  tft.setRotation(3);

  ts.begin();

  tft.fillScreen(BLACK);
}

int main() {
  init();

  tft.reset();

  Serial.begin(115200);
  while (!Serial) delay(10);

  Panel panel(1, 1, tft.width(), tft.height());
  BitmapWindow bitmap_window(80, 0, 240, 240);
  ControlButton button1(3, 3, 76, 40, "Red", &bitmap_window, RedrawMode::RedrawRed);
  ControlButton button2(3, 53, 76, 40, "Blue", &bitmap_window, RedrawMode::RedrawBlue);
  ControlButton button3(3, 103, 76, 40, "Pattern", &bitmap_window, RedrawMode::RedrawPattern);

  panel.AddChild(&button1);
  panel.AddChild(&button2);
  panel.AddChild(&button3);

  panel.AddChild(&bitmap_window);

  setup();

  UILoop(&tft, &ts, &panel);
}
