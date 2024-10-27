/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "comparisonop.h"
#include "dataop.h"
#include "elementaryop.h"
#include "matrixop.h"
#include "dsptypes.h"

namespace sal {

namespace dsp {

void Save(const std::vector<Real>& vector, const std::string& file_name,
          const dsp::Int precision) {
  dsp::Matrix<Real> matrix(vector.size(), 1);
  matrix.SetColumn(0, vector);
  matrix.Save(file_name, precision);
}

} // namespace dsp

} // namespace sal
