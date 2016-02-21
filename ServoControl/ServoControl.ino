#include <Servo.h>

int servoPin = 9;

int potPin = A0;

Servo myServo;

void setup() {
	// put your setup code here, to run once:
	myServo.attach(servoPin);		
}

void loop() {
	int analogValue;
	int position;

	analogValue = analogRead(potPin);

	position = map(analogValue, 0, 1023, 0, 179);
	myServo.write(position);

	delay(15);
}
