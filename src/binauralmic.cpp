/*
 binauralmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "binauralmic.h"
#include "mcl.h"
#include "point.h"
#include "salconstants.h"
#include <string.h>

namespace sal {
  
void BinauralMic::RecordPlaneWaveRelative(const Sample& sample,
                                          const Point& point,
                                          const UInt& wave_id) {
  // If there is no instance associated to the given wave_id then create
  if (instances_left_.count(wave_id) == 0) {
    instances_left_.insert(std::make_pair(wave_id,
                                          BinauralMicInstance(this, left_ear)));
    instances_right_.insert(std::make_pair(wave_id,
                                           BinauralMicInstance(this, right_ear)));
  }
  
  stream_.Add(instances_left_.at(wave_id).
              RecordPlaneWaveRelative(sample, point),
              instances_right_.at(wave_id).
              RecordPlaneWaveRelative(sample, point));
}

void BinauralMic::Tick() {
  stream_.Tick();
}

void BinauralMic::Reset() {
  for(auto iterator = instances_left_.begin();
      iterator != instances_left_.end();
      ++iterator) {
    iterator->second.filter_.Reset();
  }
  
  for(auto iterator = instances_right_.begin();
      iterator != instances_right_.end();
      ++iterator) {
    iterator->second.filter_.Reset();
  }
}

BinauralMic::BinauralMic(Point position, Angle theta, Angle phi, Angle psi) :
  StereoMicrophone(position, theta, phi, psi) {}



// Use signals with 44100 sampling frequency!!!
Sample BinauralMicInstance::RecordPlaneWaveRelative(const Sample& sample,
                                                    const Point& point) {
  if (! Point::IsEqual(point, previous_point_)) {
    // Update cache variables
    previous_point_ = point;
    
    filter_.UpdateFilter(base_mic_->GetBrir(ear_, point));
      }
  
  return filter_.Filter(sample);
}

  
  
} // namespace sal
