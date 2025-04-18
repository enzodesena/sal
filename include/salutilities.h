/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SALUTILITIES_H
#define SALUTILITIES_H

#include <iostream>
#include <mutex>
#include <vector>

#include "comparisonop.h"
#include "digitalfilter.h"
#include "elementaryop.h"
#include "iirfilter.h"
#include "point.h"
#include "saltypes.h"
#include "vectorop.h"

namespace sal {

enum class RotationDirection { kClockwise, kCounterclockwise };

template <class T>
std::vector<T> UniformAngles(
    const Int num_elements, const T first_element_heading,
    const RotationDirection direction =
        RotationDirection::kCounterclockwise) noexcept {
  std::vector<T> angles(num_elements);
  T separation = (T)2.0 * PI / ((Angle)num_elements);
  if (direction == RotationDirection::kClockwise) {
    separation = -separation;
  }

  for (Int i = 0; i < num_elements; ++i) {
    angles[i] = first_element_heading + separation * ((T)i);
  }
  return angles;
}

template <class T, class V>
std::vector<V> ConvertToType(std::vector<T> vector) {
  std::vector<V> new_vector(vector.size());
  for (dsp::Int i = 0; i < (Int)vector.size(); ++i) {
    new_vector[i] = (V)vector[i];
  }
  return new_vector;
}

typedef dsp::Point Triplet;

/** */
class TripletHandler {
 public:
  TripletHandler(const Triplet& initial_triplet)
      : target_triplet_(initial_triplet),
        current_triplet_(initial_triplet),
        max_speed_(std::numeric_limits<Speed>::infinity()),
        has_reached_target_(true) {
    ASSERT(std::numeric_limits<Speed>::has_infinity);
    ASSERT(std::numeric_limits<Speed>::infinity() ==
           std::numeric_limits<Speed>::infinity());
  }

  void SetMaxSpeed(const sal::Length& max_speed) { max_speed_ = max_speed; }

  /** This sets the triplet regardless of the maximum speed. */
  void SetValue(const Triplet& triplet) noexcept {
    target_triplet_ = triplet;
    current_triplet_ = triplet;
    has_reached_target_ = true;
  }

  void SetTargetValue(const Triplet& target_triplet) noexcept {
    target_triplet_ = target_triplet;
    has_reached_target_ = false;
  }

  Triplet target_value() const noexcept { return target_triplet_; }

  void Update(const Time time_elapsed_since_last_tick) {
    if (max_speed_ == std::numeric_limits<Speed>::infinity()) {
      current_triplet_ = target_triplet_;
      has_reached_target_ = true;
    } else {
      // Detect if the point is moving faster than `max_speed`
      sal::Length speed = Distance(target_triplet_, current_triplet_) /
                          time_elapsed_since_last_tick;

      if (speed <= max_speed_) {
        current_triplet_ = target_triplet_;
        has_reached_target_ = true;
      } else {
        current_triplet_ =
            PointOnLine(current_triplet_, target_triplet_,
                        max_speed_ * time_elapsed_since_last_tick);
      }
    }
  }

  bool HasReachedTarget() const noexcept { return has_reached_target_; }

  dsp::Point value() const noexcept { return current_triplet_; }

  static bool Test();

 private:
  /** This is the (un-throttled position) */
  Triplet target_triplet_;
  Triplet current_triplet_;
  sal::Length max_speed_;
  bool has_reached_target_;
};

class RampSmoother {
 public:
  /**
   @param[in] initial_value The initial assigned value. */
  RampSmoother(const Sample initial_value,
               const Time sampling_frequency) noexcept
      : current_value_(initial_value),
        target_value_(initial_value),
        step_(0.0),
        countdown_(0),
        sampling_frequency_(sampling_frequency) {
    ASSERT_WITH_MESSAGE(std::isgreaterequal(sampling_frequency, 0.0),
                        "Sampling frequency cannot be negative ");
  }

  Sample GetNextValue() noexcept {
    if (countdown_ <= 0) {
      return target_value_;
    }
    --countdown_;
    current_value_ += step_;
    return current_value_;
  }

  Sample GetNextValue(const Int num_jumps) noexcept {
    // The +1 below is to make this identical to GetNextValue()
    if ((countdown_ - num_jumps + 1) <= 0) {
      countdown_ = 0;
      return target_value_;
    } else {
      countdown_ -= num_jumps;
      current_value_ += step_ * ((Sample)num_jumps);
      return current_value_;
    }
  }

