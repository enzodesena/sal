/*
 basicinformation.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
 */

#ifndef MCL_BASICINFORMATION_H
#define MCL_BASICINFORMATION_H


#include "mcltypes.h"
#include "pointwiseop.h"
#include <vector>
using std::vector;

namespace mcl {
  
// Returns the index associated to the maximum value in the vector. The index
// counts starting from 0. If there are two maxima, the index of the first one is returned.
template<class T>
UInt MaxIndex(const std::vector<T>& input) {
  Real max_value = -std::numeric_limits<T>::infinity();
  UInt max_index;
  for (UInt i=0; i<input.size(); ++i) {
    if (input[i] > max_value) {
      max_value = input[i];
      max_index = i;
    }
  }
  return max_index;
}
  
// Returns the maximum value of the vector.
template<class T>  
T Max(const std::vector<T>& input) {
  return input[MaxIndex(input)];
}


// Returns the index associated to the maximum value in the vector. The index
// counts starting from 0. If there are two maxima, the index of the first one is returned.
template<class T>
UInt MinIndex(const std::vector<T>& input) {
  return MaxIndex(Opposite(input));
}

  
// Returns the minimum value of the vector.
template<class T>
T Min(const std::vector<T>& input) {
  return input[MinIndex(input)];
}




// Returns the indexes of the local peaks in the vector. 
// Equivalent to Matlab's findpeaks.
std::vector<UInt> FindPeaksIndexes(const std::vector<Real>& vector);

// Returns the values local peaks in the vector. Equivalent to Matlab's findpeaks
std::vector<Real> FindPeaks(const std::vector<Real>& vector);

  
bool BasicOpTest();
  
} // namespace mcl

#endif
