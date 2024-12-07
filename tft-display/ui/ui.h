#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library

#define	BLACK   0x0000
#define WHITE   0xFFFF
#define	RED     0xF800
#define	BLUE    0x001F

class Event {
public:
  virtual ~Event() = default;
};

enum class TouchEventType {
  None = 0,
  Begin = 1,
  Drag = 2,
  End = 3,
};

class TouchEvent : public Event {
public:
  TouchEvent(TouchEventType type, const TS_Point& p) : type_(type), point_(p) {}
  ~TouchEvent() override = default;

  TouchEventType type() const { return type_; }
  const TS_Point& point() const { return point_; }

protected:
  TouchEventType type_ = TouchEventType::None;
  TS_Point point_;
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

  // Return true if the touch event was handled, else false.
  virtual bool onTouch(const TouchEvent& event) = 0;

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

  bool PointInWindow(const TS_Point& p) {
    return (p.x >= x_ &&
            p.x <= x_ + cx_ &&
            p.y >= y_ &&
            p.y <= y_ + cy_);
  }

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

  bool onTouch(const TouchEvent& event) override;
  bool NeedsRefresh() const override { return true; } 

  void AddChild(Window* w);

protected:
  Window* first_child_ = nullptr;
};

class Button : public Window {
public:
  Button(int x, int y, int cx, int cy, const char* text)
      : Window(x, y, cx, cy), text_(text) {}
  ~Button() override = default;

  void Draw(Adafruit_GFX* gfx) override;
  bool onTouch(const TouchEvent& event) override;

  virtual void onPress(const TouchEvent& event) {}

  const String& getText() const { return text_; }

protected:
  String text_;
};

class Label : public Window {
public:
  Label(int x, int y, int cx, int cy, const String& text,
        int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : Window(x, y, cx, cy), text_(text), fg_(fg), bg_(bg), size_(size) {}
  ~Label() override = default;

  void Draw(Adafruit_GFX* gfx) override;
  bool onTouch(const TouchEvent& event) override { return false; }

  const String& getText() const { return text_; }
  void setText(const String& text) {
    text_ = text;
    Invalidate();
  }

protected:
  String text_;
  int16_t fg_ = WHITE;
  int16_t bg_ = BLACK;
  int size_ = 1;
};

class SmoothLabel : public Label {
public:
  SmoothLabel(int x, int y, int cx, int cy, const String& text,
        int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : Label(x, y, cx, cy, text, fg, bg, size) {}
  ~SmoothLabel() override = default;

  void Draw(Adafruit_GFX* gfx) override;
};

void DispatchProc(Adafruit_GFX* gfx, Window* main_window);

void UILoop(Adafruit_GFX* gfx, Adafruit_FT6206* ts, Window* main_window);
