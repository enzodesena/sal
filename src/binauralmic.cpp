/*
 binauralmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "binauralmic.h"
#include "point.h"
#include "salconstants.h"
#include <string.h>

using mcl::Point;
using mcl::Quaternion;

namespace sal {

void BinauralMic::AddPlaneWaveRelative(const Sample* input_data,
                                       const Int num_samples,
                                       const mcl::Point& point,
                                       const Int wave_id,
                                       Buffer& output_buffer) noexcept {
  StereoBuffer& stereo_buffer =
          dynamic_cast<StereoBuffer&>(output_buffer);
  if (!bypass_) {
    CreateInstanceIfNotExist(wave_id);
    instances_.at(wave_id).AddPlaneWaveRelative(input_data, num_samples, point,
                                                stereo_buffer);
  } else {
    stereo_buffer.AddSamplesLeft(input_data, 0, num_samples);
    stereo_buffer.AddSamplesRight(input_data, 0, num_samples);
  }
}
  

void BinauralMic::set_bypass(bool bypass) noexcept {
  if (bypass_  && !bypass) { this->Reset(); }
  bypass_ = bypass;
}

void BinauralMic::CreateInstanceIfNotExist(const Int wave_id) noexcept {
  // If there is no instance associated to the given wave_id then create
  if (instances_.count(wave_id) == 0) {
    instances_.insert(std::make_pair(wave_id,
                                          BinauralMicInstance(this,
                                                              update_length_)));
  }
}


void BinauralMic::Reset() noexcept {
  for(auto iterator = instances_.begin();
      iterator != instances_.end();
      ++iterator) {
    iterator->second.filter_left_.Reset();
    iterator->second.filter_right_.Reset();
  }
}

BinauralMic::BinauralMic(const Point& position,
                         const Quaternion orientation,
                         const Int update_length,
                         const HeadRefOrientation reference_orientation) :
        StereoMicrophone(position, orientation), update_length_(update_length),
        bypass_(false), reference_orientation_(reference_orientation) {}



// Use signals with 44100 sampling frequency!!!
void BinauralMicInstance::AddPlaneWaveRelative(const Sample* input_data,
                                               const Int num_samples,
                                               const mcl::Point& point,
                                               StereoBuffer& output_buffer) noexcept {
  UpdateFilter(point);
  output_buffer.FilterAddSamplesLeft(0, num_samples, input_data, filter_left_);
  output_buffer.FilterAddSamplesRight(0, num_samples, input_data, filter_right_);
}

void BinauralMicInstance::UpdateFilter(const Point& point) noexcept {
  if (! IsEqual(point, previous_point_)) {
    // Update cache variables
    previous_point_ = point;
    
    filter_left_.set_impulse_response(base_mic_->GetBrir(left_ear, point),
                                      update_length_);
    filter_right_.set_impulse_response(base_mic_->GetBrir(right_ear, point),
                                      update_length_);
  }
}

DatabaseBinauralMic::DatabaseBinauralMic(const Point& position,
                                         const Quaternion orientation,
                                         const Int update_length,
                                         const HeadRefOrientation reference_orientation) :
BinauralMic(position, orientation, update_length, reference_orientation) {}


void DatabaseBinauralMic::FilterAll(mcl::DigitalFilter* filter) {
  filter->Reset();
  for (Int i=0; i<(Int)hrtf_database_right_.size(); ++i) {
    for (Int j=0; j<(Int)hrtf_database_right_[i].size(); ++j) {
      hrtf_database_right_[i][j] = filter->Filter(hrtf_database_right_[i][j]);
      filter->Reset();
      hrtf_database_left_[i][j] = filter->Filter(hrtf_database_left_[i][j]);
      filter->Reset();
    }
  }
}
  
} // namespace sal
