/*
 comparisonop.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#ifndef MCL_EQUALITYOP_H
#define MCL_EQUALITYOP_H

#ifndef VERY_SMALL
  #define VERY_SMALL (1.0E-3)
#endif

#include "mcltypes.h"
#include <vector>


namespace mcl {


bool IsEqual(Real num_a, Real num_b, Real precision = VERY_SMALL);
  
bool IsEqual(Complex num_a, Complex num_b, Real precision = VERY_SMALL);

bool IsSmallerOrEqual(const Real num_a, const Real num_b);
  
bool AreAllSmallerOrEqual(const std::vector<Real>& vector_a,
                          const std::vector<Real>& vector_b);
  
template<class T>
bool IsEqual(const std::vector<T>& vector_a,
             const std::vector<T>& vector_b,
             Real precision = VERY_SMALL) {
  if (vector_a.size() != vector_b.size())
    return false;
  
  for (UInt i=0; i<vector_a.size(); ++i) {
    if (! IsEqual(vector_a[i], vector_b[i], precision))
      return false;
  }
  return true;
}


bool IsEqual(const std::vector<Int>& vector_a,
             const std::vector<Int>& vector_b);
  
  
/** Returns true if num is nan */
bool IsNan(Real num);

/** Returns true if num is nan */
std::vector<bool> IsNan(std::vector<Real> input);

/** Returns opposite bool as input */
std::vector<bool> Not(std::vector<bool> input);
  
/** Returns true if all bools are true */
bool All(std::vector<bool> input);

/** Returns true if any one of the bools is true */
bool Any(std::vector<bool> input);
  
/** Opposite of All: returns true if none of the inputs are true */
bool None(std::vector<bool> input);
  
bool ComparisonOpTest();
  
} // namespace mcl



#endif
