/*
 basicinformation.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_BASICINFORMATION_H
#define MCL_BASICINFORMATION_H


#include "mcltypes.h"
#include "pointwiseop.h"
#include <vector>
#include <limits>

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

using std::vector;

namespace mcl {
  
/**
 Returns the index associated to the maximum value in the vector. The index
 counts starting from 0. If there are two maxima,
 the index of the first one is returned.
 */
template<class T>
MCL_API UInt MinIndex(const std::vector<T>& input) {
  T min_value = std::numeric_limits<T>::max();
  UInt min_index = 0;
  for (UInt i=0; i<input.size(); ++i) {
    if (input[i] < min_value) {
      min_value = input[i];
      min_index = i;
    }
  }
  return min_index;
}
  
/** Returns the maximum value of the vector. */
template<class T>  
MCL_API T Min(const std::vector<T>& input) {
  return input[MinIndex(input)];
}


/** 
 Returns the index associated to the maximum value in the vector. The index
 counts starting from 0. If there are two maxima, 
 the index of the first one is returned.
 */
template<class T>
MCL_API UInt MaxIndex(const std::vector<T>& input) {
  return MinIndex(Opposite(input));
}
  
template<>
MCL_API UInt MaxIndex<unsigned long>(const std::vector<unsigned long>& input);

  
/** Returns the maximum value of the vector. */
template<class T>
MCL_API T Max(const std::vector<T>& input) {
  return input[MaxIndex(input)];
}




/** 
 Returns the indexes of the local peaks in the vector.
 Equivalent to Matlab's findpeaks.
 */
std::vector<UInt>
MCL_API FindPeaksIndexes(const std::vector<Real>& vector,
                         const Real min_peak_height = std::numeric_limits<Real>::min());

/** 
 Returns the values local peaks in the vector.
 Equivalent to Matlab's findpeaks.
 */
std::vector<Real>
MCL_API FindPeaks(const std::vector<Real>& vector,
                  const Real min_peak_height = std::numeric_limits<Real>::min());

  
bool BasicOpTest();
  
} /**< namespace mcl */

#endif
