/*
 microphone.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "microphone.hpp"
#include "quaternion.hpp"
#include "mcltypes.hpp"

using mcl::Point;
using mcl::Quaternion;

namespace sal
{
Microphone::Microphone(
  Point position,
  mcl::Quaternion orientation)
  : position_(position)
  , orientation_(orientation)
  , handedness_(mcl::kRightHanded)
{
}


Point Microphone::position() const noexcept
{
  return position_;
}


void Microphone::SetPosition(
  const Point& position) noexcept
{
  position_ = position;
}


/** Returns current orientation of the microphone */
Quaternion Microphone::orientation() const noexcept
{
  return orientation_;
}


/** Set microphone orientation */
void Microphone::SetOrientation(
  const mcl::Quaternion& orientation) noexcept
{
  orientation_ = orientation;
}


void Microphone::SetHandedness(
  const mcl::Handedness handedness) noexcept
{
  handedness_ = handedness;
}


void Microphone::AddPlaneWave(
  const Sample input_sample,
  const mcl::Point& point,
  Buffer& output_buffer) noexcept
{
  AddPlaneWave(&input_sample, 1, point, output_buffer);
}


void Microphone::AddPlaneWave(
  const Sample* input_data,
  const Int num_samples,
  const mcl::Point& point,
  Buffer& output_buffer) noexcept
{
  AddPlaneWave(input_data, num_samples, point, 0, output_buffer);
}


void Microphone::AddPlaneWave(
  const Sample input_sample,
  const mcl::Point& point,
  const Int wave_id,
  Buffer& output_buffer) noexcept
{
  AddPlaneWave(&input_sample, 1, point, wave_id, output_buffer);
}


void Microphone::AddPlaneWave(
  const Sample* input_data,
  const Int num_samples,
  const Point& point,
  const Int wave_id,
  Buffer& output_buffer) noexcept
{
  ASSERT(output_buffer.num_samples() >= num_samples);
  this->AddPlaneWaveRelative
  (
    input_data,
    num_samples,
    GetRelativePoint(point),
    wave_id,
    output_buffer);
}


void Microphone::AddPlaneWave(
  const MonoBuffer& input_buffer,
  const mcl::Point& point,
  Buffer& output_buffer) noexcept
{
  AddPlaneWave(input_buffer, point, 0, output_buffer);
}


void Microphone::AddPlaneWave(
  const MonoBuffer& input_buffer,
  const Point& point,
  const Int wave_id,
  Buffer& output_buffer) noexcept
{
  AddPlaneWave
  (
    input_buffer.GetReadPointer(),
    input_buffer.num_samples(),
    point,
    wave_id,
    output_buffer);
}


void Microphone::AddPlaneWaveRelative(
  const MonoBuffer& signal,
  const mcl::Point& point,
  const Int wave_id,
  Buffer& output_buffer) noexcept
{
  AddPlaneWaveRelative
  (
    signal.GetReadPointer(),
    signal.num_samples(),
    point,
    wave_id,
    output_buffer);
}


} // namespace sal
