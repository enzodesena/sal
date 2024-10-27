/*
 binauralmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <string.h>

#include "binauralmic.h"
#include "point.h"
#include "salconstants.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {

void BinauralMic::AddPlaneWaveRelative(std::span<const Sample> input_data,
                                       const mcl::Point& point,
                                       const size_t wave_id,
                                       Buffer& output_buffer) noexcept {
  if (bypass_) {
    mcl::Add(input_data, output_buffer.GetReadView(Buffer::kLeftChannel),
             output_buffer.GetWriteView(Buffer::kLeftChannel));
    mcl::Add(input_data, output_buffer.GetReadView(Buffer::kRightChannel),
             output_buffer.GetWriteView(Buffer::kRightChannel));
  } else {
    CreateInstanceIfNotExist(wave_id);
    instances_.at(wave_id).AddPlaneWaveRelative(input_data, point,
                                                output_buffer);
  }
}

void BinauralMic::SetBypass(bool bypass) noexcept {
  if (bypass_ && !bypass) {
    this->Reset();
  }
  bypass_ = bypass;
}

void BinauralMic::CreateInstanceIfNotExist(const size_t wave_id) noexcept {
  // If there is no instance associated to the given wave_id then create
  if (instances_.count(wave_id) == 0) {
    instances_.insert(
        std::make_pair(wave_id, BinauralMicInstance(this, update_length_)));
  }
}

void BinauralMic::Reset() noexcept {
  for (auto iterator = instances_.begin(); iterator != instances_.end();
       ++iterator) {
    iterator->second.filter_left_.Reset();
    iterator->second.filter_right_.Reset();
  }
}

BinauralMic::BinauralMic(const Point& position, const Quaternion orientation,
                         const Int update_length,
                         const HeadRefOrientation reference_orientation)
    : StereoMicrophone(position, orientation),
      update_length_(update_length),
      bypass_(false),
      reference_orientation_(reference_orientation) {}

void BinauralMicInstance::AddPlaneWaveRelative(
    std::span<const Sample> input_data, const mcl::Point& point,
    Buffer& output_buffer) noexcept {
  UpdateFilter(point);

  filter_left_.ProcessBlock(input_data, scratch_vector_);
  mcl::Add(scratch_vector_, output_buffer.GetReadView(Buffer::kLeftChannel),
           output_buffer.GetWriteView(Buffer::kLeftChannel));

  filter_right_.ProcessBlock(input_data, scratch_vector_);
  mcl::Add(scratch_vector_, output_buffer.GetReadView(Buffer::kRightChannel),
           output_buffer.GetWriteView(Buffer::kRightChannel));
}

void BinauralMicInstance::UpdateFilter(const Point& point) noexcept {
  if (!IsEqual(point, previous_point_)) {
    // Update cache variables
    previous_point_ = point;

    filter_left_.SetImpulseResponse(base_mic_->GetBrir(kLeftEar, point),
                                    update_length_);
    filter_right_.SetImpulseResponse(base_mic_->GetBrir(kRightEar, point),
                                     update_length_);
  }
}

DatabaseBinauralMic::DatabaseBinauralMic(
    const Point& position, const Quaternion orientation,
    const Int update_length, const HeadRefOrientation reference_orientation)
    : BinauralMic(position, orientation, update_length, reference_orientation) {
}

void DatabaseBinauralMic::FilterAll(mcl::Filter& filter) {
  mcl::FilterAll(filter, hrtf_database_right_, hrtf_database_right_);
  mcl::FilterAll(filter, hrtf_database_left_, hrtf_database_left_);
}

}  // namespace sal
