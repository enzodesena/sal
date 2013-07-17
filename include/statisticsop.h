/*
 statisticsop.h
 MCL
 Copyright (c) 2013, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_STATISTICSOP_H
#define MCL_STATISTICSOP_H


#include "mcltypes.h"
#include "pointwiseop.h"
#include <vector>
using std::vector;

namespace mcl {
  
/** Returns the Pearson linear correlation between `vector_a` and `vector_b` */
Real Corr(const std::vector<Real>& vector_a,
          const std::vector<Real>& vector_b);

  
bool StatisticsOpTest();
  
} /**< namespace mcl */

#endif
