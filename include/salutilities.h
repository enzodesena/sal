/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SALUTILITIES_H
#define SALUTILITIES_H


#include <vector>
#include "saltypes.h"
#include "comparisonop.h"
#include "digitalfilter.h"
#include "iirfilter.h"
#include "vectorop.h"
#include "point.h"
#include <iostream>


namespace sal {

std::vector<Angle> UniformAngles(const Int num_microphones,
                                 const Angle first_element_heading);

bool IsEqual(const Sample* input_data_a,
             const Sample* input_data_b,
             const Int num_samples);
  
bool IsEqual(const Sample* input_data_a,
             const std::vector<Sample> input_data_b);

bool IsEqual(const std::vector<Sample> input_data_b,
             const Sample* input_data_a);

  
template<class T, class V>
std::vector<V> ConvertToType(std::vector<T> vector) {
  std::vector<V> new_vector(vector.size());
  for (mcl::Int i=0; i<vector.size(); ++i) {
    new_vector[i] = (V) vector[i];
  }
  return new_vector;
}
  
typedef mcl::Point Triplet;
  
template< typename... argv >
void LogError(const char* format, argv... args) {
  const size_t SIZE = std::snprintf( NULL, 0, format, args... );
  
  std::string output;
  output.resize(SIZE+1);
  std::snprintf( &(output[0]), SIZE+1, format, args... );
  
  std::cerr<<output<<std::endl;
}
  
/** */
class TripletHandler {
public:
  TripletHandler(const Triplet& initial_triplet) :
        target_triplet_(initial_triplet),
        current_triplet_(initial_triplet),
        max_speed_(std::numeric_limits<Speed>::infinity()),
        has_reached_target_(true) {
    ASSERT(std::numeric_limits<Speed>::has_infinity);
    ASSERT(std::numeric_limits<Speed>::infinity() ==
           std::numeric_limits<Speed>::infinity());
  }
  
  void set_max_speed(const sal::Length& max_speed) {
    max_speed_ = max_speed;
  }
  
  /** This sets the triplet regardless of the maximum speed. */
  void set_value(const Triplet& triplet) noexcept {
    target_triplet_ = triplet;
    current_triplet_ = triplet;
    has_reached_target_ = true;
  }
  
  void set_target_value(const Triplet& target_triplet) noexcept {
    target_triplet_ = target_triplet;
    has_reached_target_ = false;
  }
  
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
        current_triplet_ = PointOnLine(current_triplet_,
                                       target_triplet_,
                                       max_speed_*time_elapsed_since_last_tick);
      }
    }
  }
  
  bool HasReachedTarget() const noexcept {
    return has_reached_target_;
  }
  
  mcl::Point value() const noexcept {
    return current_triplet_;
  }
  
  static bool Test();
  
private:
  /** This is the (un-throttled position) */
  Triplet target_triplet_;
  Triplet current_triplet_;
  sal::Length max_speed_;
  bool has_reached_target_;
};
  
  
//
//  /** Implements a first-order IIR low-pass filter with a given decay constant. */
//  class SAL_API RampSmoothingFilter : public DigitalFilter {
//  public:
//
//    /**
//     @param[in] ramp_samples number of samples after which the value is
//     to 1/e away from target value. */
//    RampSmoothingFilter(const Real ramp_samples) noexcept {
//      ASSERT_WITH_MESSAGE(isgreaterequal(ramp_samples, 0),
//                          "Decay constant cannot be negative ");
//
//
//    }
//
//    virtual Real Filter(const Real input) noexcept {
//      return filter_.Filter(input);
//    }
//
//    using DigitalFilter::Filter;
//
//    virtual void Reset() noexcept { filter_.Reset(); }
//
//
//  private:
//  };
//
  
/** Implements a first-order IIR low-pass filter with a given decay constant. */
class LowPassSmoothingFilter : public mcl::DigitalFilter {
public:
  /**
   @param[in] ramp_samples number of samples after which the value is
   to 1/e away from target value. */
  LowPassSmoothingFilter(const mcl::Real ramp_samples) noexcept {
    ASSERT_WITH_MESSAGE(isgreaterequal(ramp_samples, 0),
                        "Decay constant cannot be negative.");
    
    mcl::Real a1 = exp(-1.0/ramp_samples);
    mcl::Real b0 = 1.0 - a1;
    filter_ = mcl::IirFilter(mcl::BinaryVector(b0, 0.0),
                             mcl::BinaryVector(1.0, -a1));
  }
  
  virtual mcl::Real Filter(const mcl::Real input) noexcept {
    return filter_.Filter(input);
  }
  
  using mcl::DigitalFilter::Filter;
  
  virtual void Reset() noexcept { filter_.Reset(); }
  
private:
  mcl::IirFilter filter_;
};

} // namespace sal

#endif /* salutilities_h */
