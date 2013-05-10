/*
 microphone.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 150 $
 Last changed date:  $Date: 2012-10-05 06:11:12 +0100 (Fri, 05 Oct 2012) $
 */

#include "microphone.h"

namespace sat {

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
  
  
//std::vector<Sample> Microphone::IRPlaneWave(const Point point) const {
//  Point relative_point = GetRelativePoint(point);
//  return this->IRPlaneWaveRelative(relative_point);
//}

  
void Microphone::RecordPlaneWave(const Sample& sample, const Point& point,
                                 const UInt& wave_id) {
  if (last_point_.count(wave_id) == 0 ||
      ! Point::IsEqual(last_point_[wave_id], point)) {
    last_point_[wave_id] = point;
    last_relative_point_[wave_id] = GetRelativePoint(point);
  }
  
  this->RecordPlaneWaveRelative(sample, last_relative_point_[wave_id], wave_id);
}
  
  
//Stream Microphone::RecordPlaneWave(const Signal& input, const Point& point) {
//  std::vector<Stream> output(input.size());
//  for (UInt i=0; i<input.size(); ++i) {
//    output[i] = RecordPlaneWave(input[i], point);
//  }
//  return Stream(output);
//}

    
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
  
  
} // namespace sat
