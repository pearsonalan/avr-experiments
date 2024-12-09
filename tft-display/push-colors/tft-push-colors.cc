#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_FT6206.h> // Touch Screen Library

#define	BLACK   0x0000
#define WHITE   0xFFFF
#define	RED     0xF800
#define	BLUE    0x001F

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

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

  tft.fillScreen(BLACK);
}

class AttitudeIndicator {
 public:
   void set_pitch(int pitch) { pitch_ = pitch; }
   int pitch() const { return pitch_; }

 protected:
   int pitch_ = 0;
};

constexpr uint16_t COLORS[] = {0xD520, 0x9E5E};

class BaseImage {
 public:
  BaseImage(const AttitudeIndicator& ai) : ai_(ai) {}

  uint16_t GetXY(int x, int y) {
    if (y > 2) {
      return COLORS[1];
    }
    if (y < -2) {
      return COLORS[0];
    }
    return WHITE;
  }

  void GetLine(int y, uint16_t* line, int start_x, int end_x) {
    y = 100 - y;
    y += ai_.pitch();
    for (int x = start_x; x < end_x; x++) {
      line[x] = GetXY(x, y);
    }
  }

 protected:
   const AttitudeIndicator& ai_;
};

const uint8_t PROGMEM CHORD[] = { 14, 20, 24, 28, 31, 34, 37, 39, 41, 44, 46, 47, 49,
  51, 53, 54, 56, 57, 59, 60, 61, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73,
  74, 75, 76, 77, 78, 78, 79, 80, 81, 81, 82, 83, 84, 84, 85, 85, 86, 87, 87,
  88, 88, 89, 89, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 95,
  96, 96, 96, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 99, 99, 99,
  99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

class Mask {
 public:
   void GetLineMask(int y, int* start, int* end) {
    const int c = (y < 100) ? pgm_read_byte_near(CHORD + y) :
                              pgm_read_byte_near(CHORD + (199-y));
    *start = 100 - c;
    *end = 100 + c;
   }
};

class Overlay {
 public:
   bool IsOverlayLine(int y) {
    return y == 99 || y == 100;
   }

   void OverlayLine(int y, uint16_t* line) {
    for (int x = 80; x < 120; x++) {
      line[x] = RED;
    }
   }
};

class ImageSource {
 public:
  ImageSource(const AttitudeIndicator& ai) : img_(ai) {}
  void GetLine(int y, uint16_t* line, int* start, int* end) {
    mask_.GetLineMask(y, start, end);
    img_.GetLine(y, line, *start, *end);
    if (overlay_.IsOverlayLine(y)) {
      overlay_.OverlayLine(y, line);
    }
  }

 protected:
  BaseImage img_;
  Mask mask_;
  Overlay overlay_;
};


int main() {
  init();

  Serial.begin(115200);
  while (!Serial) delay(10);

  setup();

  int width = tft.width();
  int height = tft.height();

  AttitudeIndicator ai;
  ImageSource src(ai);
  for (;;) {
    auto t = millis() / 100;
    ai.set_pitch((t % 50) - 25);

    Serial.println("Pushing colors");
    auto start = micros();
    for (int line = 0; line < 200; line++) {
      uint16_t colors[200];
      int start, end;
      src.GetLine(line, colors, &start, &end);
      tft.setAddrWindow(100 + start, line + 20, 100 + start + end, line + 20);
      tft.pushColors(colors + start, end - start, true);
    }
    Serial.print("push colors took ");
    Serial.println(micros() - start);
  }
}
