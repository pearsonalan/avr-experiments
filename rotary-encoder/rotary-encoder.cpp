#include <Arduino.h>

#define PIN_ENCODER_A 2
#define PIN_ENCODER_B 3

static uint8_t enc_prev_pos = 0;
static uint8_t enc_flags    = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  Serial.println("Ready");

  // set pins as input with internal pull-up resistors enabled
  pinMode(PIN_ENCODER_A, INPUT);
  pinMode(PIN_ENCODER_B, INPUT);
  digitalWrite(PIN_ENCODER_A, HIGH);
  digitalWrite(PIN_ENCODER_B, HIGH);

  // get an initial reading on the encoder pins
  if (digitalRead(PIN_ENCODER_A) == LOW) {
    enc_prev_pos |= (1 << 0);
  }
  if (digitalRead(PIN_ENCODER_B) == LOW) {
    enc_prev_pos |= (1 << 1);
  }
}

#define TEST_BIT(x, b)   ((x) & _BV(b))
#define SET_BIT(x, b)    ((x) |= _BV(b))

void loop() {
  int8_t enc_action = 0; // 1 or -1 if moved, sign is direction

  // note: for better performance, the code will now use
  // direct port access techniques
  // http://www.arduino.cc/en/Reference/PortManipulation
  uint8_t enc_cur_pos = 0;
  // read in the encoder state first
  if (digitalRead(PIN_ENCODER_A)) {
    enc_cur_pos |= (1 << 0);
  } 
  if (digitalRead(PIN_ENCODER_B)) {
    enc_cur_pos |= (1 << 1);
  }

  // Encoder Position combines the state of the two grey code
  // outputs (A, B) from the encoder and can be:
  //   A=L, B=L   00   
  //   A=L, B=H   01   
  //   A=H, B=L   10   
  //   A=H, B=H   11   
  //

  // See if there was any change of position.
  if (enc_cur_pos != enc_prev_pos) {
    Serial.print("PREV: ");
    Serial.print(enc_prev_pos, BIN);
    Serial.print(" CUR: ");
    Serial.println(enc_cur_pos, BIN);

    if (enc_prev_pos == 0b00) {
      // this is the first edge
      if (enc_cur_pos == 0b01) {
        SET_BIT(enc_flags, 0);
      } else if (enc_cur_pos == 0b10) {
        SET_BIT(enc_flags, 1);
      }
    }

    if (enc_cur_pos == 0b11) {
      // this is when the encoder is in the middle of a "step"
      SET_BIT(enc_flags, 4);
    } else if (enc_cur_pos == 0b00) {
      // this is the final edge
      if (enc_prev_pos == 0b10) {
        SET_BIT(enc_flags, 2);
      } else if (enc_prev_pos == 0b01) {
        SET_BIT(enc_flags, 3);
      }

      // check the first and last edge
      // or maybe one edge is missing, if missing then require the middle state
      // this will reject bounces and false movements
      if (TEST_BIT(enc_flags, 0) && (TEST_BIT(enc_flags, 2) || TEST_BIT(enc_flags, 4))) {
        enc_action = 1;
      } else if (TEST_BIT(enc_flags, 2) && (TEST_BIT(enc_flags, 0) || TEST_BIT(enc_flags, 4))) {
        enc_action = 1;
      } else if (TEST_BIT(enc_flags, 1) && (TEST_BIT(enc_flags, 3) || TEST_BIT(enc_flags, 4))) {
        enc_action = -1;
      } else if (TEST_BIT(enc_flags, 3) && (TEST_BIT(enc_flags, 1) || TEST_BIT(enc_flags, 4))) {
        enc_action = -1;
      }

      enc_flags = 0; // reset for next time
    }
  }

  enc_prev_pos = enc_cur_pos;

  if (enc_action > 0) {
    Serial.println("UP");
  }
  else if (enc_action < 0) {
    Serial.println("DOWN");
  }
}
