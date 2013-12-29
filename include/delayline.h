/*
 delayline.h
 Spatial Audio Library (SAL)
 Copyright (c) 2013, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DELAYLINE_H
#define SAL_DELAYLINE_H


#include "delayfilter.h"
#include "saltypes.h"

namespace sal {
  
class DelayLine : public DelayFilter {
  
public:
  DelayLine(sal::UInt max_latency);
  
  sal::Sample Read(const UInt& delay_tap) const;
};
  
} // namespace sal

#endif
