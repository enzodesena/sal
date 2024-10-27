/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "dataop.h"
#include "elementaryop.h"
#include "comparisonop.h"
#include "mcltypes.h"
#include "matrixop.h"
#include <vector>


namespace mcl {
  
void Save(const std::vector<Real>& vector,
          const std::string& file_name,
          const mcl::Int precision) {
  mcl::Matrix<Real> matrix(vector.size(), 1);
  matrix.SetColumn(0, vector);
  matrix.Save(file_name, precision);
}
  
} // namespace mcl
