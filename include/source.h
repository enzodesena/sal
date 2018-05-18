/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_SOURCE_H
#define SAL_SOURCE_H


#include "saltypes.h"
#include "salutilities.h"
#include "point.h"
#include <vector>
#include "salconstants.h"

namespace sal {

class Source {
public:
  Source(const mcl::Point&) noexcept;
  
  mcl::Point position() const noexcept;
  
  void set_position(const mcl::Point& target_position) noexcept;
  
  void set_target_position(const mcl::Point& target_position) noexcept;
  
  void set_max_speed(const Speed max_speed) noexcept;
  
  void UpdatePosition(const Time time_elapsed_since_last_update) noexcept;
  
  bool HasReachedTarget() noexcept;
  
private:
  TripletHandler position_;
};

} // namespace sal

#endif
