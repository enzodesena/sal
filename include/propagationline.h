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
#include "iirfilter.h"
#include "point.h"
#include "firfilter.h"
#include "salconstants.h"

namespace sal {
  
enum InterpolationType {
  rounding, /** Rounds the latency to the nearest integer. */
  linear, /** Applies fractional delays with linear interpolation.
            It reduces audible clicks, but can cause low-pass
            effect. */
  linear_dynamic /** Only applies fractional delays with linear
                   interpolation when the propagation line
                   is changing length. It reduces audible clicks
                   while not adding low-pass effects. */
};
  
/** This describes a simple propagation line. It has one input and one output
 and models the delay between them. It also models attenuation between these
 two points, following 1/r rule. This object can also handle changes in the
 propagation line length.
 */
class SAL_API PropagationLine {
public:
  /**
   This constructs a `PropagationLine` object. You need to feed the `distance`
   between the two points (in [m]), the `sampling_frequency` and the maximum
   distance you expect that this propagaiton line may have.
   The value of `distance_update_step` is the length (in meters)
   by which the delay line length changes at each sample.
   A `distance_update_step` of 0 will not change the delay line length.
   The value of `gain_update_length` is the number of samples it takes 
   to update the gain. A `gain_update_length` of 0 will immediately change the
   gain of the delay line.
   */
  PropagationLine(const sal::Length distance, 
                  const sal::Time sampling_frequency, 
                  const sal::Length max_distance = 100.0,
                  const InterpolationType = rounding,
                  const bool air_filters_active = false);
  
  /** Returns the multiplicative gain of the propagation line */
  sal::Sample gain() const;
  
  sal::Length distance() const;
  
  /** This overwrites the 1/r rule attenuation. */
  void set_gain(const sal::Sample, const sal::Time ramp_time = 0.0);
  
  sal::Time current_latency() const;
  
  void set_air_filters_active(const bool);
  
  void Write(const sal::Sample &sample);
  
  /** Returns the current read sample */
  sal::Sample Read() const;
  
  /** Ticks time to next sample */
  void Tick();
  
  /**
   This resets the propagation line's length. It updates also the attenuation
   according to 1/r law. You need to be careful with this method,
   since if the distance is changed too fast, sound distortion will be
   observed.
   */
  void set_distance(const sal::Length distance,
                    const sal::Time ramp_time = 0.0);
  
  /** Returns the latency of the propagation line */
  sal::Time latency() const;
  
  /** Resets the state of the filter */
  void Reset();
  
  static bool Test();
private:
  DelayFilter delay_filter_;
  
  void Update();
  
  static sal::Time ComputeLatency(const sal::Length,
                                  const sal::Time);
  
  static sal::Sample ComputeGain(const sal::Length,
                                 const sal::Time);
  
  static std::vector<sal::Sample> GetAirFilter(sal::Length distance);
  
  sal::Time sampling_frequency_;
  
  sal::Int gain_update_counter_;
  sal::UInt gain_update_length_;
  
  sal::Sample target_gain_;
  sal::Sample current_gain_;
  sal::Sample previous_gain_;
  bool updating_gain_;
  
  sal::Int latency_update_counter_;
  sal::UInt latency_update_length_;
  sal::Time current_latency_;
  sal::Time previous_latency_;
  sal::Time target_latency_;
  bool updating_latency_;
  
  bool air_filters_active_;
  mcl::FirFilter air_filter_;
  
  InterpolationType interpolation_type_;
};

} // namespace sal
  
#endif
