/*
 saltypes.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 This file contains several types, macros and supporting classes definitions.

 */

#ifndef SAL_TYPES_H
#define SAL_TYPES_H

#include <vector>

#include "dsptypes.h"

namespace sal {

typedef dsp::Real Sample;
typedef dsp::Real Time;
typedef dsp::Real Speed;
typedef dsp::Real Length;
typedef dsp::Real Angle;
typedef dsp::Int UInt;
typedef dsp::Int Int;

typedef std::vector<Sample> Signal;

struct StereoSignal {
  Signal left;
  Signal right;
};

enum Ear { kLeftEar, kRightEar };

enum class InterpolationType {
  kRounding, /** Rounds the latency to the nearest integer. */
  kLinear,   /** Applies fractional delays with linear interpolation.
              It reduces audible clicks, but can cause low-pass
              effect. */
};

}  // namespace sal

#endif
