#include <Arduino.h>

#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <Trig.h>

void TestUQ8x8(void)
{
	Serial.print(F("The size of UQ8x8 on your system is: "));
	Serial.println(sizeof(UQ8x8));
	
	UQ8x8 a = 1.5;
	Serial.print(F("a as float: "));
	Serial.println(static_cast<float>(a));
	
	Serial.print(F("a.integer: "));
	Serial.println(a.getInteger());
	
	Serial.print(F("floorFixed(a): "));
	Serial.println(static_cast<float>(floorFixed(a)));
	
	Serial.print(F("ceilFixed(a): "));
	Serial.println(static_cast<float>(ceilFixed(a)));
	
	Serial.print(F("roundFixed(a): "));
	Serial.println(static_cast<float>(roundFixed(a)));
	
	Serial.print(F("truncFixed(a): "));
	Serial.println(static_cast<float>(truncFixed(a)));
	
	UQ8x8 b = 5.25;
	Serial.print(F("b as float: "));
	Serial.println(static_cast<float>(b));
	
	Serial.print(F("b.integer: "));
	Serial.println(b.getInteger());
	
	Serial.print(F("floorFixed(b): "));
	Serial.println(static_cast<float>(floorFixed(b)));
	
	Serial.print(F("ceilFixed(b): "));
	Serial.println(static_cast<float>(ceilFixed(b)));
	
	Serial.print(F("roundFixed(b): "));
	Serial.println(static_cast<float>(roundFixed(b)));
	
	Serial.print(F("truncFixed(b): "));
	Serial.println(static_cast<float>(truncFixed(b)));

	Serial.print(F("a + b as float: "));
	Serial.println(static_cast<float>(a + b));

	Serial.print(F("a - b as float: "));
	Serial.print(F("(Note the underflow due lack of sign bit)" ));
	Serial.println(static_cast<float>(a - b));

	Serial.print(F("b - a as float: "));
	Serial.println(static_cast<float>(b - a));
	
	Serial.print(F("a * b as float: "));
	Serial.println(static_cast<float>(a * b));
	
	Serial.print(F("a / b as float: "));
	Serial.println(static_cast<float>(a / b));
}

void TestSQ7x8(void)
{
	Serial.print(F("The size of SQ7x8 on your system is: "));
	Serial.println(sizeof(SQ7x8));
	
	SQ7x8 a = 1.5;
	Serial.print(F("a as float: "));
	Serial.println(static_cast<float>(a));
	
	Serial.print(F("a.integer: "));
	Serial.println(a.getInteger());
	
	Serial.print(F("floorFixed(a): "));
	Serial.println(static_cast<float>(floorFixed(a)));
	
	Serial.print(F("ceilFixed(a): "));
	Serial.println(static_cast<float>(ceilFixed(a)));
	
	Serial.print(F("roundFixed(a): "));
	Serial.println(static_cast<float>(roundFixed(a)));
	
	Serial.print(F("truncFixed(a): "));
	Serial.println(static_cast<float>(truncFixed(a)));
	
	SQ7x8 b = 5.25;
	Serial.print(F("b as float: "));
	Serial.println(static_cast<float>(b));
	
	Serial.print(F("b.integer: "));
	Serial.println(b.getInteger());
	
	Serial.print(F("floorFixed(b): "));
	Serial.println(static_cast<float>(floorFixed(b)));
	
	Serial.print(F("ceilFixed(b): "));
	Serial.println(static_cast<float>(ceilFixed(b)));
	
	Serial.print(F("roundFixed(b): "));
	Serial.println(static_cast<float>(roundFixed(b)));
	
	Serial.print(F("truncFixed(b): "));
	Serial.println(static_cast<float>(truncFixed(b)));

	Serial.print(F("a + b as float: "));
	Serial.println(static_cast<float>(a + b));

	Serial.print(F("a - b as float: "));
	Serial.println(static_cast<float>(a - b));

	Serial.print(F("b - a as float: "));
	Serial.println(static_cast<float>(b - a));
	
	Serial.print(F("a * b as float: "));
	Serial.println(static_cast<float>(a * b));
	
	Serial.print(F("a / b as float: "));
	Serial.println(static_cast<float>(a / b));
}

void TestTrig() {
	for (int i = 0; i <= 90; i++) {
		SQ15x16 d = i;
		Serial.print("d=");
		Serial.print(static_cast<float>(d));
		Serial.print(", sin(d)=");
		Serial.print(static_cast<float>(sinFixed(d)));
		Serial.print(", cos(d)=");
		Serial.print(static_cast<float>(cosFixed(d)));
		Serial.println();
	}
}

void setup() {
	Serial.begin(115200);
	while(!Serial) delay(10);

	TestUQ8x8();
	TestSQ7x8();

	TestTrig();

}

void loop() {}
