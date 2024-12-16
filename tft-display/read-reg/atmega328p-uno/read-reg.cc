#include <Arduino.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

volatile uint8_t *csPort, *cdPort, *wrPort, *rdPort;
uint8_t csPinSet, cdPinSet, wrPinSet, rdPinSet, csPinUnset, cdPinUnset,
    wrPinUnset, rdPinUnset;

#define RD_ACTIVE *rdPort &= rdPinUnset
#define RD_IDLE *rdPort |= rdPinSet
#define WR_ACTIVE *wrPort &= wrPinUnset
#define WR_IDLE *wrPort |= wrPinSet
#define CD_COMMAND *cdPort &= cdPinUnset
#define CD_DATA *cdPort |= cdPinSet
#define CS_ACTIVE *csPort &= csPinUnset
#define CS_IDLE *csPort |= csPinSet

#define DELAY7                                                                 \
  asm volatile("rjmp .+0"                                                      \
               "\n\t"                                                          \
               "rjmp .+0"                                                      \
               "\n\t"                                                          \
               "rjmp .+0"                                                      \
               "\n\t"                                                          \
               "nop"                                                           \
               "\n" ::);

// Data write strobe, ~2 instructions and always inline
#define WR_STROBE                                                              \
  {                                                                            \
    WR_ACTIVE;                                                                 \
    WR_IDLE;                                                                   \
  }

#define write8inline(d)                                                        \
  {                                                                            \
    PORTD = (PORTD & B00000011) | ((d)&B11111100);                             \
    PORTB = (PORTB & B11111100) | ((d)&B00000011);                             \
    WR_STROBE;                                                                 \
  }
#define read8inline(result)                                                    \
  {                                                                            \
    RD_ACTIVE;                                                                 \
    DELAY7;                                                                    \
    result = (PIND & B11111100) | (PINB & B00000011);                          \
    RD_IDLE;                                                                   \
  }
#define setWriteDirInline()                                                    \
  {                                                                            \
    DDRD |= B11111100;                                                         \
    DDRB |= B00000011;                                                         \
  }
#define setReadDirInline()                                                     \
  {                                                                            \
    DDRD &= ~B11111100;                                                        \
    DDRB &= ~B00000011;                                                        \
  }

#define write8 write8inline
#define read8 read8inline
#define setReadDir setReadDirInline
#define setWriteDir setWriteDirInline

uint32_t readReg(uint8_t r) {
  uint32_t id;
  uint8_t x;

  // try reading register #4
  CS_ACTIVE;
  CD_COMMAND;
  write8(r);
  setReadDir(); // Set up LCD data port(s) for READ operations
  CD_DATA;
  delayMicroseconds(50);
  read8(x);
  id = x;   // Do not merge or otherwise simplify
  id <<= 8; // these lines.  It's an unfortunate
  read8(x);
  id |= x;  // shenanigans that are going on.
  id <<= 8; // these lines.  It's an unfortunate
  read8(x);
  id |= x;  // shenanigans that are going on.
  id <<= 8; // these lines.  It's an unfortunate
  read8(x);
  id |= x; // shenanigans that are going on.
  CS_IDLE;
  setWriteDir(); // Restore LCD data port(s) to WRITE configuration

  // Serial.print("Read $"); Serial.print(r, HEX);
  // Serial.print(":\t0x"); Serial.println(id, HEX);
  return id;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Begin test");

  csPort = portOutputRegister(digitalPinToPort(LCD_CS));
  cdPort = portOutputRegister(digitalPinToPort(LCD_CD));
  wrPort = portOutputRegister(digitalPinToPort(LCD_WR));
  rdPort = portOutputRegister(digitalPinToPort(LCD_RD));
  csPinSet = digitalPinToBitMask(LCD_CS);
  cdPinSet = digitalPinToBitMask(LCD_CD);
  wrPinSet = digitalPinToBitMask(LCD_WR);
  rdPinSet = digitalPinToBitMask(LCD_RD);
  csPinUnset = ~csPinSet;
  cdPinUnset = ~cdPinSet;
  wrPinUnset = ~wrPinSet;
  rdPinUnset = ~rdPinSet;
  *csPort |= csPinSet; // Set all control bits to HIGH (idle)
  *cdPort |= cdPinSet; // Signals are ACTIVE LOW
  *wrPort |= wrPinSet;
  *rdPort |= rdPinSet;

  Serial.println("init pins");
  pinMode(LCD_CS, OUTPUT); // Enable outputs
  pinMode(LCD_CD, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  if (LCD_RESET) {
    digitalWrite(LCD_RESET, HIGH);
    pinMode(LCD_RESET, OUTPUT);
  }

  Serial.println("Reading ID reg");
  uint16_t id;
  for (int i = 0; i < 5; i++) {
    id = (uint16_t)readReg(0xD3);
    delayMicroseconds(50);
    Serial.print("ReadReg 0xD3 returned ");
    Serial.println(id, HEX);
    if (id == 0x9341) {
      Serial.print("ID = ");
      Serial.println(id, HEX);
      break;
    }
  }
}

void loop() {}