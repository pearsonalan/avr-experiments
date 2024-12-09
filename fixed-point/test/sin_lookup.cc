#include <cstdio>
#include <cmath>

// This is the floating point version of SinLookup

const float SIN_LOOKUP[] = {
    0.000000000, 0.017452406, 0.034899497, 0.052335956, 0.069756474, 0.087155743, 0.104528463, 0.121869343, 0.139173101, 0.156434465,
    0.173648178, 0.190808995, 0.207911691, 0.224951054, 0.241921896, 0.258819045, 0.275637356, 0.292371705, 0.309016994, 0.325568154,
    0.342020143, 0.358367950, 0.374606593, 0.390731128, 0.406736643, 0.422618262, 0.438371147, 0.453990500, 0.469471563, 0.484809620,
    0.500000000, 0.515038075, 0.529919264, 0.544639035, 0.559192903, 0.573576436, 0.587785252, 0.601815023, 0.615661475, 0.629320391,
    0.642787610, 0.656059029, 0.669130606, 0.681998360, 0.694658370, 0.707106781, 0.719339800, 0.731353702, 0.743144825, 0.754709580,
    0.766044443, 0.777145961, 0.788010754, 0.798635510, 0.809016994, 0.819152044, 0.829037573, 0.838670568, 0.848048096, 0.857167301,
    0.866025404, 0.874619707, 0.882947593, 0.891006524, 0.898794046, 0.906307787, 0.913545458, 0.920504853, 0.927183855, 0.933580426,
    0.939692621, 0.945518576, 0.951056516, 0.956304756, 0.961261696, 0.965925826, 0.970295726, 0.974370065, 0.978147601, 0.981627183,
    0.984807753, 0.987688341, 0.990268069, 0.992546152, 0.994521895, 0.996194698, 0.997564050, 0.998629535, 0.999390827, 0.999847695,
    1.000000000
};

double SinLookup(double x) {
    double f = floor(x);
    if (x - f >= 0.5) {
        // round up
        int deg = static_cast<int>(f) + 1;
        if (deg < 0 || deg > 90) {
            return NAN;
        }
        return SIN_LOOKUP[deg];
    } else {
        // round down
        int deg = static_cast<int>(f);
        if (deg < 0 || deg > 90) {
            return NAN;
        }
        return SIN_LOOKUP[deg];
    }
}

double SinLookupWithInterpolation(double x) {
    double f = floor(x);
    if (x - f >= 0.5) {
        // round up
        int deg = static_cast<int>(f) + 1;
        if (deg < 0 || deg > 90) {
            return NAN;
        }
        return SIN_LOOKUP[deg] - (1 - (x - f)) * (SIN_LOOKUP[deg] - SIN_LOOKUP[deg - 1]);
    } else {
        // round down
        int deg = static_cast<int>(f);
        if (deg < 0 || deg > 90) {
            return NAN;
        }
        return SIN_LOOKUP[deg] + (x - f) * (SIN_LOOKUP[deg + 1] - SIN_LOOKUP[deg]);
    }
}

inline double ToRadians(double d) {
    return d * M_PI / 180.0;
}

void test(double d) {
    printf("TEST %f\n", d);
    printf("  Lookup SIN = %0.9f\n", SinLookupWithInterpolation(d));
    printf("  Actual = %0.9f\n", sin(ToRadians(d)));
    printf("  DELTA = %0.9f\n", sin(ToRadians(d)) - SinLookupWithInterpolation(d));
    printf("\n");
}

int main() {
    test(0.25);
    test(0.5);
    test(0.75);
    test(0.9);
    test(0.95);
    test(44.5);
    test(45.5);
    test(89.5);
}
