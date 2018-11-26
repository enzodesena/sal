/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "salutilities.hpp"
#include <stdio.h>
#include "vector.hpp"

namespace sal {
  
mcl::Vector<Angle> UniformAngles(const Int num_elements,
                                 const Angle first_element_heading) {
  mcl::Vector<Angle> angles(num_elements);
  for (Int i=0; i<num_elements; ++i) {
    angles[i] = first_element_heading +
    2.0*PI/((Angle) num_elements)*((Angle) i);
  }
  return angles;
}

  
} // namespace sal
