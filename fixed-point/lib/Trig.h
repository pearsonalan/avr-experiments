#include "FixedPoints.h"
#include "FixedPointsCommon.h"

#ifndef ARDUINO
#define PROGMEM
#define pgm_read_float_near(p) (*(float*)(p))
#endif

extern const float PROGMEM SIN_LOOKUP[];
extern const float PROGMEM COS_LOOKUP[];

template< unsigned Integer, unsigned Fraction >
SFixed<Integer, Fraction> _sinFixedHelper(const SFixed<Integer, Fraction> & x) {
    SFixed<Integer, Fraction> f = floorFixed(x);
    if (x - f >= 0.5) {
        // round up
        int deg = static_cast<int>(f) + 1;
        SFixed<Integer,Fraction> tdeg = pgm_read_float_near(SIN_LOOKUP + deg);
        SFixed<Integer,Fraction> tdeg1 = pgm_read_float_near(SIN_LOOKUP + deg - 1);
        if (deg < 0 || deg > 90) {
            return SFixed<Integer, Fraction>::MaxValue();
        }
        return tdeg - (1 - (x - f)) * (tdeg - tdeg1);
    } else {
        // round down
        int deg = static_cast<int>(f);
        SFixed<Integer,Fraction> tdeg = pgm_read_float_near(SIN_LOOKUP + deg);
        SFixed<Integer,Fraction> tdeg1 = pgm_read_float_near(SIN_LOOKUP + deg + 1);
        if (deg < 0 || deg > 90) {
            return SFixed<Integer, Fraction>::MaxValue();
        }
        return tdeg + (x - f) * (tdeg1 - tdeg);
    }
}

template< unsigned Integer, unsigned Fraction >
SFixed<Integer, Fraction> sinFixed(SFixed<Integer, Fraction> x) {
    while (x < -180) {
        x += 360;
    }
    while (x > 180) {
        x -= 360;
    }
    if (x < -90) {
        return - _sinFixedHelper(x + 180);
    }
    if (x < 0) {
        return - _sinFixedHelper(-x);
    }
    if (x > 90) {
        return _sinFixedHelper(180 - x);
    }
    return _sinFixedHelper(x);
}

template< unsigned Integer, unsigned Fraction >
SFixed<Integer, Fraction> _cosFixedHelper(const SFixed<Integer, Fraction> & x) {
    const SFixed<Integer, Fraction> f = floorFixed(x);
    if (x - f >= 0.5) {
        // round up
        int deg = static_cast<int>(f) + 1;
        SFixed<Integer,Fraction> tdeg = pgm_read_float_near(COS_LOOKUP + deg);
        SFixed<Integer,Fraction> tdeg1 = pgm_read_float_near(COS_LOOKUP + deg - 1);
        if (deg < 0 || deg > 90) {
            return SFixed<Integer, Fraction>::MaxValue();
        }
        return tdeg - (1 - (x - f)) * (tdeg - tdeg1);
    } else {
        // round down
        int deg = static_cast<int>(f);
        SFixed<Integer,Fraction> tdeg = pgm_read_float_near(COS_LOOKUP + deg);
        SFixed<Integer,Fraction> tdeg1 = pgm_read_float_near(COS_LOOKUP + deg + 1);
        if (deg < 0 || deg > 90) {
            return SFixed<Integer, Fraction>::MaxValue();
        }
        return tdeg + (x - f) * (tdeg1 - tdeg);
    }
}

template< unsigned Integer, unsigned Fraction >
SFixed<Integer, Fraction> cosFixed(SFixed<Integer, Fraction> x) {
    while (x < -90) {
        x += 360;
    }
    while (x > 270) {
        x -= 360;
    }
    if (x < 0) {
        return _cosFixedHelper(-x);
    }
    if (x > 180) {
        return - _cosFixedHelper(x - 180);
    }
    if (x > 90) {
        return - _cosFixedHelper(180 - x);
    }
    return _cosFixedHelper(x);
}
