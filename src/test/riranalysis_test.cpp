/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "riranalysis.h"
#include <assert.h>
#include "comparisonop.h"


namespace sal {
  
bool RirAnalysis::Test() {
  
  
  // Test EDC
  
  std::vector<sal::Sample> vector_a(4);
  vector_a[0] = -0.3;
  vector_a[1] = 30.3;
  vector_a[2] = 2.4;
  vector_a[3] = 12.4;
  
  std::vector<sal::Sample> vector_a_edc =
            RirAnalysis::EnergyDecayCurve(vector_a);
  
  std::vector<sal::Sample> vector_a_edc_cmp(4);
  vector_a_edc_cmp[0] = 0.0;
  vector_a_edc_cmp[1] = -0.000362699596754;
  vector_a_edc_cmp[2] = -8.296827418895298;
  vector_a_edc_cmp[3] = -8.456545125326402;
  
  ASSERT(mcl::IsEqual(vector_a_edc, vector_a_edc_cmp));
  
  
  std::vector<sal::Sample> vector_b(6);
  vector_b[0] = -0.3;
  vector_b[1] = 30.3;
  vector_b[2] = 2.4;
  vector_b[3] = 12.4;
  vector_b[4] = 0.0;
  vector_b[5] = 0.0;
  
  ASSERT(RirAnalysis::Rs60(vector_b) == 4);
  ASSERT(mcl::IsEqual(RirAnalysis::Rt60(vector_b, 110.0), 4.0/110.0));
  
  
  
  return true;
}
  
} // namespace sal
