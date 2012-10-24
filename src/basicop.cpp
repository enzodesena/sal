/*
 basicinformationop.cpp
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
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
  for (UInt i=0; i<indexes.size(); ++i) { output[i] = vector[indexes[i]]; }
  return output;
}

std::vector<UInt> FindPeaksIndexes(const std::vector<Real>& vector,
                                   const Real min_peak_height) {
  // Allocate new vectors for the indexes of the local maxima
  std::vector<UInt> indexes;
  for (UInt i=1; i<(vector.size()-1); ++i) {
    if (vector[i] > min_peak_height & 
        vector[i] > vector[i-1] &
        vector[i] > vector[i+1]) {
      indexes.push_back(i);
    }
  }
  
  return indexes;
}
  
  template<>
  unsigned long MinIndex<unsigned long>(const std::vector<unsigned long>& input) {
    return MaxIndex(Opposite(ConvertToInt(input)));
  }

  
} // namespace mcl
