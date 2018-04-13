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
#include "point.h"
#include <iostream>


namespace sal {

std::vector<Angle> UniformAngles(const UInt num_microphones,
                                 const Angle first_element_heading);


  
template<class T, class V>
std::vector<V> ConvertToType(std::vector<T> vector) {
  std::vector<V> new_vector(vector.size());
  for (mcl::UInt i=0; i<vector.size(); ++i) {
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
    assert(std::numeric_limits<Speed>::has_infinity);
    assert(std::numeric_limits<Speed>::infinity() ==
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
  

} // namespace sal

#endif /* salutilities_h */
