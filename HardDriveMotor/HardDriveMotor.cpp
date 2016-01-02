#include <Arduino.h>

#define INITIAL_DELAY_TIME   20000

class HardDriveMotor {
private:
	// the pin numbers of the 3 output pins
	int pin[3];

	// which step (0,1,2) of the 3-phase firing cycle
	// the motor is currently in
	int currentStep;

	// how many cycles the motor has run
	int cycles;

	// which acceleration stage the motor is in
	int stage;

	// how much time to delay between phases
	int delayTime;

public:
	HardDriveMotor(int p[]) : 
		currentStep(0),
		cycles(0),
		stage(0),
		delayTime(INITIAL_DELAY_TIME)
	{
		for (int i = 0; i < 3; i++) {
			pin[i] = p[i];
			pinMode(pin[i], OUTPUT);
		}

		/* set the LEDs to OUTPUT mode */
		pinMode(8, OUTPUT);
		pinMode(9, OUTPUT);
		pinMode(10, OUTPUT);
		pinMode(11, OUTPUT);

		Serial.begin(9600);
	}

	void Step() {
		switch (currentStep) {
		case 0:
			digitalWrite(pin[0], HIGH);
			digitalWrite(pin[1], LOW);
			digitalWrite(pin[2], LOW);
			currentStep++;
			break;

		case 1:
			digitalWrite(pin[0], LOW);
			digitalWrite(pin[1], HIGH);
			digitalWrite(pin[2], LOW);
			currentStep++;
			break;

		case 2:
			digitalWrite(pin[0], LOW);
			digitalWrite(pin[1], LOW);
			digitalWrite(pin[2], HIGH);
			currentStep = 0;
			NextCycle();
		}

		Delay();
	}

	void NextCycle() {
		int stageCycles = stage < 8 ? 20 : 25;

		if (++cycles % stageCycles == 0) {
			if (delayTime > 12000) {
				delayTime = delayTime - 1000;
				stage++;
				ShowStage();
			} else if (delayTime > 4000) {
				delayTime = delayTime - 500;
				stage++;
				ShowStage();
			}
		}
	}
	
	/* show the acceleration stage int the serial out and on 
	 * the LEDs */
	void ShowStage() {
		char buf[80];
		sprintf(buf, "%d: %d", stage, delayTime);
		Serial.println(buf);

		digitalWrite(8,  (stage & 1) ? HIGH : LOW);
		digitalWrite(9,  (stage & 2) ? HIGH : LOW);
		digitalWrite(10, (stage & 4) ? HIGH : LOW);
		digitalWrite(11, (stage & 8) ? HIGH : LOW);
	}
	
	void Delay() {
		/* why doesn't delayMicroseconds(20000) properly delay for 20ms ?? */
		if (delayTime > 16000)
			delay(delayTime / 1000);
		else
			delayMicroseconds(delayTime);
	}
};

HardDriveMotor *m = NULL;

void setup() {
	int pins[3] = {5, 6, 7};
	m = new HardDriveMotor(pins);
}

void loop() {
	m->Step();
}
