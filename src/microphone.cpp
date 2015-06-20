/*
 microphone.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"

namespace sal {

Point Microphone::position() const { return position_; }

void Microphone::set_position(const Point& position) {
  position_ = position;
  last_point_.clear();
}
  
  
void Microphone::set_theta(Angle theta) {
  theta_ = theta;
  last_point_.clear();
}

void Microphone::set_phi(Angle phi) {
  phi_ = phi;
  last_point_.clear();
}

void Microphone::set_psi(Angle psi) {
  psi_ = psi;
  last_point_.clear();
}

void Microphone::CalculateRelativePoint(const Point& point,
                                        const UInt& wave_id) {
  if (last_point_.count(wave_id) == 0 ||
      ! Point::IsEqual(last_point_[wave_id], point)) {
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


/**
 This method has to be defined by the microphone. This is not a pure virtual
 method because it is optional to implement this.
 */
void Microphone::RecordPlaneWaveRelative(const Signal& signal,
                                         const Point& point,
                                         const UInt& wave_id) {
  throw_line();
}


Point Microphone::GetRelativePoint(const Point& point) const {
  // Centering the reference system around the microphone
  Point centered(point.x()-position_.x(),
                 point.y()-position_.y(),
                 point.z()-position_.z());
  
  // Rotating using the 3 euler angles with ZYX convention for phi, theta and
  // psi, respectively.
  Point rotated = Point::Rotate(centered, -phi_, -theta_, -psi_);
  return rotated;
}

  
} // namespace sal
