#include <Arduino.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_FT6206.h> // Touch Screen Library

#include <tft-display.h> // Hardware-specific library

#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <Trig.h>

#include "ui.h"
#include "affine.h"

class Orientation {
 public:
  void set_rotation(const fixed& r) { rotation_ = r; }
  const fixed& rotation() const { return rotation_; }

  void set_pitch(const fixed& r) { pitch_ = r; }
  const fixed& pitch() const { return pitch_; }

 protected:
  fixed rotation_ = 0;
  fixed pitch_ = 0;
};

class Image {
 public:
  int16_t GetXY(int x, int y) {
    if (x < -20 || x > 20) {
      if (y > 2) return 0x9E5E;
      if (y < -2) return 0xD520;
    } else {
      if (y == 20 || y == 40 || y == -20 || y == -40) return WHITE;
      if (y > 2) return 0x9E5E;
      if (y < -2) return 0xD520;
    }
    return WHITE;
  }
};

#define WIDTH 200
#define HEIGHT 200

const uint8_t PROGMEM CHORD[] = { 14, 20, 24, 28, 31, 34, 37, 39, 41, 44, 46, 47, 49,
  51, 53, 54, 56, 57, 59, 60, 61, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
  74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 85, 86, 87, 87,
  88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 95,
  96, 96, 96, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99,
  99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

class RoundMask {
 public:
   void GetLineMask(int y, int* start, int* end) {
    const int c = (y < 100) ? pgm_read_byte_near(CHORD + y) :
                              pgm_read_byte_near(CHORD + (199-y));
    *start = 100 - c;
    *end = 100 + c;
   }
};

class NullMask {
 public:
  void GetLineMask(int y, int* start, int* end) {
    *start = 0;
    *end = 200;
  }
};

#define USE_AFFINE_TRANSFORM 1
class ImageRasterizer {
 public:
  ImageRasterizer(Orientation& orientation) {
    AffineTransform reflect = AffineTransform::Scale(1, -1);
    AffineTransform translate = AffineTransform::Translate(-(WIDTH/2), HEIGHT/2);
    AffineTransform rotate = AffineTransform::Rotate(orientation.rotation());
    AffineTransform pitch = AffineTransform::Translate(0, orientation.pitch());
    t_ = pitch * (rotate * (translate * reflect));
  }

  void GetLine(Image* img, int y, uint16_t* line, int* start, int* end) {
    RoundMask m;
    m.GetLineMask(y, start, end);
    Point2D p_start(*start, y);
    Point2D s_start = t_ * p_start;
    Point2D p_end(*end, y);
    Point2D s_end = t_ * p_end;
    fixed dx = (s_end.x() - s_start.x()) / (fixed)(*end - *start);
    fixed dy = (s_end.y() - s_start.y()) / (fixed)(*end - *start);
    for (int x = *start; x < *end; x++) {
      line[x] = img->GetXY((s_start.x() + (x - *start) * dx).getInteger(),
                           (s_start.y() + (x - *start) * dy).getInteger());
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

class Overlay {
 public:
   bool IsOverlayLine(int y) {
    return y == 99 || y == 100;
   }

   void OverlayLine(int y, uint16_t* line) {
    for (int x = 70; x < 130; x++) {
      line[x] = RED;
    }
   }
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
      orientation_.set_rotation(orientation_.rotation() + 1);
    } else if (getText() == "CCW") {
      orientation_.set_rotation(orientation_.rotation() - 1);
    } else if (getText() == "UP") {
      orientation_.set_pitch(orientation_.pitch() + 1);
    } else if (getText() == "DN") {
      orientation_.set_pitch(orientation_.pitch() - 1);
    }
    bitmap_window_.Invalidate();
  }

 protected:
  Orientation& orientation_;
  BitmapWindow& bitmap_window_;
};


void BitmapWindow::Draw(Adafruit_GFX* gfx) {
  Serial.println(F("Starting draw"));
  TFTDisplay* tft = (TFTDisplay*)gfx;
  auto start_time = micros();

  Image img;
  ImageRasterizer src(orientation_);
  Overlay overlay;
  for (int line = 0; line < HEIGHT; line++) {
    static uint16_t colors[WIDTH] = {0};
    int start, end;
    src.GetLine(&img, line, colors, &start, &end);
    if (overlay.IsOverlayLine(line)) {
      overlay.OverlayLine(line, colors);
    }
    tft->setAddrWindow(x() + start, y() + line, x() + start + end, y() + line);
    tft->pushColors(colors + start, end - start, true);
  }

  Serial.print(F("draw took "));
  Serial.println(micros() - start_time);

  Validate();
}

void setup(TFTDisplay& tft, Adafruit_FT6206& ts) {
  Serial.println("Setup");
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

  Serial.println("TFT Begin");
  tft.begin(identifier);
  tft.setRotation(3);

  Serial.println("TS Begin");
  ts.begin();

  tft.fillScreen(BLACK);
}

int main() {
  init();

  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Begin");

  TFTDisplay tft;
  Adafruit_FT6206 ts;

  tft.reset();

  Panel panel(1, 1, tft.width(), tft.height());
  Orientation orientation;
  BitmapWindow bitmap_window(100, 20, 200, 200, orientation);
  ControlButton button1(3, 23, 76, 40, "CW", orientation, bitmap_window);
  ControlButton button2(3, 73, 76, 40, "CCW", orientation, bitmap_window);
  ControlButton buttonU(3, 123, 76, 40, "UP", orientation, bitmap_window);
  ControlButton buttonD(3, 173, 76, 40, "DN", orientation, bitmap_window);

  panel.AddChild(&button1);
  panel.AddChild(&button2);
  panel.AddChild(&buttonU);
  panel.AddChild(&buttonD);
  panel.AddChild(&bitmap_window);

  setup(tft, ts);

  Serial.println("Entering UI Loop");
  UILoop(&tft, &ts, &panel);
  Serial.println("Entering infininte loop");
  for (;;) ;
}
