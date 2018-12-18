/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "vector.hpp"
#include "saltypes.hpp"
#include "comparisonop.hpp"
#include "digitalfilter.hpp"
#include "iirfilter.hpp"
#include "vectorop.hpp"
#include "elementaryop.hpp"
#include "point.hpp"
#include <mutex>
#include <iostream>

namespace sal
{
inline mcl::Vector<Angle> UniformAngles(
  const size_t num_elements,
  const Angle first_element_heading)
{
  mcl::Vector<Angle> angles(num_elements);
  for (size_t i = 0; i < num_elements; ++i)
  {
    angles[i] = first_element_heading +
      2.0 * PI / ((Angle)num_elements) * ((Angle)i);
  }
  return angles;
}


template<class T, class V>
mcl::Vector<V> ConvertToType(
  mcl::Vector<T> vector)
{
  mcl::Vector<V> new_vector(vector.size());
  for (mcl::Int i = 0; i < (Int)vector.size(); ++i)
  {
    new_vector[i] = (V)vector[i];
  }
  return new_vector;
}


typedef Point Triplet;


/** */
class TripletHandler
{
public:
  TripletHandler(
    const Triplet& initial_triplet)
    : target_triplet_(initial_triplet)
    , current_triplet_(initial_triplet)
    , max_speed_(std::numeric_limits<Speed>::infinity())
    , has_reached_target_(true)
  {
    ASSERT(std::numeric_limits<Speed>::has_infinity);
    ASSERT
    (
      std::numeric_limits<Speed>::infinity() ==
      std::numeric_limits<Speed>::infinity());
  }


  void SetMaxSpeed(
    const Length& max_speed)
  {
    max_speed_ = max_speed;
  }


  /** This sets the triplet regardless of the maximum speed. */
  void SetValue(
    const Triplet& triplet) noexcept
  {
    target_triplet_ = triplet;
    current_triplet_ = triplet;
    has_reached_target_ = true;
  }


  void SetTargetValue(
    const Triplet& target_triplet) noexcept
  {
    target_triplet_ = target_triplet;
    has_reached_target_ = false;
  }


  Triplet target_value() const noexcept
  {
    return target_triplet_;
  }


  void Update(
    const Time time_elapsed_since_last_tick)
  {
    if (max_speed_ == std::numeric_limits<Speed>::infinity())
    {
      current_triplet_ = target_triplet_;
      has_reached_target_ = true;
    }
    else
    {
      // Detect if the point is moving faster than `max_speed`
      Length speed = Distance(target_triplet_, current_triplet_) /
        time_elapsed_since_last_tick;

      if (speed <= max_speed_)
      {
        current_triplet_ = target_triplet_;
        has_reached_target_ = true;
      }
      else
      {
        current_triplet_ = PointOnLine
        (
          current_triplet_,
          target_triplet_,
          max_speed_ * time_elapsed_since_last_tick);
      }
    }
  }


  bool HasReachedTarget() const noexcept
  {
    return has_reached_target_;
  }


  Triplet value() const noexcept
  {
    return current_triplet_;
  }


  static bool Test();

private:
  /** This is the (un-throttled position) */
  Triplet target_triplet_;
  Triplet current_triplet_;
  Length max_speed_;
  bool has_reached_target_;
};





///** Implements a first-order IIR low-pass filter with a given decay constant. */
//class LowPassSmoothingFilter : public mcl::DigitalFilter
//{
//public:
//  /**
//   @param[in] ramp_samples number of samples after which the value is
//   to 1/e away from target value. */
//  LowPassSmoothingFilter(
//    const mcl::Real ramp_samples) noexcept
//  {
//    ASSERT_WITH_MESSAGE
//    (
//      std::isgreaterequal(ramp_samples, 0),
//      "Decay constant cannot be negative.");
//
//    mcl::Real a1 = exp(-1.0 / ramp_samples);
//    mcl::Real b0 = 1.0 - a1;
//    filter_ = mcl::IirFilter
//    (
//      mcl::Binarymcl::Vector<mcl::Real>(b0, 0.0),
//      mcl::Binarymcl::Vector<mcl::Real>(1.0, -a1));
//  }
//
//
//  virtual mcl::Real Filter(
//    const mcl::Real input) noexcept
//  {
//    return filter_.Filter(input);
//  }
//
//
//  using mcl::DigitalFilter::Filter;
//
//
//  virtual void Reset() noexcept
//  {
//    filter_.Reset();
//  }
//
//
//private:
//  mcl::IirFilter filter_;
//};


template<typename T>
std::string ToString(
  T input)
{
  std::ostringstream output;
  output << input;
  return output.str();
}
} // namespace sal

