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

using mcl::Point;
using mcl::Quaternion;

namespace sal {

void BinauralMic::RecordPlaneWaveRelative(const Sample& sample,
                                          const Point& point,
                                          const UInt& wave_id) {
  if (!bypass_) {
    CreateInstanceIfNotExist(wave_id);
    
    stream_.left_stream()->Add(instances_left_.at(wave_id).
                               RecordPlaneWaveRelative(sample, point));
    
    stream_.right_stream()->Add(instances_right_.at(wave_id).
                                RecordPlaneWaveRelative(sample, point));
  } else {
    stream_.left_stream()->Add(sample);
    stream_.right_stream()->Add(sample);
  }
}

void BinauralMic::RecordPlaneWaveRelative(const Signal& signal,
                                          const Point& point,
                                          const UInt& wave_id) {
  if (!bypass_) {
    CreateInstanceIfNotExist(wave_id);
    
    stream_.left_stream()->Add(instances_left_.at(wave_id).
                               RecordPlaneWaveRelative(signal, point));
    stream_.right_stream()->Add(instances_right_.at(wave_id).
                                RecordPlaneWaveRelative(signal, point));
  } else {
    stream_.left_stream()->Add(signal);
    stream_.right_stream()->Add(signal);
  }
}

void BinauralMic::set_bypass(bool bypass) {
  if (bypass_  && !bypass) { this->Reset(); }
  bypass_ = bypass;
}

void BinauralMic::CreateInstanceIfNotExist(const UInt& wave_id) {
  // If there is no instance associated to the given wave_id then create
  if (instances_left_.count(wave_id) == 0) {
    instances_left_.insert(std::make_pair(wave_id,
                                          BinauralMicInstance(this, left_ear,
                                                              update_length_)));
    instances_right_.insert(std::make_pair(wave_id,
                                           BinauralMicInstance(this, right_ear,
                                                               update_length_)));
  }
}

void BinauralMic::Tick() {
  stream_.left_stream()->Tick();
  stream_.right_stream()->Tick();
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

BinauralMic::BinauralMic(const Point& position,
                         const Quaternion orientation,
                         const UInt update_length) :
StereoMicrophone(position, orientation), update_length_(update_length),
bypass_(false) {}



// Use signals with 44100 sampling frequency!!!
Sample BinauralMicInstance::RecordPlaneWaveRelative(const Sample& sample,
                                                    const Point& point) {
  UpdateFilter(point);
  return filter_.Filter(sample);
}

Signal BinauralMicInstance::RecordPlaneWaveRelative(const Signal& signal,
                                                    const Point& point) {
  UpdateFilter(point);
  return filter_.Filter(signal);
}

void BinauralMicInstance::UpdateFilter(const Point& point) {
  if (! IsEqual(point, previous_point_)) {
    // Update cache variables
    previous_point_ = point;
    
    filter_.set_impulse_response(base_mic_->GetBrir(ear_, point),
                                 update_length_);
  }
}

DatabaseBinauralMic::DatabaseBinauralMic(const Point& position,
                                         const Quaternion orientation,
                                         const UInt update_length) :
BinauralMic(position, orientation, update_length) {}


void DatabaseBinauralMic::FilterAll(mcl::DigitalFilter* filter) {
  filter->Reset();
  for (UInt i=0; i<hrtf_database_right_.size(); ++i) {
    for (UInt j=0; j<hrtf_database_right_[i].size(); ++j) {
      hrtf_database_right_[i][j] = filter->Filter(hrtf_database_right_[i][j]);
      filter->Reset();
      hrtf_database_left_[i][j] = filter->Filter(hrtf_database_left_[i][j]);
      filter->Reset();
    }
  }
}
  
} // namespace sal
