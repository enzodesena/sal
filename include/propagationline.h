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
  

  
/** This describes a simple propagation line. It has one input and one output
 and models the delay between them. It also models attenuation between these
 two points, following 1/r rule. This object can also handle changes in the
 propagation line length.
 */
class SAL_API PropagationLine {
public:

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
  
  /**
   This constructs a `PropagationLine` object. You need to feed the `distance`
   between the two points (in [m]), the `sampling_frequency` and the maximum
   distance you expect that this propagaiton line may have.
   The value of `distance_update_step` is the length (in meters)
   by which the delay line length changes at each sample.
   A `distance_update_step` of 0 will not change the delay line length.
   The value of `attenuation_update_length` is the number of samples it takes
   to update the attenuation. A `attenuation_update_length` of 0 will immediately change the
   attenuation of the delay line.
   */
  PropagationLine(const sal::Length distance, 
                  const sal::Time sampling_frequency, 
                  const sal::Length max_distance = 100.0,
                  const InterpolationType = rounding,
                  const bool air_filters_active = false,
                  const bool allow_attenuation_larger_than_one = false) noexcept;
  
  /** Returns the multiplicative attenuation of the propagation line */
  sal::Sample attenuation() const noexcept;
  
  sal::Length distance() const noexcept;
  
  /** This overrides the 1/r rule attenuation. */
  void set_attenuation(const sal::Sample,
                       const sal::Time ramp_time = 0.0) noexcept;
  
  sal::Time current_latency() const noexcept;
  
  void set_air_filters_active(const bool) noexcept;
  
  void Write(const sal::Sample &sample) noexcept;
  
  /** Returns the current read sample */
  sal::Sample Read() const noexcept;
  
  /** Ticks time to next sample */
  void Tick() noexcept;
  
  /**
   This resets the propagation line's length. It updates also the attenuation
   according to 1/r law. You need to be careful with this method,
   since if the distance is changed too fast, sound distortion will be
   observed.
   */
  void set_distance(const sal::Length distance,
                    const sal::Time ramp_time = 0.0) noexcept;
  
  /** Returns the latency of the propagation line */
  sal::Time latency() const noexcept;
  
  /** Resets the state of the filter */
  void Reset() noexcept;
  
  
  static bool Test();
private:
  DelayFilter delay_filter_;
  
  void Update() noexcept;
  
  static sal::Time ComputeLatency(const sal::Length,
                                  const sal::Time) noexcept;
  
  static sal::Sample ComputeAttenuation(const sal::Length,
                                 const sal::Time) noexcept;
  
  static std::vector<sal::Sample> GetAirFilter(sal::Length distance) noexcept;
  
  sal::Time sampling_frequency_;
  
  sal::Int attenuation_update_counter_;
  sal::Int attenuation_update_length_;
  
  sal::Sample target_attenuation_;
  sal::Sample current_attenuation_;
  sal::Sample previous_attenuation_;
  bool updating_attenuation_;
  
  sal::Int latency_update_counter_;
  sal::Int latency_update_length_;
  sal::Time current_latency_;
  sal::Time previous_latency_;
  sal::Time target_latency_;
  bool updating_latency_;
  
  bool air_filters_active_;
  mcl::FirFilter air_filter_;
  
  InterpolationType interpolation_type_;
  
  /** This is true if the attenuation coefficients can be larger than 1.0 */
  bool allow_gain_;
  
  static Sample SanitiseAttenuation(const sal::Sample attenuation);
};
  

} // namespace sal
  
#endif
