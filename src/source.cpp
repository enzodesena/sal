/*
 source.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "source.h"
#include <vector>
#include "saltypes.h"

using mcl::Point;

namespace sal {
// This class represents an acoustic source. It plays back a signal and is 
// located in a given position.
// TODO: Add source directivity.
Source::Source(const Point& position) noexcept :
        position_(position) {}

Point Source::position() const noexcept { return position_; }

void Source::set_position(const Point& position) noexcept {
  position_ = position;
}

  
} // namespace sal
