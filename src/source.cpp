/*
 source.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <vector>

#include "saltypes.h"
#include "source.h"

using sal::dsp::Point;

namespace sal {
// This class represents an acoustic source. It plays back a signal and is
// located in a given position.
// TODO: Add source directivity.
Source::Source(const dsp::Point& position, const dsp::Quaternion& orientation,
               const dsp::Handedness handedness) noexcept
  : position_(position), orientation_(orientation), handedness_(handedness) {}

Point Source::position() const noexcept { return position_; }

void Source::SetPosition(const Point& position) noexcept {
  position_ = position;
}


Point Source::GetRelativePoint(const Point& point) const noexcept {
  if (dsp::IsEqual(point, position_)) {
    dsp::Logger::GetInstance().LogError(
        "Source (%f, %f, %f) and observation point (%f, %f, %f) appear "
        "to be approximately in the same position. Behaviour undefined.",
        point.x(), point.y(), point.z(), position_.x(), position_.y(),
        position_.z());
  }
  return dsp::RotatePoint(point, position_, orientation_, handedness_);
}

/** Returns current orientation of the source */
dsp::Quaternion Source::orientation() const noexcept { return orientation_; }

/** Set source orientation */
void Source::SetOrientation(const dsp::Quaternion& orientation) noexcept {
  orientation_ = orientation;
}

void Source::SetHandedness(const dsp::Handedness handedness) noexcept {
  handedness_ = handedness;
}

}  // namespace sal
