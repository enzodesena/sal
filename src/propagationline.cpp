/*
 propagationline.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <cstdlib>
#include <span>

#include "delayfilter.h"
#include "matrixop.h"
#include "propagationline.h"
#include "salconstants.h"
#include "salutilities.h"

using sal::Length;
using sal::Sample;
using sal::Time;

namespace sal {

const Length PropagationLine::kOneSampleDistance = NAN;

PropagationLine::PropagationLine(
    const Length distance, const Time sampling_frequency,
    const Length max_distance, const sal::InterpolationType interpolation_type,
    const bool air_filters_active, const bool allow_gain,
    const sal::Length reference_distance,
    const size_t max_expected_input_size) noexcept
    : sampling_frequency_(sampling_frequency),
      delay_filter_(DelayFilter(dsp::RoundToInt(ComputeLatency(distance)),
                                dsp::RoundToInt(ComputeLatency(max_distance)))),
      reference_distance_(isnan(reference_distance)
                              ? SOUND_SPEED / sampling_frequency
                              : reference_distance),
      allow_gain_(allow_gain),
      current_attenuation_(
          allow_gain_ ? ComputeAttenuation(distance)
                      : SanitiseAttenuation(ComputeAttenuation(distance))),
      current_latency_(ComputeLatency(distance)),
      air_filters_active_(air_filters_active),
      air_filter_(dsp::FirFilter(GetAirFilter(distance))),
      interpolation_type_(interpolation_type),
      attenuation_smoother_(
          RampSmoother(current_attenuation_, sampling_frequency)),
      latency_smoother_(RampSmoother(current_latency_, sampling_frequency)),
      scratch_vector_(max_expected_input_size, 0.0) {
  ASSERT_WITH_MESSAGE(std::isgreaterequal(sampling_frequency, 0.0),
                      "The sampling frequency cannot be negative.");
  ASSERT_WITH_MESSAGE(std::isgreaterequal(max_distance, 0.0),
                      "The maximum distance cannot be negative.");

  if (air_filters_active_) {
    air_filter_ = dsp::FirFilter(GetAirFilter(distance));
  }
}

Sample PropagationLine::SanitiseAttenuation(const sal::Sample attenuation) {
  if (std::isgreater(dsp::Abs(attenuation), 1.0)) {
    dsp::Logger::GetInstance().LogError(
        "Attempting to set the attenuation of a propagation line to %f, "
        "which has modulus larger than 1. Clipping to 1 "
        "(+-, depending on sign). If you want to "
        "bypass this check, please enable allow_gain.",
        attenuation);
    return (std::isgreater(attenuation, 0.0)) ? 1.0 : -1.0;
  } else {
    return attenuation;
  }
}

void PropagationLine::SetAttenuation(const Sample attenuation,
                                     const sal::Time ramp_time) noexcept {
  Sample attenuation_value =
      (allow_gain_) ? attenuation : SanitiseAttenuation(attenuation);

  attenuation_smoother_.SetTargetValue(attenuation_value, ramp_time);
}

void PropagationLine::SetDistance(const Length distance,
                                  const sal::Time ramp_time) noexcept {
  latency_smoother_.SetTargetValue(ComputeLatency(distance), ramp_time);
  SetAttenuation(ComputeAttenuation(distance), ramp_time);

  if (air_filters_active_) {
    air_filter_.SetImpulseResponse(GetAirFilter(distance),
                                   (int)round(ramp_time * sampling_frequency_));
  }
}

void PropagationLine::SetAirFiltersActive(
    const bool air_filters_active) noexcept {
  air_filters_active_ = air_filters_active;
  if (air_filters_active_ == false) {
    air_filter_.ResetState();
  }
}

/** Resets the state of the filter */
void PropagationLine::ResetState() noexcept { delay_filter_.ResetState(); }

void PropagationLine::Tick() noexcept { Tick(1); }

void PropagationLine::Tick(const Int num_samples) noexcept {
  current_attenuation_ = attenuation_smoother_.GetNextValue(num_samples);
  current_latency_ = latency_smoother_.GetNextValue(num_samples);
  delay_filter_.SetLatency(dsp::RoundToInt(current_latency_));
  delay_filter_.Tick(num_samples);
}

Time PropagationLine::ComputeLatency(const Length distance) noexcept {
  ASSERT_WITH_MESSAGE(std::isgreaterequal(distance, 0.0),
                      "Distance cannot be negative.");
  return (Time)(distance / SOUND_SPEED * sampling_frequency_);
}

Sample PropagationLine::ComputeAttenuation(const Length distance) noexcept {
  return (Sample)reference_distance_ / distance;
}

sal::Length PropagationLine::distance() const noexcept {
  return current_latency_ / sampling_frequency_ * SOUND_SPEED;
}

