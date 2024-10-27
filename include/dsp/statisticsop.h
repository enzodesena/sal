/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_STATISTICSOP_H
#define MCL_STATISTICSOP_H

#include <vector>

#include "mcltypes.h"
#include "pointwiseop.h"

using std::vector;

namespace mcl {

/** Returns the Pearson linear correlation between `vector_a` and `vector_b` */
Real Corr(const std::vector<Real>& vector_a, const std::vector<Real>& vector_b);

bool StatisticsOpTest();

}  // namespace mcl

#endif
