#include <Arduino.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#if 0
volatile uint8_t *csPort, *cdPort, *wrPort, *rdPort;
uint8_t csPinSet, cdPinSet, wrPinSet, rdPinSet, csPinUnset, cdPinUnset,
    wrPinUnset, rdPinUnset;
#endif

#define RD_ACTIVE PORTD &= ~(1<<PD5)
#define RD_IDLE PORTD |= (1<<PD5)
#define WR_ACTIVE PORTD &= ~(1<<PD4)
#define WR_IDLE PORTD |= (1<<PD4)
#define CD_COMMAND PORTD &= ~(1<<PD3)
#define CD_DATA PORTD |= (1<<PD3)
#define CS_ACTIVE PORTD &= ~(1<<PD6)
#define CS_IDLE PORTD |= (1<<PD6)

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
    PORTA = d;                                                                 \ 
    WR_STROBE;                                                                 \
  }
#define read8inline(result)                                                    \
  {                                                                            \
    RD_ACTIVE;                                                                 \
    DELAY7;                                                                    \
    result = PINA;                                                             \
    RD_IDLE;                                                                   \
  }
#define setWriteDirInline()                                                    \
  {                                                                            \
    DDRA = 0xff;                                                               \
  }
#define setReadDirInline()                                                     \
  {                                                                            \
    DDRA = 0;                                                                  \
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
#if 0
  csPort = (volatile uint8_t *)PORTD;
  cdPort = (volatile uint8_t *)PORTD;
  wrPort = (volatile uint8_t *)PORTD;
  rdPort = (volatile uint8_t *)PORTD;
  csPinSet = (1<<PD6);
  cdPinSet = (1<<PD3);
  wrPinSet = (1<<PD4);
  rdPinSet = (1<<PD5);
  csPinUnset = ~csPinSet;
  cdPinUnset = ~cdPinSet;
  wrPinUnset = ~wrPinSet;
  rdPinUnset = ~rdPinSet;
  *csPort |= csPinSet; // Set all control bits to HIGH (idle)
  *cdPort |= cdPinSet; // Signals are ACTIVE LOW
  *wrPort |= wrPinSet;
  *rdPort |= rdPinSet;
#endif

  Serial.println("init pins");
  DDRD |= (1<<PD3); // Enable outputs
  DDRD |= (1<<PD4);
  DDRD |= (1<<PD5);
  DDRD |= (1<<PD6);
  CS_IDLE;
  RD_IDLE;
  WR_IDLE;
  CD_COMMAND;

  // Reset
  PORTD &= ~(1<<PD2);
  DDRD  |= (1<<PD2);
  PIND  |= (1<<PD2);

  PORTA = 0;
  setWriteDir();

  Serial.println("Reading ID reg");
  uint16_t id;
  for (int i = 0; i < 25; i++) {
    id = (uint16_t)readReg(0xD3);
    delay(50);
    Serial.print("ReadReg 0xD3 returned ");
    Serial.println(id, HEX);
    if (id == 0x9341) {
      Serial.print("ID = ");
      Serial.println(id, HEX);
      break;
    }
  }
}

void loop() {
#if 0
  CS_ACTIVE;
  delay(500);
  CS_IDLE;
  delay(500);
#endif
}