#ifndef _affine_h_
#define _affine_h_

#ifdef AFFINE_IOSTREAMS
#include <iostream>
#endif

#include <FixedPoints.h>
#include <FixedPointsCommon.h>

using fixed = SFixed<15, 16>;

//
// Represents the transorm matrix:
//
//  a  c  e
//  b  d  f
//  0  0  1
//
class AffineTransform {
public:
  AffineTransform(fixed a, fixed b, fixed c, fixed d, fixed e, fixed f) :
      a_(a), b_(b), c_(c), d_(d), e_(e), f_(f) {}

  const fixed& a() const { return a_; }
  const fixed& b() const { return b_; }
  const fixed& c() const { return c_; }
  const fixed& d() const { return d_; }
  const fixed& e() const { return e_; }
  const fixed& f() const { return f_; }

  static AffineTransform Scale(fixed x, fixed y);
  static AffineTransform Translate(fixed x, fixed y);
  static AffineTransform Rotate(fixed theta);

protected:
  fixed a_;
  fixed b_;
  fixed c_;
  fixed d_;
  fixed e_;
  fixed f_;
};

class Point2D {
public:
  Point2D(fixed x, fixed y) : x_(x), y_(y) {}

  const fixed& x() const { return x_; }
  const fixed& y() const { return y_; }

protected:
  fixed x_;
  fixed y_;
};

AffineTransform operator*(const AffineTransform& m1, const AffineTransform& m2);

Point2D operator*(const AffineTransform& m, const Point2D& p);

#ifdef AFFINE_IOSTREAMS
std::ostream& operator << (std::ostream& os, const AffineTransform& m);
std::ostream& operator << (std::ostream& os, const Point2D& p);
#endif

#endif
