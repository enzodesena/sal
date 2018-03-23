/*
 microphone.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "quaternion.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
Microphone::Microphone(Point position, mcl::Quaternion orientation) :
  position_(position), orientation_(orientation),
  handedness_(mcl::right_handed) {}
  

Point Microphone::position() const { return position_; }

void Microphone::set_position(const Point& position) {
  position_ = position;
  last_point_.clear();
}
  
  
/** Returns current orientation of the microphone */
Quaternion Microphone::orientation() const { return orientation_; }
  
/** Set microphone orientation */
void Microphone::set_orientation(const mcl::Quaternion& orientation) {
  orientation_ = orientation;
  last_point_.clear();
}
  
  
void Microphone::set_handedness(const mcl::Handedness handedness) {
  handedness_ = handedness;
}
  

void Microphone::CalculateRelativePoint(const Point& point,
                                        const UInt& wave_id) {
  if (last_point_.count(wave_id) == 0 ||
      ! IsEqual(last_point_[wave_id], point)) {
    last_point_[wave_id] = point;
    last_relative_point_[wave_id] = GetRelativePoint(point);
  }
}

void Microphone::RecordPlaneWave(const Sample& sample, const Point& point,
                                 const UInt& wave_id) {
  CalculateRelativePoint(point, wave_id);
  this->RecordPlaneWaveRelative(sample, last_relative_point_[wave_id], wave_id);
}

void Microphone::RecordPlaneWave(const Signal& signal, const Point& point,
                                 const UInt& wave_id) {
  CalculateRelativePoint(point, wave_id);
  this->RecordPlaneWaveRelative(signal, last_relative_point_[wave_id], wave_id);
}


void Microphone::RecordPlaneWaveRelative(const Sample& sample,
                                         const Point& point,
                                         const UInt& wave_id) {
  RecordPlaneWaveRelative(mcl::UnaryVector(sample), point, wave_id);
}


Point Microphone::GetRelativePoint(const Point& point) const {
  // Centering the reference system around the microphone
  Point centered(point.x()-position_.x(),
                 point.y()-position_.y(),
                 point.z()-position_.z());
  
  // Instead of rotating the head, we are rotating the point in an opposite
  // direction (that's why the QuatInverse).
  Point rotated = mcl::QuatRotate(mcl::QuatInverse(orientation_), centered,
                                  handedness_);
  return rotated;
}
  
  
void Microphone::RecordPlaneWave(const Sample& sample, const Point& point) {
  RecordPlaneWave(sample, point, 0);
  Tick();
}

  
void Microphone::RecordPlaneWave(const Signal& signal, const Point& point) {
  for (UInt i=0; i<signal.size(); ++i) {
    RecordPlaneWave(signal[i], point);
  }
}

  
void Microphone::RecordPlaneWave(Source source) {
  if (! source.stream()->IsEmpty()) {
    RecordPlaneWave(source.stream()->PullAll(), source.position());
  }
}
  
  
Microphone::~Microphone() {}
  
  
} // namespace sal
