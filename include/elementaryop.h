/*
 elementaryop.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_ELEMENTARYOP_H
#define MCL_ELEMENTARYOP_H

#include "mcltypes.h"

namespace mcl {
  
template<class T>
T Max(const T& scalar_a, const T& scalar_b) {
  if (scalar_a >= scalar_b) { return scalar_a; }
  else { return scalar_b; }
}
  
template<class T>
T Min(const T& scalar_a, const T& scalar_b) {
  if (scalar_a < scalar_b) { return scalar_a; }
  else { return scalar_b; }
}

// Equivalent to Matlab's rem(scalar_a,scalar_b)
Real Rem(const Real& scalar_a, const Real& scalar_b);

// Equivalent to Matlab's mod(scalar_a,scalar_b)
Real Mod(const Real& scalar_a, const Real& scalar_b);

// Equivalent to Matlab's mod(scalar_a,scalar_b)
Int Mod(const Int& scalar_a, const Int& scalar_b);

// Equivalent to Matlab's fix(scalar)
Int Fix(const Real scalar);

// Equivalent to Matlab's abs(scalar)
Real Abs(Real input);
  
// Equivalent to Matlab's abs(scalar)
Real Abs(Complex input);
  
// Equivalent to Matlab's sign. Returns 1 if the element
// is greater than zero, 0 if it equals zero and -1 if it is less than zero.
Int Sign(const Real scalar);
  
// Returns the conjugate of the element. Equivalent to Matlab's conj(scalar).
Complex Conj(Complex scalar);

Real RealPart(Complex scalar);
  
// Equivalent to Matlab's nextpow2(input)
UInt NextPow2(Real input);
  
// This returns the next power of 2. For instance 5=>8, 12=>16, 16=>16.
UInt Next2(UInt input);
  
// Converts a string to a double
double StringToDouble(const std::string& s);
  
  
// Returns true if the imaginary part is approximately zero. The precision used
// is VERY_SMALL in equality operations, hence use only for testing.
bool IsReal(const std::vector<Complex>& input);
  
// Calculates the entropy of a discreate random variable with given `pdf'.
// It normalises the pdf if its sum is not 1.
// Note: this function is identical to Matlab's only for uint8 values.
Real Entropy(std::vector<Real> pdf, Real base);
  
bool ElementaryOpTest();
  
} // namespace mcl

#endif
