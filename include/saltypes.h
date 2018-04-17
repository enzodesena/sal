/*
 saltypes.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 This file contains several types, macros and supporting classes definitions.
 
 */

#ifndef SAL_TYPES_H
#define SAL_TYPES_H

#include <vector>
#include "mcltypes.h"


namespace sal {

typedef mcl::Real Sample;
typedef mcl::Real Time;
typedef mcl::Real Speed;
typedef mcl::Real Length;
typedef mcl::Real Angle;
typedef mcl::Int UInt;
typedef mcl::Int Int;

  
typedef std::vector<Sample> Signal;
  
struct StereoSignal {
  Signal left;
  Signal right;
};
  
  
enum Ear {
  left_ear,
  right_ear
};

  
  
} // namespace sal




#endif
