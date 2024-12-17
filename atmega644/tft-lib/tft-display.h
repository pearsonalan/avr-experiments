// Graphics library by ladyada/adafruit with init code from Rossum
// MIT license

// Changed by alanpearson@mac.com

#ifndef _TFT_DISPLAY_H_
#define _TFT_DISPLAY_H_

#include "Arduino.h"

#include <Adafruit_GFX.h>

class TFTDisplay : public Adafruit_GFX {
public:
  TFTDisplay(void);

  void begin(uint16_t id = 0x9325);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
  void fillScreen(uint16_t color);
  void reset(void);
  void setRegisters8(uint8_t *ptr, uint8_t n);
  void setRegisters16(uint16_t *ptr, uint8_t n);
  void setRotation(uint8_t x);
  // These methods are public in order for BMP examples to work:
  void setAddrWindow(int x1, int y1, int x2, int y2);
  void pushColors(uint16_t *data, uint8_t len, boolean first);

  uint16_t color565(uint8_t r, uint8_t g, uint8_t b),
      readPixel(int16_t x, int16_t y), readID(void);
  uint32_t readReg(uint8_t r);

private:
  void init();
  void setLR(void);
  void flood(uint16_t color, uint32_t len);
  void writeRegister24(uint8_t a, uint32_t d);
  void writeRegister32(uint8_t a, uint32_t d);

  uint8_t driver;
};

// For compatibility with sketches written for older versions of library.
// Color function name was changed to 'color565' for parity with 2.2" LCD
// library.
#define Color565 color565

#endif
