/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_SOURCE_H
#define SAL_SOURCE_H


#include "saltypes.h"
#include "salutilities.h"
#include "point.h"
#include "stream.h"
#include <vector>
#include "salconstants.h"

namespace sal {

class SAL_API Source {
public:
  Source(const mcl::Point&) noexcept;
  
  Source(const mcl::Point&, const Signal& signal) noexcept;
  
  mcl::Point position() const noexcept;
  
  void set_position(const mcl::Point& target_position) noexcept;
  
  void set_target_position(const mcl::Point& target_position) noexcept;
  
  void set_max_speed(const Speed max_speed) noexcept;
  
  void UpdatePosition(const Time time_elapsed_since_last_update) noexcept;
  
  bool HasReachedTarget() noexcept;
  
  MonoStream* stream() noexcept { return &stream_; }
  
private:
  MonoStream stream_;
  TripletHandler position_;
};

} // namespace sal

#endif
