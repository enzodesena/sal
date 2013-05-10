/*
 propagationline.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 152 $
 Last changed date:  $Date: 2012-10-06 01:03:12 +0100 (Sat, 06 Oct 2012) $
 */

#ifndef SAT_PROPAGATIONLINE_H
#define SAT_PROPAGATIONLINE_H

#include "delayfilter.h"
#include "delayfilter.h"
#include "iirfilter.h"
#include "point.h"

namespace sat {
// This describes a simple propagation line. It has one input and one output
// and models the delay between them. It also models attenuation between these
// two points, following 1/r rule. This object can also handle changes in the 
// propagation line length.
class PropagationLine {
public:
  // This constructs a `PropagationLine` object. You need to feed the `distance`
  // between the two points, the `sampling_frequency` and the maximum distance
  // you expect that this propagaiton line may have.
  PropagationLine(const sat::Length distance, 
                  const sat::Time sampling_frequency, 
                  const sat::Length max_distance = 100);
  
  sat::Sample gain() const;
  
  // This overwrites the 1/r rule attenuation.
  void set_gain(const sat::Sample);
  
  inline void Write(const sat::Sample &sample) {
    delay_filter_.Write(sample);
  }
  
  inline sat::Sample Read() const { return delay_filter_.Read() * gain_; }
  
  inline void Tick() { delay_filter_.Tick(); }
  
  // This resets the propagation line's length. It updates also the attenuation
  // according to 1/r law. You need to be careful with this method, 
  // since if the distance is changed too fast, sound distortion will be 
  // observed.
  void set_distance(const sat::Length distance);
  
  sat::Time latency() const;
  
  static bool Test();
private:
  DelayFilter delay_filter_;
  static sat::Time ComputeLatency(const sat::Length,
                                      const sat::Time);
  
  static sat::Sample ComputeGain(const sat::Length,
                                     const sat::Time);
  sat::Sample gain_;
  sat::Time sampling_frequency_;
};

} // namespace sat
  
#endif
