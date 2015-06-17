/*
 mathfunctions.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "elementaryop.h"
#include "comparisonop.h"
#include "vectorop.h"
#include <limits>

#if MCL_LOAD_BOOST
#include "boost/math/special_functions/spherical_harmonic.hpp"
#endif

namespace mcl {

Real Pow(Real input, Real exponent) {
  return pow(input, exponent);
}
  
Real Sqrt(Real input) {
  return sqrt(input);
}

Int Sign(const Real scalar) {
  if (IsEqual(scalar, 0.0, std::numeric_limits<Real>::epsilon())) { return 0; }
  else if (scalar > 0.0) { return 1; }
  else { return -1; }
}

Int Fix(const Real scalar) {
  if (scalar >= 0.0) { return floor(scalar); }
  else { return ceil(scalar); }
}


Real Rem(const Real& x, const Real& y) {
  if (IsEqual(y, 0, std::numeric_limits<Real>::epsilon())) { return NAN; }
  if (IsEqual(x, y, std::numeric_limits<Real>::epsilon())) { return 0.0; }
  Int n = Fix(x/y);
  return x - ((Real) n)*y; 
}

Real Mod(const Real& x, const Real& y) {
  if (IsEqual(y, 0, std::numeric_limits<Real>::epsilon())) { return x; }
  if (IsEqual(x, y, std::numeric_limits<Real>::epsilon())) { return 0.0; }
  Real rem(Rem(x, y));
  Int signum(Sign(x/y));
  if (signum == 1 || signum == 0) { return rem; }
  else { return (rem + y); }
}



Int Mod(const Int& x, const Int& y) {
  if (y == 0) { return x; }
  if (x == y || x == -y) { return 0; }
  Int n = Fix(((Real) x)/((Real) y));
  Int rem(x - n*y);
  Int signum(Sign(((Real) x)/((Real) y)));
  if (signum == 1 || signum == 0) { return rem; }
  else { return (rem + y); }
}

  
Complex Conj(Complex scalar) {
  return Complex(scalar.real(), -scalar.imag());
}

Real RealPart(Complex scalar) {
  return scalar.real();
}
  
Real ImagPart(Complex scalar) {
  return scalar.imag();
}
  
UInt NextPow2(Real input) { return ceil(log2(fabs(input))); }

  
double StringToDouble(const std::string& s) {
  std::istringstream i(s);
  double x;
  if (!(i >> x))
    return 0;
  return x;
}
  
Real Abs(Real input) {
  return fabs(input);
}
  
Real Abs(Complex input) {
  return abs(input);
}
  
UInt Next2(UInt input) {
  return (UInt) pow(2, NextPow2(input));
}
  
Real Entropy(std::vector<Real> pdf, Real base) {
  pdf = Multiply(pdf, 1.0/Sum(pdf));
  return -Sum(Multiply(pdf, Log(pdf)))/log(base);
}
  
  
UInt Factorial(const UInt input) {
  if(input <= 1) return 1;
  
  UInt temp;
  temp = input * Factorial(input - 1);
  return temp;
}

#if MCL_LOAD_BOOST
Real AssociatedLegendreP(Int n, Int m, Real x) {
  if (n < 0) throw; // As in Matlab we don't accept n<0
  if (m < 0) throw; // As in Matlab we don't accept m<0
  if (n < m) throw; // As in Matlab we don't accept n<m
  return boost::math::legendre_p<Real>((int) n, (int) m, x);
}
  
Complex SphericalHarmonic(Int n, Int m, Real theta, Real phi) {
  return boost::math::spherical_harmonic<Real, Real>((int) n, (int) m,
                                                     theta, phi);
}
#endif
  
  
  
} // namespace mcl