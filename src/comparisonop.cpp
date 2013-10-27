/*
 mathfunctions.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com

 */

#include "comparisonop.h"
#include "mcltypes.h"
#include "vectorop.h"
#include "pointwiseop.h"
#include <vector>


namespace mcl {


bool IsEqual(Real num_a, Real num_b, Real precision) {
  return fabs(num_a - num_b) < precision;
}
  


bool IsSmallerOrEqual(const Real num_a, const Real num_b) {
  return num_a <= (num_b + VERY_SMALL);
}
  
bool AreAllSmallerOrEqual(const std::vector<Real>& vector_a,
                          const std::vector<Real>& vector_b) {
  if (vector_a.size() != vector_b.size())
    return false;
  
  for (UInt i=0; i<vector_a.size(); ++i) {
    if (! IsSmallerOrEqual(vector_a[i], vector_b[i])) { return false; }
  }
  
  return true;
}
  
bool IsEqual(Complex num_a, Complex num_b, Real precision) {
  return (fabs(num_a.real() - num_b.real()) < precision) &
  (fabs(num_a.imag() - num_b.imag()) < precision);
}



bool IsEqual(const std::vector<Int>& vector_a,
             const std::vector<Int>& vector_b) {
  if (vector_a.size() != vector_b.size())
    return false;
  
  for (UInt i=0; i<vector_a.size(); ++i) {
    if (vector_a[i] != vector_b[i]) { return false; }
  }
  return true;
}

bool IsReal(const std::vector<Complex>& input) {
  const UInt size = input.size();
  for (UInt i=0; i<size; ++i) {
    if (! IsEqual(input[i].imag(), 0.0)) { return false; }
  }
  return true;
}
  
} // namespace mcl
