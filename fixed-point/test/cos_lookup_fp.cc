#include <cstdio>
#include <cmath>

#include "FixedPoints.h"
#include "FixedPointsCommon.h"
#include "Trig.h"

using fixed = SFixed<15, 16>;

inline double ToRadians(double d) {
    return d * M_PI / 180.0;
}

void test(double d) {
    printf("TEST %f\n", d);
    fixed f = d;
    printf("  Fixed = %f, f.i = %d, f.d = %d\n", static_cast<double>(f), f.getInteger(), f.getFraction());
    printf("  Lookup COS = %0.9f\n", static_cast<double>(cosFixed(f)));
    printf("  Actual = %0.9f\n", cos(ToRadians(d)));
    printf("  DELTA = %0.9f\n", cos(ToRadians(d)) - static_cast<double>(cosFixed(f)));
    printf("\n");
}

void testRange() {
    printf("Testing range from -720 to +720\n");
    for (int i = -720; i < 720; i+= 10) {
        double d = i;
        fixed f = i;

        double delta = cos(ToRadians(d)) - static_cast<double>(cosFixed(f));
        if (abs(delta) > 0.001) {
            printf("  Fixed = %f, f.i = %d, f.d = %d\n", static_cast<double>(f), f.getInteger(), f.getFraction());
            printf("  Lookup COS = %0.9f\n", static_cast<double>(cosFixed(f)));
            printf("  Actual = %0.9f\n", cos(ToRadians(d)));
            printf("  DELTA = %0.9f\n", cos(ToRadians(d)) - static_cast<double>(cosFixed(f)));
        }
    }
}

int main() {
    test(0.001);
    test(0.25);
    test(0.5);
    test(0.75);
    test(0.9);
    test(0.95);
    test(44.5);
    test(45.5);
    test(89.5);

    test(0);
    test(90);
    test(180);
    test(270);
    test(360);

    test(-45);
    test(135);
    test(-135);
    test(720+45);
    test(720+135);

    testRange();
}