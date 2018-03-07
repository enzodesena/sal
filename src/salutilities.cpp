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
  
std::vector<Angle> UniformAngles(const UInt num_elements,
                                 const Angle first_element_heading) {
  std::vector<Angle> angles(num_elements);
  for (UInt i=0; i<num_elements; ++i) {
    angles[i] = first_element_heading +
    2.0*PI/((Angle) num_elements)*((Angle) i);
  }
  return angles;
}


template<class T, class V>
std::vector<V> ConvertToType(std::vector<T> vector) {
  std::vector<V> new_vector(vector.size());
  for (mcl::UInt i=0; i<vector.size(); ++i) {
    new_vector[i] = (V) vector[i];
  }
  return new_vector;
}
  
  
} // namespace sal
