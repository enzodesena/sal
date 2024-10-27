/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_STATISTICSOP_H
#define SAL_DSP_STATISTICSOP_H

#include <vector>

#include "mcltypes.h"
#include "pointwiseop.h"

using std::vector;

namespace sal {

namespace dsp {

/** Returns the Pearson linear correlation between `vector_a` and `vector_b` */
Real Corr(const std::vector<Real>& vector_a, const std::vector<Real>& vector_b);

bool StatisticsOpTest();

} // namespace dsp

} // namespace sal

#endif
