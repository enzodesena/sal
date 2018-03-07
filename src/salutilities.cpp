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

  
  
} // namespace sal
