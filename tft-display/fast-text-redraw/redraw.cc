#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

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

void setup() {
  Serial.begin(9600);
  while (!Serial) delay(10);
  
  Serial.println(F("TFT LCD test"));
  Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());
 
  tft.reset();

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

  unsigned long start = micros();
  tft.fillScreen(BLACK);
  Serial.print(F("CLRSCR took "));
  Serial.println(micros() - start);

}


bool useCanvas = true;

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(RED);
  tft.println("This is a test.");
  tft.setCursor(1, 30);
  tft.print("Current time is ");
  
  if (useCanvas) {
    unsigned long m = micros();
    for (int i = 0; i < 10; i++) {
      GFXcanvas16 canvas(80, 1); // 16-bit, 80x1 pixels
      canvas.setCursor(1, 1-i);
      canvas.fillScreen(WHITE);
      canvas.setTextColor(BLUE);
      canvas.print(m);
      tft.drawRGBBitmap(100, 30 + i, canvas.getBuffer(),
        canvas.width(), canvas.height());
    }
  } else {
    tft.fillRect(0, 30, 160, 10, BLACK);
    tft.setCursor(1, 30);
    tft.print("Current time is ");
    tft.print(micros());
  }

  Serial.print(F("Draw text took "));
  Serial.println(micros() - start);
  delay(200);
}
