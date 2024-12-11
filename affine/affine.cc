#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifdef AFFINE_IOSTREAMS
#include <iostream>
#endif

#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <Trig.h>

#include "affine.h"

AffineTransform AffineTransform::Scale(fixed x, fixed y) {
  return AffineTransform(x, 0, 0, y, 0, 0);
}

AffineTransform AffineTransform::Translate(fixed x, fixed y) {
  return AffineTransform(1, 0, 0, 1, x, y);
}

AffineTransform AffineTransform::Rotate(fixed theta) {
  return AffineTransform(cosFixed(theta), sinFixed(theta),
                        -sinFixed(theta), cosFixed(theta), 0, 0);
}

AffineTransform operator*(const AffineTransform& m1, const AffineTransform& m2) {
  return AffineTransform( m1.a() * m2.a() + m1.c() * m2.b(),
                          m1.b() * m2.a() + m1.d() * m2.b(),
                          m1.a() * m2.c() + m1.c() * m2.d(),
                          m1.b() * m2.c() + m1.d() * m2.d(),
                          m1.a() * m2.e() + m1.c() * m2.f() + m1.e(),
                          m1.b() * m2.e() + m1.d() * m2.f() + m1.f());
}

Point2D operator*(const AffineTransform& m, const Point2D& p) {
  return Point2D(p.x() * m.a() + p.y() * m.c() + m.e(),
                 p.x() * m.b() + p.y() * m.d() + m.f());
}

#ifdef AFFINE_IOSTREAMS
std::ostream& operator << (std::ostream& os, const AffineTransform& m) {
  os << (float)m.a() << "\t" << (float)m.c() << "\t" << (float)m.e() << std::endl;
  os << (float)m.b() << "\t" << (float)m.d() << "\t" << (float)m.f() << std::endl;
  os << "0\t0\t1" << std::endl;
  return os;
}

std::ostream& operator << (std::ostream& os, const Point2D& p) {
  os << (float)p.x() << std::endl;
  os << (float)p.y() << std::endl;
  os << "1" << std::endl;
  return os;
}
#endif
