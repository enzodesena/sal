/*
 directivityfunction.cpp
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

void BinauralMic::FilterAll(mcl::DigitalFilter* filter) {
  filter->Reset();
  for (UInt i=0; i<NUM_ELEVATIONS; ++i) {
    for (UInt j=0; j<MAX_NUM_AZIMUTHS; ++j) {
      hrtf_database_right_[i][j] = filter->Filter(hrtf_database_right_[i][j]);
      filter->Reset();
      hrtf_database_left_[i][j] = filter->Filter(hrtf_database_left_[i][j]);
      filter->Reset();
    }
  }
}

// Use signals with 44100 sampling frequency!!!
Sample BinauralMicInstance::RecordPlaneWaveRelative(const Sample& sample,
                                                    const Point& point) {
  if (! Point::IsEqual(point, previous_point_)) {
    // Update cache variables
    previous_point_ = point;
    
    // For forward looking direction, Azimuth = 0 and elevation =0
    Point norm_point = Point::Normalized(point);
    Angle elevation = (asin(-norm_point.x())) / PI * 180.0;
    
    Angle azimuth;
    if (norm_point.z() >= 0) azimuth = (asin(norm_point.y())) / PI * 180.0;
    else azimuth = (acos(norm_point.y())+PI/2.0) / PI * 180.0;
    
    azimuth = mcl::Mod(azimuth, 360.0);
    
    assert(elevation >= (-90.0-VERY_SMALL) & elevation <= (90.0+VERY_SMALL));
    assert(azimuth >= (0.0-VERY_SMALL) & azimuth <= (360.0+VERY_SMALL));
    
    UInt elevation_index = base_mic_->FindElevationIndex(elevation);
    UInt azimuth_index = base_mic_->FindAzimuthIndex(azimuth, elevation_index);
    
    if (ear_ == left_ear) {
      filter_.UpdateFilter(
                           base_mic_->hrtf_database_left_[elevation_index][azimuth_index]);
    } else {
      filter_.UpdateFilter(
                           base_mic_->hrtf_database_right_[elevation_index][azimuth_index]);
    }
  }
  
  return filter_.Filter(sample);
}

  
  
} // namespace sal
