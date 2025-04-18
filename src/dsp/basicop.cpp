/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "basicop.h"
#include "dsptypes.h"
#include "pointwiseop.h"

namespace sal {

namespace dsp {

std::vector<Real> FindPeaks(const std::vector<Real>& vector,
                            const Real min_peak_height) {
  std::vector<UInt> indexes = FindPeaksIndexes(vector, min_peak_height);
  std::vector<Real> output(indexes.size());
  for (Int i = 0; i < (Int)indexes.size(); ++i) {
    output[i] = vector[indexes[i]];
  }
  return output;
}

std::vector<UInt> FindPeaksIndexes(const std::vector<Real>& vector,
                                   const Real min_peak_height) {
  // Allocate new vectors for the indexes of the local maxima
  std::vector<UInt> indexes;
  for (Int i = 1; i < (Int)(vector.size() - 1); ++i) {
    if ((vector[i] > min_peak_height) && (vector[i] > vector[i - 1]) &&
        (vector[i] > vector[i + 1])) {
      indexes.push_back(i);
    }
  }

  return indexes;
}

template <>
Int MaxIndex<UInt>(const std::vector<UInt>& input) noexcept {
  return MinIndex(Opposite(ConvertToInt(input)));
}

Real EvaluateTrigPolynomial(const Real value, const std::vector<Real>& coefficients) {
  Real output = coefficients[0];
  for (size_t i = 1; i < coefficients.size(); ++i) {
    output += coefficients[i] * pow(cos(value), i);
  }
  return output;
}

} // namespace dsp

} // namespace sal
