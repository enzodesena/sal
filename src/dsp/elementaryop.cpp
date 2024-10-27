/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <limits>

#include "comparisonop.h"
#include "elementaryop.h"
#include "vectorop.h"

#if MCL_LOAD_BOOST
#include <boost/math/special_functions/spherical_harmonic.hpp>
#endif

namespace mcl {

Real Pow(Real input, Real exponent) {
  return (Real)pow((double)input, (double)exponent);
}

Real Sqrt(Real input) { return (Real)sqrt((double)input); }

Int Sign(const Real scalar) {
  if (IsEqual(scalar, 0.0, std::numeric_limits<Real>::epsilon())) {
    return 0;
  } else if (scalar > 0.0) {
    return 1;
  } else {
    return -1;
  }
}

Int Fix(const Real scalar) {
  if (scalar >= 0.0) {
    return (Int)floor((double)scalar);
  } else {
    return (Int)ceil((double)scalar);
  }
}

Real Rem(const Real& x, const Real& y) {
  if (IsEqual(y, 0, std::numeric_limits<Real>::epsilon())) {
    return NAN;
  }
  if (IsEqual(x, y, std::numeric_limits<Real>::epsilon())) {
    return 0.0;
  }
  Int n = Fix(x / y);
  return x - ((Real)n) * y;
}

Real Mod(const Real& x, const Real& y) {
  if (IsEqual(y, 0, std::numeric_limits<Real>::epsilon())) {
    return x;
  }
  if (IsEqual(x, y, std::numeric_limits<Real>::epsilon())) {
    return 0.0;
  }
  Real rem(Rem(x, y));
  Int signum(Sign(x / y));
  if (signum == 1 || signum == 0) {
    return rem;
  } else {
    return (rem + y);
  }
}

Int Mod(const Int& x, const Int& y) {
  if (y == 0) {
    return x;
  }
  if (x == y || x == -y) {
    return 0;
  }
  Int n = Fix(((Real)x) / ((Real)y));
  Int rem(x - n * y);
  Int signum(Sign(((Real)x) / ((Real)y)));
  if (signum == 1 || signum == 0) {
    return rem;
  } else {
    return (rem + y);
  }
}

Complex Conj(Complex scalar) { return Complex(scalar.real(), -scalar.imag()); }

Real RealPart(Complex scalar) { return scalar.real(); }

Real ImagPart(Complex scalar) { return scalar.imag(); }

Int NextPow2(Real input) {
  return (UInt)std::ceil(log2(std::fabs((double)input)));
}

double StringToDouble(const std::string& s) {
  std::istringstream i(s);
  double x;
  if (!(i >> x)) return 0;
  return x;
}

Real Abs(Real input) { return (Real)std::fabs((double)input); }

Real Abs(Complex input) { return (Real)std::abs(input); }

Int Next2(Int input) { return (UInt)pow(2, NextPow2(input)); }

Real Entropy(std::vector<Real> pdf, Real base) {
  pdf = Multiply<Real>(pdf, 1.0 / Sum(pdf));
  return -Sum(Multiply(pdf, Log(pdf))) / log(base);
}

Int Factorial(const Int input) {
  if (input <= 1) return 1;

  Int temp;
  temp = input * Factorial(input - 1);
  return temp;
}

Real LinearInterpolation(Real x0, Real y0, Real x1, Real y1, Real x) {
  Real m = (y1 - y0) / (x1 - x0);
  return y0 + (x - x0) * m;
}

#if MCL_LOAD_BOOST
Real AssociatedLegendreP(Int n, Int m, Real x) {
  ASSERT(n >= 0);  // As in Matlab we don't accept n<0
  ASSERT(m >= 0);  // As in Matlab we don't accept m<0
  ASSERT(n >= m);  // As in Matlab we don't accept n<m
  return boost::math::legendre_p<Real>((int)n, (int)m, x);
}

Complex SphericalHarmonic(Int n, Int m, Real theta, Real phi) {
  return boost::math::spherical_harmonic<Real, Real>((int)n, (int)m, theta,
                                                     phi);
}
#endif

}  // namespace mcl
