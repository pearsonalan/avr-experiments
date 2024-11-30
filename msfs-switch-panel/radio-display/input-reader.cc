#include "input-reader.h"

void InputReader::CheckSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    switch (c) {
      case 'u':
        Serial.println();
        Serial.println("UP");
        radio_.Up();
        break;

      case 'd':
        Serial.println();
        Serial.println("DOWN");
        radio_.Down();
        break;

      case 's':
        Serial.println();
        Serial.println("SWAP");
        radio_.Swap();
        break;

      case 't':
        Serial.println();
        Serial.println("TOGL");
        radio_.Toggle();
        break;

      default:
        Serial.println();
        Serial.println("UNK");
        break;
    }
  }
}
