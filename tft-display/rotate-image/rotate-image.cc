#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library

#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <Trig.h>

#include "ui.h"
#include "affine.h"

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_FT6206 ts;

class Orientation {
 public:
  void set_rotation(const fixed& r) { rotation_ = r; }
  const fixed& rotation() const { return rotation_; }

 protected:
  fixed rotation_ = 0;
};

class Image {
 public:
  int16_t GetXY(int x, int y) {
    if (x == 0 || y == 0) return RED;
    if (y == 40) return BLUE;
    if (y % 20 == 0) {
      return WHITE;
    } else if (x % 20 == 0) {
      return WHITE;
    }
    return BLACK;
  }
};

#define WIDTH 240
#define HEIGHT 240

#define USE_AFFINE_TRANSFORM 1
class ImageRasterizer {
 public:
  ImageRasterizer(Orientation& orientation) {
    AffineTransform rotate = AffineTransform::Rotate(orientation.rotation());
    AffineTransform translate = AffineTransform::Translate(-(WIDTH/2), HEIGHT/2);
    AffineTransform scale = AffineTransform::Scale(1, -1);
    t_ = (rotate * translate) * scale;
  }

  void GetLine(Image* img, int y, uint16_t* line, int* start, int* end) {
    *start = 0;
    *end = WIDTH;
    Point2D p_start(*start, y);
    Point2D s_start = t_ * p_start;
    Point2D p_end(*end, y);
    Point2D s_end = t_ * p_end;
    fixed dx = (s_end.x() - s_start.x()) / (fixed)(*end - *start);
    fixed dy = (s_end.y() - s_start.y()) / (fixed)(*end - *start);
    for (int x = 0; x < WIDTH; x++) {
      line[x] = img->GetXY((s_start.x() + x * dx).getInteger(),
                           (s_start.y() + x * dy).getInteger());
    }
  }

 protected:
  AffineTransform t_;
};

class BitmapWindow : public Window {
 public:
  BitmapWindow(int x, int y, int cx, int cy, Orientation& orientation) :
      Window(x, y, cx, cy), orientation_(orientation) {}

  void Draw(Adafruit_GFX*) override;
  bool onTouch(const TouchEvent& event) override { return false; }

 protected:
  Orientation& orientation_;
};

class ControlButton : public Button {
public:
  ControlButton(int x, int y, int cx, int cy, const char* text,
                Orientation& orientation, BitmapWindow& bitmap_window) :
      Button(x, y, cx, cy, text),
      orientation_(orientation),
      bitmap_window_(bitmap_window) {}

  void onPress(const TouchEvent&) override {
    Serial.println(F("In ControlButton::onPress"));
    if (getText() == "CW") {
      orientation_.set_rotation(orientation_.rotation() + 5);
    } else {
      orientation_.set_rotation(orientation_.rotation() - 5);
    }
    bitmap_window_.Invalidate();
  }

 protected:
  Orientation& orientation_;
  BitmapWindow& bitmap_window_;
};

void BitmapWindow::Draw(Adafruit_GFX* gfx) {
  Serial.println(F("Starting draw"));
  auto start_time = micros();

  Image img;
  ImageRasterizer src(orientation_);
  for (int line = 0; line < HEIGHT; line++) {
    static uint16_t colors[WIDTH];
    int start, end;
    src.GetLine(&img, line, colors, &start, &end);
    tft.setAddrWindow(x() + start, y() + line, x() + start + end, y() + line);
    tft.pushColors(colors + start, end - start, true);
  }

  Serial.print(F("draw took "));
  Serial.println(micros() - start_time);

  Validate();
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
  Orientation orientation;
  BitmapWindow bitmap_window(80, 0, 240, 240, orientation);
  ControlButton button1(3, 3, 76, 40, "CW", orientation, bitmap_window);
  ControlButton button2(3, 53, 76, 40, "CCW", orientation, bitmap_window);

  panel.AddChild(&button1);
  panel.AddChild(&button2);
  panel.AddChild(&bitmap_window);

  setup();

  UILoop(&tft, &ts, &panel);
}
