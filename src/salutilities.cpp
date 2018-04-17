/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "salutilities.h"
#include <stdio.h>
#include <vector>

namespace sal {
  
std::vector<Angle> UniformAngles(const Int num_elements,
                                 const Angle first_element_heading) {
  std::vector<Angle> angles(num_elements);
  for (Int i=0; i<num_elements; ++i) {
    angles[i] = first_element_heading +
    2.0*PI/((Angle) num_elements)*((Angle) i);
  }
  return angles;
}

bool IsEqual(const Sample* input_data_a,
             const Sample* input_data_b,
             const Int num_samples) {
  for (Int i=0; i<num_samples; ++i) {
    if (! mcl::IsEqual(input_data_a[i], input_data_b[i])) { return false; }
  }
  return true;
}

bool IsEqual(const Sample* input_data_a,
             const std::vector<Sample> input_data_b) {
  return IsEqual(input_data_a, input_data_b.data(), input_data_b.size());
}

bool IsEqual(const std::vector<Sample> input_data_b,
             const Sample* input_data_a) {
  return IsEqual(input_data_a, input_data_b.data(), input_data_b.size());
}
  
} // namespace sal
