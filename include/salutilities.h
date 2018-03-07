/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SALUTILITIES_H
#define SALUTILITIES_H


#include <vector>
#include "saltypes.h"

namespace sal {

std::vector<Angle> UniformAngles(const UInt num_microphones,
                                 const Angle first_element_heading);


  
template<class T, class V>
std::vector<V> ConvertToType(std::vector<T> vector) {
  std::vector<V> new_vector(vector.size());
  for (mcl::UInt i=0; i<vector.size(); ++i) {
    new_vector[i] = (V) vector[i];
  }
  return new_vector;
}

} // namespace sal

#endif /* salutilities_h */
