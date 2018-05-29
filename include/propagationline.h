/*
 propagationline.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_PROPAGATIONLINE_H
#define SAL_PROPAGATIONLINE_H

#include "delayfilter.h"
#include "iirfilter.h"
#include "point.h"
#include "firfilter.h"
#include "salconstants.h"
#include "salutilities.h"

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
                  const sal::InterpolationType = sal::kRounding,
                  const bool air_filters_active = false,
                  const bool allow_attenuation_larger_than_one = false) noexcept;
  
  /** Returns the multiplicative attenuation of the propagation line */
  sal::Sample attenuation() const noexcept;
  
  sal::Length distance() const noexcept;
  
  /** This overrides the 1/r rule attenuation. */
  void SetAttenuation(const sal::Sample,
                       const sal::Time ramp_time = 0.0) noexcept;
  
  inline sal::Time current_latency() const noexcept { return current_latency_; }
  
  inline sal::Time target_latency() const noexcept {
    return latency_smoother_.target_value();
  }
  
  void SetAirFiltersActive(const bool) noexcept;
  
  void Write(const sal::Sample &sample) noexcept;
  
  void Write(const Sample* samples, const Int num_samples) noexcept;
  
  /** Returns the current read sample */
  sal::Sample Read() const noexcept;
  
  /** Returns a set of `num_samples` samples and writes them into `output_data`.
   @param[in] num_samples the number of samples to read.
   @param[out] output_data the output array*/
  void Read(const Int num_samples, Sample* output_data) const noexcept;
  
  void Tick() noexcept;
  
  /** Ticks time to next sample */
  void Tick(const Int num_samples) noexcept;
  
  /**
   This resets the propagation line's length. It updates also the attenuation
   according to 1/r law. You need to be careful with this method,
   since if the distance is changed too fast, sound distortion will be
   observed.
   */
  void SetDistance(const sal::Length distance,
                    const sal::Time ramp_time = 0.0) noexcept;
  
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
  
  static Sample SanitiseAttenuation(const sal::Sample attenuation);
  
  sal::Time sampling_frequency_;
  
  sal::Sample current_attenuation_;
  
  sal::Time current_latency_; /** Target latency [in samples]. */
  
  bool air_filters_active_;
  mcl::FirFilter air_filter_;
  
  sal::InterpolationType interpolation_type_;
  
  /** This is true if the attenuation coefficients can be larger than 1.0 */
  bool allow_gain_;
  
  RampSmoother attenuation_smoother_;
  RampSmoother latency_smoother_;
};
  

} // namespace sal
  
#endif
