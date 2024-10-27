/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_ELEMENTARYOP_H
#define MCL_ELEMENTARYOP_H

#include "mcltypes.h"

namespace mcl {
  
template<class T>
T Max(const T& scalar_a, const T& scalar_b) noexcept {
  if (scalar_a >= scalar_b) { return scalar_a; }
  else { return scalar_b; }
}
  
template<class T>
T Min(const T& scalar_a, const T& scalar_b) noexcept {
  if (scalar_a < scalar_b) { return scalar_a; }
  else { return scalar_b; }
}

/** Equivalent to Matlab's rem(scalar_a,scalar_b) */
Real Rem(const Real& scalar_a, const Real& scalar_b);

/** Equivalent to Matlab's mod(scalar_a,scalar_b) */
Real Mod(const Real& scalar_a, const Real& scalar_b);

/** Equivalent to Matlab's mod(scalar_a,scalar_b) */
Int Mod(const Int& scalar_a, const Int& scalar_b);

/** Equivalent to Matlab's fix(scalar) */
Int Fix(const Real scalar);

/** Equivalent to Matlab's abs(scalar) */
Real Abs(Real input);
  
/** Equivalent to Matlab's abs(scalar) */
Real Abs(Complex input);
  
/** Power function. Equivalent to Matlab's input^exponent. */
Real Pow(Real input, Real exponent);
  
/** Square root function. Equivalent to Matlab's sqrt(input) */
Real Sqrt(Real input);
  
/** Equivalent to Matlab's round(input). This is faster than the standard
 C++ round function, especially on Windows. Returns an integer. */
inline Int RoundToInt(Real input) {
  Int output = static_cast<int>(input);
  output += (input-output >= 0.5) - (input-output <= -0.5);
  return output;
}
    
/** 
 Equivalent to Matlab's sign. Returns 1 if the element
 is greater than zero, 0 if it equals zero and -1 if it is less than zero.
 */
Int Sign(const Real scalar);
  
/** Returns the conjugate of the element. Equivalent to Matlab's conj(scalar). */
Complex Conj(Complex scalar);

/** Returns the real part of a complex scalar. Equivalent to Matlab's 
 real(scalar). I am calling it `RealPart' since `Real' denotes the number type */
Real RealPart(Complex scalar);
  
/** Returns the imaginary part of a complex scalar. Equivalent to Matlab's
 imag(scalar). I am calling it `ImagPart' for consistency with `RealPart' */
Real ImagPart(Complex scalar);
  
/** Equivalent to Matlab's nextpow2(input) */
Int NextPow2(Real input);
  
/** This returns the next power of 2. For instance 5=>8, 12=>16, 16=>16. */
Int Next2(Int input);
  
/** Converts a string to a double */
double StringToDouble(const std::string& s);
  
/** Equivalent to Matlab's factorial(input) */
Int Factorial(const Int input);
  
/** Linear interpolation between two values */
Real LinearInterpolation(Real x1, Real y1, Real x2, Real y2, Real x);
  
/** 
 Returns true if the imaginary part is approximately zero. The precision used
 is VERY_SMALL in equality operations, hence use only for testing.
 */
bool IsReal(const std::vector<Complex>& input);
  
/** 
 Calculates the entropy of a discreate random variable with given `pdf'.
 It normalises the pdf if its sum is not 1.
 Note: this function is identical to Matlab's only for uint8 values.
 */
Real Entropy(std::vector<Real> pdf, Real base);
  
bool ElementaryOpTest();
  
#if MCL_LOAD_BOOST
/** 
 Returns the value of the associated Legendre polynomial of degree `n' and
 order `m' of the values x. Equivalent to the m-th value of the vector
 returned by Matlab's legendre(n, x) 
 */
Real AssociatedLegendreP(Int n, Int m, Real x);
  
/** 
 Returns the value of the spherical harmonic of degree n and order m,
 and where theta is elevation, measured as the angle formed with the z-axis,
 and phi is the aximuth, measured as the angle formed with the x-axis.
 The harmonics are defined as:
 Y_\ell^m( \theta , \varphi ) =
 \sqrt{{(2\ell+1)\over 4\pi}{(\ell-m)!\over (\ell+m)!}}  \,
 P_\ell^m ( \cos{\theta} ) \, e^{i m \varphi }
 which are orthonormal
 */
Complex SphericalHarmonic(Int n, Int m, Real theta, Real phi);
#endif
  
  
} /**< namespace mcl  */

#endif
