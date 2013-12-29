/*
 delayline.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2013, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "delayline.h"
#include <cassert>

using sal::Sample;
using sal::UInt;

namespace sal {
  
DelayLine::DelayLine(sal::UInt max_latency) :
          DelayFilter(max_latency, max_latency) {}
  
sal::Sample DelayLine::Read(const UInt& delay_tap) const {
  assert(delay_tap < max_latency_);
  return ((write_index_ - delay_tap) >= start_) ?
          *(write_index_ - delay_tap) :
          *(write_index_ - delay_tap + max_latency_ + 1);
}
  
} // namespace sal



