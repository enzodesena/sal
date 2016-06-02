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
  position_(position), orientation_(orientation) {
  pthread_rwlock_init(&rw_lock_, NULL);
}
  

Point Microphone::position() const { return position_; }

void Microphone::set_position(const Point& position) {
  pthread_rwlock_wrlock(&rw_lock_); // Request write lock
  position_ = position;
  last_point_.clear();
  pthread_rwlock_unlock(&rw_lock_); // Release lock
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
  pthread_rwlock_rdlock(&rw_lock_); // Request read lock
  CalculateRelativePoint(point, wave_id);
  pthread_rwlock_unlock(&rw_lock_); // Release read lock
  this->RecordPlaneWaveRelative(sample, last_relative_point_[wave_id], wave_id);
}

void Microphone::RecordPlaneWave(const Signal& signal, const Point& point,
                                 const UInt& wave_id) {
  pthread_rwlock_rdlock(&rw_lock_); // Request read lock
  CalculateRelativePoint(point, wave_id);
  pthread_rwlock_unlock(&rw_lock_); // Release read lock
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
  
  // Instead of rotating the head, we are rotating the point in an opposite
  // direction (that's why the QuatInverse).
  Point rotated = mcl::QuatRotate(mcl::QuatInverse(orientation_), centered);
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
  
  
Microphone::~Microphone() {
  pthread_rwlock_destroy(&rw_lock_);
}
  
  
} // namespace sal
