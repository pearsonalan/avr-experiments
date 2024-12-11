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

class TimeLabel : public SmoothLabel {
public:
  TimeLabel(int x, int y, int cx, int cy, const String& text,
            int16_t fg=WHITE, int16_t bg=BLACK, int size=1)
      : SmoothLabel(x, y, cx, cy, text, fg, bg, size) {}
  ~TimeLabel() override = default;

  virtual void Update() override;

protected:
  char display_time_[16];
};

class OutputButton : public Button {
public:
  OutputButton(int x, int y, int cx, int cy, const char* text, Label* output_label) :
    Button(x, y, cx, cy, text), output_label_(output_label) {}

  void onPress(const TouchEvent&) override {
    Serial.println("In OutputButton::onPress");
    String s = getText();
    s += " pressed";
    output_label_->setText(s.c_str());
  }

protected:
  Label* const output_label_;
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

int main() {
  init();

  tft.reset();

  Serial.begin(115200);
  while (!Serial) delay(10);

  Panel panel(1, 1, tft.width(), tft.height());
  Label lbl(170, 10, 90, 15, "UI Demo", WHITE, 0xCE79, 2);
  Label lbl2(170, 30, 100, 10, "Isn't this cool", RED);
  Label lbl3(10, 220, 110, 10, "Seconds since start:", BLUE);
  Label output(40, 180, 240, 30, "", WHITE, BLACK, 2);
  TimeLabel time(140, 220, 100, 10, "");

  panel.AddChild(&lbl);
  panel.AddChild(&lbl2);
  panel.AddChild(&lbl3);
  panel.AddChild(&output);
  panel.AddChild(&time);

  OutputButton button1(3, 3, 80, 40, "Button 1", &output);
  panel.AddChild(&button1);

  OutputButton button2(3, 53, 80, 40, "Button 2", &output);
  panel.AddChild(&button2);

  setup();

  UILoop(&tft, &ts, &panel);
}
