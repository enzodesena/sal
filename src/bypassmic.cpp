/*
 Spatial Audio Library (SAL)
 Copyright (c) 2021, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */


#include "bypassmic.h"

using mcl::Point;
using sal::Length;
using sal::Angle;
using sal::Sample;
using sal::Time;
using sal::Int;

namespace sal {
  
BypassMic::BypassMic(Point position, Int num_channels) noexcept :
        sal::Microphone(position, mcl::Quaternion::Identity()),
        num_channels_(num_channels) {}

  
void BypassMic::AddPlaneWaveRelative(const sal::Sample* input_data,
                                     const sal::Int num_samples,
                                     const mcl::Point& point,
                                     const sal::Int wave_id,
                                     sal::Buffer& output_buffer) noexcept {
  ASSERT(wave_id >= 0);
  if (wave_id < output_buffer.num_channels()) {
    output_buffer.AddSamples(wave_id, 0, num_samples, input_data);
  }
}


  
  
  
} // namespace sal
