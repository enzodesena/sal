/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "comparisonop.h"
#include "mcltypes.h"
#include "statisticsop.h"
#include "vectorop.h"

namespace mcl {

Real Corr(const std::vector<Real>& x, const std::vector<Real>& y) {
  Real pearson_num_lin = Sum(mcl::Multiply(Add(x, -Mean(x)), Add(y, -Mean(y))));
  Real pearson_den_lin = sqrt(Sum(Pow(Add(x, -Mean(x)), 2.0))) *
                         sqrt(Sum(Pow(Add(y, -Mean(y)), 2.0)));

  return pearson_num_lin / pearson_den_lin;
}

}  // namespace mcl
