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
#include "firfilter.h"
#include "iirfilter.h"
#include "point.h"
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
   to update the attenuation. A `attenuation_update_length` of 0 will
   immediately change the attenuation of the delay line.
   */
  PropagationLine(
      const sal::Length distance, const sal::Time sampling_frequency,
      const sal::Length max_distance = 100.0,
      const sal::InterpolationType = sal::InterpolationType::kRounding,
      const bool air_filters_active = false,
      const bool allow_attenuation_larger_than_one = false,
      const sal::Length reference_distance = kOneSampleDistance,
      const size_t max_expected_input = 1 << 14) noexcept;

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

  void Write(std::span<const Sample> input_data) noexcept;

  /** Returns the current read sample */
  inline sal::Sample Read() const noexcept {
    if (interpolation_type_ == sal::InterpolationType::kLinear) {
      return delay_filter_.FractionalReadAt(current_latency_) *
             current_attenuation_;
    } else {
      return delay_filter_.ReadAt(dsp::RoundToInt(current_latency_)) *
             current_attenuation_;
    }
  }

  /** Returns a set of `num_samples` samples and writes them into `output_data`.
   @param[out] output_data the output array*/
  void Read(std::span<Sample> output_data) const noexcept;

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
  void ResetState() noexcept;

  static const Length kOneSampleDistance;

  static bool Test();

 private:
  sal::Time sampling_frequency_;
  DelayFilter delay_filter_;
  sal::Length reference_distance_; /** Distance with attenuation equal to 1 */
  /** This is true if the attenuation coefficients can be larger than 1.0 */
  bool allow_gain_;
  sal::Sample current_attenuation_;
  sal::Time current_latency_; /** Target latency [in samples]. */
  bool air_filters_active_;
  dsp::FirFilter air_filter_;
  sal::InterpolationType interpolation_type_;
  RampSmoother attenuation_smoother_;
  RampSmoother latency_smoother_;
  std::vector<Sample> scratch_vector_;

  void Update() noexcept;
  sal::Time ComputeLatency(const sal::Length) noexcept;
  sal::Sample ComputeAttenuation(const sal::Length) noexcept;

  static std::vector<sal::Sample> GetAirFilter(sal::Length distance) noexcept;
  static Sample SanitiseAttenuation(const sal::Sample attenuation);
};

}  // namespace sal

#endif
