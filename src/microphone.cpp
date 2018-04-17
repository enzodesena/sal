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
  

Point Microphone::position() const noexcept { return position_.value(); }

void Microphone::set_position(const Point& position) noexcept {
  position_.set_value(position);
  last_point_.clear();
}
  
void Microphone::set_target_position(const Point& position) noexcept {
  position_.set_target_value(position);
}

bool Microphone::HasReachedTarget() noexcept {
  return position_.HasReachedTarget();
}

void Microphone::set_max_speed(const Speed max_speed) noexcept {
  position_.set_max_speed(max_speed);
}

void Microphone::UpdatePosition(const Time time_since_last_update) noexcept {
  position_.Update(time_since_last_update);
}
  
/** Returns current orientation of the microphone */
Quaternion Microphone::orientation() const noexcept { return orientation_; }
  
/** Set microphone orientation */
void Microphone::set_orientation(const mcl::Quaternion& orientation) noexcept {
  orientation_ = orientation;
  last_point_.clear();
}
  
  
void Microphone::set_handedness(const mcl::Handedness handedness) noexcept {
  handedness_ = handedness;
}
  

void Microphone::CalculateRelativePoint(const Point& point,
                                        const Int wave_id) {
  if (last_point_.count(wave_id) == 0 ||
      ! IsEqual(last_point_[wave_id], point)) {
    last_point_[wave_id] = point;
    last_relative_point_[wave_id] = GetRelativePoint(point);
  }
}

void Microphone::AddPlaneWave(const Sample input_sample,
                              const mcl::Point& point,
                              Buffer& output_buffer) noexcept {
  AddPlaneWave(&input_sample, 1, point, output_buffer);
}
  
void Microphone::AddPlaneWave(const Sample* input_data,
                              const Int num_samples,
                              const mcl::Point& point,
                              Buffer& output_buffer) noexcept {
  AddPlaneWave(input_data, num_samples, point, 0, output_buffer);
}
  
void Microphone::AddPlaneWave(const Sample input_sample,
                              const mcl::Point& point,
                              const Int wave_id,
                              Buffer& output_buffer) noexcept {
  AddPlaneWave(&input_sample, 1, point, wave_id, output_buffer);
}
  

void Microphone::AddPlaneWave(const Sample* input_data,
                              const Int num_samples,
                              const Point& point,
                              const Int wave_id,
                              Buffer& output_buffer) noexcept {
  CalculateRelativePoint(point, wave_id);
  this->AddPlaneWaveRelative(input_data,
                             num_samples,
                             last_relative_point_[wave_id],
                             wave_id,
                             output_buffer);
}

void Microphone::AddPlaneWave(const MonoBuffer& input_buffer,
                              const mcl::Point& point,
                              Buffer& output_buffer) noexcept {
  AddPlaneWave(input_buffer, point, 0, output_buffer);
}
  
void Microphone::AddPlaneWave(const MonoBuffer& input_buffer,
                              const Point& point,
                              const Int wave_id,
                              Buffer& output_buffer) noexcept {
  AddPlaneWave(input_buffer.GetReadPointer(),
               input_buffer.num_samples(),
               point, wave_id, output_buffer);
}

void Microphone::AddPlaneWaveRelative(const MonoBuffer& signal,
                                      const mcl::Point& point,
                                      const Int wave_id,
                                      Buffer& output_buffer) noexcept {
  AddPlaneWaveRelative(signal.GetReadPointer(), signal.num_samples(),
                       point, wave_id, output_buffer);
}



Point Microphone::GetRelativePoint(const Point& point) const noexcept {
  if (mcl::IsEqual(point, position_.value())) {
    LogError("Microphone (%f, %f, %f) and observation point (%f, %f, %f) appear "
             "to be approximately in the same position. Behaviour undefined.",
             point.x(), point.y(), point.z(),
             position_.value().x(),
             position_.value().y(),
             position_.value().z());
  }
  // Centering the reference system around the microphone
  Point centered(point.x()-position_.value().x(),
                 point.y()-position_.value().y(),
                 point.z()-position_.value().z());
  
  // Instead of rotating the head, we are rotating the point in an opposite
  // direction (that's why the QuatInverse).
  Point rotated = mcl::QuatRotate(mcl::QuatInverse(orientation_), centered,
                                  handedness_);
  return rotated;
}
  
  
  
  
  
} // namespace sal
