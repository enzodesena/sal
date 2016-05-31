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
Source::Source(const Point position) : position_(position) {}

Source::Source(const Point position, const Signal signal) :
        position_(position) {
  stream_.Push(signal);
}

Point Source::position() const { return position_; }

void Source::set_position(const Point position) { position_ = position; }
  
} // namespace sal
