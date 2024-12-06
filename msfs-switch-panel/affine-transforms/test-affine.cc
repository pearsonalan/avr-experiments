#include <iostream>

#include "affine.h"

inline fixed DegreesToRadians(fixed d) {
  return (d * fixed::Pi()) / (fixed)180.0;
}

void TestRotate() {
  std::cout << "TEST ROTATE" << std::endl;

  AffineTransform r = AffineTransform::Rotate(-60.0);
  std::cout << "R:\n" << r << std::endl;

  Point2D p(0.0, 1.0);
  std::cout << "R * P:\n" << r * p << std::endl;
}

void TestRotateAndScale() {
  std::cout << "TEST ROTATE AND SCALE" << std::endl;

  AffineTransform r = AffineTransform::Rotate(-60.0);
  std::cout << "R:\n" << r << std::endl;

  AffineTransform s = AffineTransform::Scale(2.0, 2.0);
  std::cout << "S:\n" << s << std::endl;

  std::cout << "S * R:\n" << s * r << std::endl;

  Point2D p(0.0, 1.0);
  std::cout << "S * (R * P):\n" << s * (r * p) << std::endl;
  std::cout << "(S * R) * P:\n" << (s * r) * p << std::endl;
}

void TestScaleAndTranslate() {
  std::cout << "TEST SCALE AND TRANSLATE" << std::endl;

  AffineTransform s = AffineTransform::Scale(2.0, 1.0);
  std::cout << "S:\n" << s << std::endl;

  AffineTransform t = AffineTransform::Translate(4.0, 3.0);
  std::cout << "T:\n" << t << std::endl;

  Point2D p(1.0, 1.0);
  // Scale then translate
  std::cout << "T * S:\n" << t * s << std::endl;
  std::cout << "T * (S * P):\n" << t * (s * p) << std::endl;

  // Translate then scale
  std::cout << "S * T:\n" << s * t << std::endl;
  std::cout << "S * (T * P):\n" << s * (t * p) << std::endl;
}

int main() {
  //TestRotate();
  TestRotateAndScale();
  //TestScaleAndTranslate();
  return 0;
}
