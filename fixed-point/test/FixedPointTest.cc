#include <stdio.h>

#include <FixedPoints.h>
#include <FixedPointsCommon.h>

void TestUQ8x8(void)
{
	printf("TestUQ8x8\n\n");
	printf("The size of UQ8x8 on your system is: %lu\n", sizeof(UQ8x8));
	printf("EPSILON = %0.9f\n", static_cast<double>(UQ8x8::Epsilon()));
	
	UQ8x8 a = 1.5;
	printf("Displaying a as float: %f\n", static_cast<float>(a));
	printf("Displaying the integer part of a: %d\n", a.getInteger());
	printf("Displaying the fractional part of b: %d\n", a.getFraction());
	printf("Displaying floorFixed(a): %f\n", static_cast<float>(floorFixed(a)));
	printf("Displaying ceilFixed(a): %f\n", static_cast<float>(ceilFixed(a)));
	printf("Displaying roundFixed(a): %f\n", static_cast<float>(roundFixed(a)));
	printf("Displaying truncFixed(a): %f\n", static_cast<float>(truncFixed(a)));
	
	UQ8x8 b = 5.25;
	printf("Displaying b as float: %f\n", static_cast<float>(b));
	printf("Displaying the integer part of b: %d\n", b.getInteger());
	printf("Displaying the fractional part of b: %d\n", b.getFraction());
	printf("Displaying floorFixed(b): %f\n", static_cast<float>(floorFixed(b)));
	printf("Displaying ceilFixed(b): %f\n", static_cast<float>(ceilFixed(b)));
	printf("Displaying roundFixed(b): %f\n", static_cast<float>(roundFixed(b)));
	printf("Displaying truncFixed(b): %f\n", static_cast<float>(truncFixed(b)));

	printf("Displaying a + b as float: %f\n", static_cast<float>(a + b));
	printf("Displaying a - b as float (Note the underflow due lack of sign bit): %f\n", static_cast<float>(a - b));
	printf("Displaying b - a as float: %f\n", static_cast<float>(b - a));
	printf("Displaying a * b as float: %f\n", static_cast<float>(a * b));
	printf("Displaying a / b as float: %f\n", static_cast<float>(a / b));
	printf("\n");
}

void TestSQ7x8(void)
{
	printf("TestSQ7x8\n\n");

	printf("The size of SQ7x8 on your system is: %lu\n", sizeof(SQ7x8));
	printf("EPSILON = %0.9f\n", static_cast<double>(SQ7x8::Epsilon()));
	printf("MAX = %0.9f\n", static_cast<double>(SQ7x8::MaxValue()));
	printf("MIN = %0.9f\n", static_cast<double>(SQ7x8::MinValue()));
	printf("PI = %0.9f\n", static_cast<double>(SQ7x8::Pi()));

	SQ7x8 a = 1.5;
	printf("Displaying a as float: %f\n", static_cast<float>(a));
	printf("Displaying the integer part of a: %d\n", a.getInteger());
	printf("Displaying the fractional part of b: %d\n", a.getFraction());
	printf("Displaying floorFixed(a): %f\n", static_cast<float>(floorFixed(a)));
	printf("Displaying ceilFixed(a): %f\n", static_cast<float>(ceilFixed(a)));
	printf("Displaying roundFixed(a): %f\n", static_cast<float>(roundFixed(a)));
	printf("Displaying truncFixed(a): %f\n", static_cast<float>(truncFixed(a)));
	
	SQ7x8 b = 5.25;
	printf("Displaying b as float: %f\n", static_cast<float>(b));
	printf("Displaying the integer part of b: %d\n", b.getInteger());
	printf("Displaying the fractional part of b: %d\n", b.getFraction());
	printf("Displaying floorFixed(b): %f\n", static_cast<float>(floorFixed(b)));
	printf("Displaying ceilFixed(b): %f\n", static_cast<float>(ceilFixed(b)));
	printf("Displaying roundFixed(b): %f\n", static_cast<float>(roundFixed(b)));
	printf("Displaying truncFixed(b): %f\n", static_cast<float>(truncFixed(b)));

	printf("Displaying a + b as float: %f\n", static_cast<float>(a + b));
	printf("Displaying a - b as float (Note this is correct due to sign bit): %f\n", static_cast<float>(a - b));
	printf("Displaying b - a as float: %f\n", static_cast<float>(b - a));
	printf("Displaying a * b as float: %f\n", static_cast<float>(a * b));
	printf("Displaying a / b as float: %f\n", static_cast<float>(a / b));
	printf("\n");
}

void TestSQ15x16(void)
{
	printf("TestSQ15x16\n\n");

	printf("The size of SQ15x16 on your system is: %lu\n", sizeof(SQ15x16));
	printf("EPSILON = %0.9f\n", static_cast<double>(SQ15x16::Epsilon()));
	printf("MAX = %0.9f\n", static_cast<double>(SQ15x16::MaxValue()));
	printf("MIN = %0.9f\n", static_cast<double>(SQ15x16::MinValue()));
	printf("PI = %0.9f\n", static_cast<double>(SQ15x16::Pi()));

	SQ15x16 a = 1.5;
	printf("Displaying a as float: %f\n", static_cast<float>(a));
	printf("Displaying the integer part of a: %d\n", a.getInteger());
	printf("Displaying the fractional part of b: %d\n", a.getFraction());
	printf("Displaying floorFixed(a): %f\n", static_cast<float>(floorFixed(a)));
	printf("Displaying ceilFixed(a): %f\n", static_cast<float>(ceilFixed(a)));
	printf("Displaying roundFixed(a): %f\n", static_cast<float>(roundFixed(a)));
	printf("Displaying truncFixed(a): %f\n", static_cast<float>(truncFixed(a)));
	
	SQ15x16 b = 5.25;
	printf("Displaying b as float: %f\n", static_cast<float>(b));
	printf("Displaying the integer part of b: %d\n", b.getInteger());
	printf("Displaying the fractional part of b: %d\n", b.getFraction());
	printf("Displaying floorFixed(b): %f\n", static_cast<float>(floorFixed(b)));
	printf("Displaying ceilFixed(b): %f\n", static_cast<float>(ceilFixed(b)));
	printf("Displaying roundFixed(b): %f\n", static_cast<float>(roundFixed(b)));
	printf("Displaying truncFixed(b): %f\n", static_cast<float>(truncFixed(b)));

	printf("Displaying a + b as float: %f\n", static_cast<float>(a + b));
	printf("Displaying a - b as float (Note this is correct due to sign bit): %f\n", static_cast<float>(a - b));
	printf("Displaying b - a as float: %f\n", static_cast<float>(b - a));
	printf("Displaying a * b as float: %f\n", static_cast<float>(a * b));
	printf("Displaying a / b as float: %f\n", static_cast<float>(a / b));

	const SQ15x16 incr = 0.001;
	printf("incr = %0.9f\n", static_cast<double>(incr));
	for (int i = 0; i < 10; i++) {
		SQ15x16 r = b + i * incr;
		printf("iter %d: r = %f\n", i, static_cast<float>(r));
	}

	printf("\n");
}


int main()
{
	TestUQ8x8();
	TestSQ7x8();
	TestSQ15x16();
}
