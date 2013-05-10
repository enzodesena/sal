/*
 global.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 This file contains several types, macros and supporting classes definitions.
 
 Last committed:     $Revision: 106 $
 Last changed date:  $Date: 2012-06-22 03:31:02 +0100 (Fri, 22 Jun 2012) $
 */

#ifndef SAT_TYPES_H
#define SAT_TYPES_H

#include <vector>

namespace sat {

typedef double Sample;
typedef double Time;
typedef double Speed;
typedef double Length;
typedef double Angle;
typedef unsigned long UInt;
typedef long Int;
  
typedef Sample Ild;
typedef Time Itd;
  
typedef std::vector<Sample> Signal;
  
struct StereoSignal {
  Signal left;
  Signal right;
};
  
struct EarSignalBank {
  std::vector<Signal> left_ear;
  std::vector<Signal> right_ear;
};
  
enum Ear {
  left_ear,
  right_ear
};
  
  
} // namespace sat




#endif
