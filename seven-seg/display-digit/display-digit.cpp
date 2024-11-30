#include <Arduino.h>

//  Seven Segment display:
// 
//        A
//      -----
//   F |     | B
//     |  G  |
//      -----
//   E |     | C
//     |     |
//      -----  . DP
//        D
//
// Wiring to the ATmega328:
//   A - PB0
//   B - PB1
//   C - PB2
//   D - PB3
//   E - PB4
//   F - PC0
//   G - PC1
//  DP - PC2
//

// Table of digits
//
// DIGIT    SEGMENTS        BINARY      PORTC  PORTB
//   0      A,B,C,D,E,F     110 00000   FE     E0
//   1      B,C             111 11001   FF     F9
//   2      A,B,D,E,G       101 00100   FD     E4
//   3      A,B,C,D,G       101 10000   FD     F0
//   4      B,C,F,G         100 11001   FC     F9
//   5      A,C,D,F,G       100 10010   FC     F2
//   6      A,C,D,E,F,G     100 00010   FC     E2
//   7      A,B,C           111 11000   FF     F8
//   8      A,B,C,D,E,F,G   100 00000   FC     E0
//   9      A,B,C,D,F,G     100 10000   FC     F0
//

void setup() {
  // Set PORTB[5:0] to output
  DDRB |= 0x1F;

  // Set PORTC[2:0] to output
  DDRC |= 0x07;

  // Set PORTB[5:0] to HIGH, turning off LED segments
  PORTB = 0x1F;

  // Set PORTC[3:0] to HIGH, turning off LED segments
  PORTC = 0x07;
}

static int i = 0;
void loop() {
  switch (i++) {
  case 0:
    PORTC = 0xFE;
    PORTB = 0xE0;
    break;

  case 1:
    PORTC = 0xFF;
    PORTB = 0xF9;
    break;

  case 2:
    PORTC = 0xFD;
    PORTB = 0xE4;
    break;

  case 3:
    PORTC = 0xFD;
    PORTB = 0xF0;
    break;

  case 4:
    PORTC = 0xFC;
    PORTB = 0xF9;
    break;

  case 5:
    PORTC = 0xFC;
    PORTB = 0xF2;
    break;

  case 6:
    PORTC = 0xFC;
    PORTB = 0xE2;
    break;

  case 7:
    PORTC = 0xFF;
    PORTB = 0xF8;
    break;

  case 8:
    PORTC = 0xFC;
    PORTB = 0xE0;
    break;

  case 9:
    PORTC = 0xFC;
    PORTB = 0xF0;
    i = 0;
    break;
  }

  delay(1000);
}