  /** Takes an array of values (`input_data`), multiplies them by the
   next values coming out of the smoother, and writes the result into
   an output array (`output_data`).
   @param[in] input_data The input data.
   @param[out] output_data The output array (data already there will be
   overwritten. */
  void GetNextValuesMultiply(std::span<const Sample> input_data,
                             std::span<Sample> output_data) noexcept {
    const size_t num_samples = input_data.size();
    if (IsUpdating()) {
      for (size_t i = 0; i < num_samples; ++i) {
        output_data[i] = input_data[i] * GetNextValue();
      }
    } else {
      dsp::Multiply(input_data, target_value_, output_data);
    }
  }

  /** Takes an array of values (`input_data`), multiplies them by the
   next values coming out of the smoother, and adds the result to
   an input-output array (`input_output_data`).
   @param[in] input_data The input data to multiply by.
   @param[in,out] input_output_data The data onto which we will add the result
   of the multiplication. */
  void GetNextValuesMultiplyAdd(std::span<const Sample> input_data,
                                std::span<Sample> input_output_data) noexcept {
    const size_t num_samples = input_data.size();
    if (IsUpdating()) {
      for (size_t i = 0; i < num_samples; ++i) {
        input_output_data[i] += input_data[i] * GetNextValue();
      }
    } else {
      for (size_t i = 0; i < num_samples; ++i) {
        input_output_data[i] += input_data[i] * target_value_;
      }
    }
  }

  /** Does the same as GetNextValuesAndMultiply, but without modifying the
   object. */
  void PredictNextValuesAndMultiply(
      std::span<const Sample> input_data,
      std::span<Sample> output_data) const noexcept {
    const size_t num_samples = input_data.size();
    if (IsUpdating()) {
      RampSmoother temp(
          *this);  // Create a copy of itself that we will discard.
      for (size_t i = 0; i < num_samples; ++i) {
        output_data[i] = input_data[i] * temp.GetNextValue();
      }
    } else {
      dsp::Multiply(input_data, target_value_, output_data);
    }
  }

  Sample target_value() const noexcept { return target_value_; }

  void SetTargetValue(const Sample target_value,
                      const Time ramp_time) noexcept {
    ASSERT_WITH_MESSAGE(std::isgreaterequal(ramp_time, 0.0),
                        "Ramp time cannot be negative ");
    if ((dsp::RoundToInt(ramp_time * sampling_frequency_)) == 0) {
      target_value_ = target_value;
      current_value_ = target_value;
      countdown_ = 0;
      return;
    }

    if (std::islessgreater(target_value, target_value_)) {
      const Int num_update_samples =
          dsp::RoundToInt(ramp_time * sampling_frequency_);
      countdown_ = num_update_samples;
      target_value_ = target_value;

      if (num_update_samples == 0) {
        current_value_ = target_value;
      } else {
        step_ = (target_value_ - current_value_) / ((Sample)num_update_samples);
      }
    }
  }

  bool IsUpdating() const noexcept { return countdown_ > 0; }

 private:
  Sample current_value_;
  Sample target_value_;
  Sample step_;
  Int countdown_;

  Time sampling_frequency_;
};

/** Implements a first-order IIR low-pass filter with a given decay constant. */
class LowPassSmoothingFilter {
 public:
  /**
   @param[in] ramp_samples number of samples after which the value is
   to 1/e away from target value. */
  LowPassSmoothingFilter(const dsp::Real ramp_samples) noexcept {
    ASSERT_WITH_MESSAGE(std::isgreaterequal(ramp_samples, 0),
                        "Decay constant cannot be negative.");

    dsp::Real a1 = exp(-1.0 / ramp_samples);
    dsp::Real b0 = 1.0 - a1;
    filter_ = dsp::IirFilter(dsp::BinaryVector<dsp::Real>(b0, 0.0),
                             dsp::BinaryVector<dsp::Real>(1.0, -a1));
  }

  dsp::Real ProcessSample(const dsp::Real input) noexcept {
    return filter_.ProcessSample(input);
  }

  void ResetState() noexcept { filter_.ResetState(); }

 private:
  dsp::IirFilter filter_;
};

template <typename T>
std::string ToString(T input) {
  std::ostringstream output;
  output << input;
  return output.str();
}

}  // namespace sal

#endif /* salutilities_h */
