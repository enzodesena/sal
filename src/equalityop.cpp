/*
 mathfunctions.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
 */

#include "equalityop.h"
#include "mcltypes.h"
#include "vectorop.h"
#include "pointwiseop.h"
#include <vector>


namespace mcl {


bool IsEqual(Real num_a, Real num_b) {
  return fabs(num_a - num_b) < VERY_SMALL;
}

bool IsEqual(Complex num_a, Complex num_b) {
  return (fabs(num_a.real() - num_b.real()) < VERY_SMALL) &
  (fabs(num_a.imag() - num_b.imag()) < VERY_SMALL);
}



bool IsEqual(const std::vector<Int>& vector_a,
             const std::vector<Int>& vector_b) {
  if (vector_a.size() != vector_b.size())
    return false;
  
  for (UInt i=0; i<vector_a.size(); ++i) {
    if (vector_a[i] != vector_b[i])
      return false;
  }
  return true;
}

  
} // namespace mcl
