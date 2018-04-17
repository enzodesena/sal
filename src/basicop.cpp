/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#include "basicop.h"
#include "mcltypes.h"
#include "pointwiseop.h"
#include <vector>


namespace mcl {
  

std::vector<Real> FindPeaks(const std::vector<Real>& vector,
                            const Real min_peak_height) {
  std::vector<UInt> indexes = FindPeaksIndexes(vector, min_peak_height);
  std::vector<Real> output(indexes.size());
  for (Int i=0; i<(Int)indexes.size(); ++i) { output[i] = vector[indexes[i]]; }
  return output;
}

std::vector<UInt> FindPeaksIndexes(const std::vector<Real>& vector,
                                   const Real min_peak_height) {
  // Allocate new vectors for the indexes of the local maxima
  std::vector<UInt> indexes;
  for (Int i=1; i<(Int)(vector.size()-1); ++i) {
    if ((vector[i] > min_peak_height) &
        (vector[i] > vector[i-1]) &
        (vector[i] > vector[i+1])) {
      indexes.push_back(i);
    }
  }
  
  return indexes;
}
  
template<>
Int MaxIndex<UInt>(const std::vector<UInt>& input) noexcept {
  return MinIndex(Opposite(ConvertToInt(input)));
}

  
} // namespace mcl
