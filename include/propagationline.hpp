/*
 propagationline.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_PROPAGATIONLINE_H
#define SAL_PROPAGATIONLINE_H

#include "delayfilter.hpp"
#include "iirfilter.hpp"
#include "point.hpp"
#include "firfilter.hpp"
#include "salconstants.hpp"
#include "salutilities.hpp"

namespace sal
{
/** This describes a simple propagation line. It has one input and one output
 and models the delay between them. It also models attenuation between these
 two points, following 1/r rule. This object can also handle changes in the
 propagation line length.
 */
class PropagationLine
{
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
  PropagationLine(
    Length distance,
    Time sampling_frequency,
    Length max_distance = 100.0,
    InterpolationType = InterpolationType::kRounding,
    bool air_filters_active = false,
    bool allow_attenuation_larger_than_one = false,
    Length reference_distance = kOneSampleDistance) noexcept;

  /** Returns the multiplicative attenuation of the propagation line */
  Sample attenuation() const noexcept;

  Length distance() const noexcept;

  /** This overrides the 1/r rule attenuation. */
  void SetAttenuation(
    Sample,
    Time ramp_time = 0.0) noexcept;


  Time current_latency() const noexcept
  {
    return current_latency_;
  }


  Time target_latency() const noexcept
  {
    return latency_smoother_.target_value();
  }


  void SetAirFiltersActive(
    bool) noexcept;

  void Write(
    const Sample& sample) noexcept;

  void Write(
    const Sample* samples,
    Int num_samples) noexcept;

  /** Returns the current read sample */
  Sample Read() const noexcept
  {
    if (interpolation_type_ == InterpolationType::kLinear)
    {
      return delay_filter_.FractionalReadAt(current_latency_) *
        current_attenuation_;
    }
    return delay_filter_.ReadAt(mcl::RoundToInt(current_latency_)) *
      current_attenuation_;
  }


  /** Returns a set of `num_samples` samples and writes them into `output_data`.
   @param[in] num_samples the number of samples to read.
   @param[out] output_data the output array*/
  void Read(
    Int num_samples,
    Sample* output_data) const noexcept;

  void Tick() noexcept;

  /** Ticks time to next sample */
  void Tick(
    Int num_samples) noexcept;

  /**
   This resets the propagation line's length. It updates also the attenuation
   according to 1/r law. You need to be careful with this method,
   since if the distance is changed too fast, sound distortion will be
   observed.
   */
  void SetDistance(
    Length distance,
    Time ramp_time = 0.0) noexcept;

  /** Resets the state of the filter */
  void Reset() noexcept;

  static const Length kOneSampleDistance;

  static bool Test();
private:
  Time sampling_frequency_;
  DelayFilter delay_filter_;
  Length reference_distance_; /** Distance with attenuation equal to 1 */
  /** This is true if the attenuation coefficients can be larger than 1.0 */
  bool allow_gain_;
  Sample current_attenuation_;
  Time current_latency_; /** Target latency [in samples]. */
  bool air_filters_active_;
  mcl::FirFilter air_filter_;
  InterpolationType interpolation_type_;
  RampSmoother attenuation_smoother_;
  RampSmoother latency_smoother_;

  void Update() noexcept;
  Time ComputeLatency(
    Length) noexcept;
  Sample ComputeAttenuation(
    Length) noexcept;

  static mcl::Vector<Sample> GetAirFilter(
    Length distance) noexcept;
  static Sample SanitiseAttenuation(
    Sample attenuation);
};
} // namespace sal

#endif
