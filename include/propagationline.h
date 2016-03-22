/*
 propagationline.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_PROPAGATIONLINE_H
#define SAL_PROPAGATIONLINE_H

#include "delayfilter.h"
#include "delayfilter.h"
#include "iirfilter.h"
#include "point.h"
#include "firfilter.h"

namespace sal {
/** This describes a simple propagation line. It has one input and one output
 and models the delay between them. It also models attenuation between these
 two points, following 1/r rule. This object can also handle changes in the
 propagation line length.
 */
class PropagationLine {
public:
  /**
   This constructs a `PropagationLine` object. You need to feed the `distance`
   between the two points (in [m]), the `sampling_frequency` and the maximum
   distance you expect that this propagaiton line may have.
   */
  PropagationLine(const sal::Length distance, 
                  const sal::Time sampling_frequency, 
                  const sal::Length max_distance = 100,
                  const sal::Length update_step = 100,
                  const bool air_filters_active = false,
                  const UInt air_filters_update_step = 1);
  
  /** Returns the multiplicative gain of the propagation line */
  sal::Sample gain() const;
  
  /** This overwrites the 1/r rule attenuation. */
  void set_gain(const sal::Sample);
  
  void set_air_filters_active(const bool);
  
  void Write(const sal::Sample &sample);
  
  /** Returns the current read sample */
  inline sal::Sample Read() const { return delay_filter_.Read(); }
  
  /** Ticks time to next sample */
  void Tick();
  
  /**
   This resets the propagation line's length. It updates also the attenuation
   according to 1/r law. You need to be careful with this method,
   since if the distance is changed too fast, sound distortion will be
   observed.
   */
  void set_distance(const sal::Length distance);
  
  void set_update_step(const sal::Length update_step);
  
  /** Returns the latency of the propagation line */
  sal::Time latency() const;
  
  /** Resets the state of the filter */
  void Reset();
  
  static bool Test();
private:
  DelayFilter delay_filter_;
  static sal::Time ComputeLatency(const sal::Length,
                                  const sal::Time);
  
  static sal::Sample ComputeGain(const sal::Length,
                                 const sal::Time);
  
  static std::vector<sal::Sample> GetAirFilter(sal::Length distance);
  
  sal::Sample gain_;
  sal::Time sampling_frequency_;
  
  bool updating_distance_;
  sal::Length update_step_;
  sal::Length target_distance_;
  sal::Length current_distance_;
  
  bool air_filters_active_;
  mcl::FirFilter air_filter_;
  sal::UInt air_filters_update_step_;
};

} // namespace sal
  
#endif