void PropagationLine::Write(const sal::Sample& sample) noexcept {
  if (air_filters_active_) {
    delay_filter_.Write(air_filter_.ProcessSample(sample));
  } else {
    delay_filter_.Write(sample);
  }
}

void PropagationLine::Write(std::span<const Sample> input_data) noexcept {
  size_t num_samples = input_data.size();
  if (air_filters_active_) {
    ASSERT(scratch_vector_.size() >= num_samples);
    air_filter_.ProcessBlock(input_data, scratch_vector_);
    delay_filter_.Write(
        std::span<const Sample>(scratch_vector_.begin(), num_samples));
  } else {
    delay_filter_.Write(input_data);
  }
}

void PropagationLine::Read(std::span<Sample> output_data) const noexcept {
  size_t num_samples = output_data.size();
  if (interpolation_type_ == sal::InterpolationType::kRounding &&
      !attenuation_smoother_.IsUpdating() && !latency_smoother_.IsUpdating()) {
    delay_filter_.Read(output_data);
    dsp::Multiply(output_data, current_attenuation_, output_data);
  } else {
    output_data[0] = Read();

    if (num_samples > 1) {
      // Create a temporary we can discard
      RampSmoother temp_attenuation(
          attenuation_smoother_);  // I believe this doesn't heap allocate
                                   // (TODO: double check please)
      RampSmoother temp_latency(latency_smoother_);

      if (interpolation_type_ == sal::InterpolationType::kLinear) {
        for (size_t i = 1; i < num_samples; ++i) {
          output_data[i] = delay_filter_.FractionalReadAt(
                               temp_latency.GetNextValue() - ((Time)i)) *
                           temp_attenuation.GetNextValue();
        }
      } else {
        for (size_t i = 1; i < num_samples; ++i) {
          output_data[i] = delay_filter_.ReadAt(
              (dsp::RoundToInt(temp_latency.GetNextValue()))-i);
        }
        temp_attenuation.GetNextValuesMultiply(
            std::span(output_data.begin() + 1, num_samples - 1),
            std::span(output_data.begin() + 1, num_samples - 1));
      }
    }
  }
}

std::vector<sal::Sample> PropagationLine::GetAirFilter(
    sal::Length distance) noexcept {
  std::vector<sal::Length> distances = {1,      1.2743, 1.6238, 2.0691, 2.6367,
                                        3.3598, 4.2813, 5.4556, 6.9519, 8.8587,
                                        11.288, 14.384, 18.33,  23.357, 29.764,
                                        37.927, 48.329, 61.585, 78.476, 100};

  Int filter_index = dsp::MinIndex(dsp::Abs(dsp::Subtract(
      distances, std::vector<sal::Length>(distances.size(), distance))));
  switch (filter_index) {  // 70% humidity N=4
    case 0:
      return {0.98968, 0.010477, -0.00015333, -2.0147e-06};
      break;
    case 1:
      return {0.98689, 0.013296, -0.00018291, -3.2688e-06};
      break;
    case 2:
      return {0.98336, 0.016855, -0.00021301, -5.3015e-06};
      break;
    case 3:
      return {0.97891, 0.021335, -0.00023927, -8.5928e-06};
      break;
    case 4:
      return {0.97331, 0.026957, -0.00025352, -1.3914e-05};
      break;
    case 5:
      return {0.96628, 0.033981, -0.00024136, -2.2495e-05};
      break;
    case 6:
      return {0.95751, 0.042709, -0.00017846, -3.6285e-05};
      break;
    case 7:
      return {0.9466, 0.05348, -2.5039e-05, -5.8314e-05};
      break;
    case 8:
      return {0.93316, 0.06665, 0.00028196, -9.3195e-05};
      break;
    case 9:
      return {0.91674, 0.082568, 0.00083926, -0.00014768};
      break;
    case 10:
      return {0.89693, 0.10152, 0.0017896, -0.00023102};
      break;
    case 11:
      return {0.87337, 0.12367, 0.0033336, -0.0003545};
      break;
    case 12:
      return {0.84593, 0.14892, 0.0057347, -0.00052874};
      break;
    case 13:
      return {0.81473, 0.17682, 0.0093063, -0.00075646};
      break;
    case 14:
      return {0.78034, 0.2065, 0.014367, -0.0010186};
      break;
    case 15:
      return {0.74378, 0.23665, 0.021154, -0.0012551};
      break;
    case 16:
      return {0.70645, 0.26573, 0.029729, -0.0013489};
      break;
    case 17:
      return {0.66967, 0.29235, 0.039945, -0.0011326};
      break;
    case 18:
      return {0.63415, 0.31585, 0.051577, -0.00042462};
      break;
    case 19:
      return {0.59941, 0.33652, 0.064613, 0.00092966};
      break;
    default:
      ASSERT(false);
      return std::vector<sal::Sample>(0, 1);
  }
}

}  // namespace sal
