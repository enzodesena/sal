/*
 source.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "source.h"
#include <vector>
#include "saltypes.h"
#include "stream.h"

using mcl::Point;

namespace sal {
// This class represents an acoustic source. It plays back a signal and is 
// located in a given position.
// TODO: Add source directivity.
Source::Source(const Point& position) :
        position_(TripletHandler(position)) {}

Source::Source(const Point& position, const Signal& signal) :
        position_(TripletHandler(position)) {
  stream_.Push(signal);
}

Point Source::position() const { return position_.value(); }

void Source::set_position(const Point& position) {
  position_.set_value(position);
}

void Source::set_target_position(const Point& position) {
  position_.set_target_value(position);
}
  
bool Source::HasReachedTarget() {
  return position_.HasReachedTarget();
}
  
void Source::set_max_speed(const Speed max_speed) {
  position_.set_max_speed(max_speed);
}

void Source::UpdatePosition(const Time time_elapsed_since_last_update) {
  position_.Update(time_elapsed_since_last_update);
}
  
} // namespace sal
