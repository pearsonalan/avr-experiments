#include <Arduino.h>

#define PIN_SWITCH 7
#define PIN_LED    13

int switch_state = 0;
int toggle_count = 0;
unsigned long clock;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Ready");

  // set Switch pin as input with internal pull-up resistors disabled
  pinMode(PIN_SWITCH, INPUT);

  // set LED pin as output
  pinMode(PIN_LED, OUTPUT);

  clock = millis();
}

void loop() {
	int current_state = digitalRead(PIN_SWITCH);
	if (current_state != switch_state) {
		toggle_count++;
		switch_state = current_state;
		digitalWrite(PIN_LED, switch_state);
	}
	unsigned long now = millis();
	if (now - clock >= 250) {
		Serial.print("CLOCK: ");
		Serial.print(clock);
		Serial.print(": switch=");
		Serial.print(switch_state);
		Serial.print(", toggle_count=");
		Serial.print(toggle_count);
		Serial.print("\r");
		clock = now;
	}
}
