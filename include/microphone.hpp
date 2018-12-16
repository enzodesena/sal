/*
 microphone.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "audiobuffer.hpp"
#include "point.hpp"
#include "quaternion.hpp"
#include "directivity.hpp"
#include <assert.h>
#include <map>

namespace sal
{


template<typename T>
class Receiver
{
public:
  /**
   `position` is the position of the microphone, and orientation 
   is a quaternion describing the microphone rotation.
   A microphone generally has its acoustical axis on the x-axis.
   
   An example on the use of this function is as follow:
   if you don't want to change its orientation, then you should use
   a Quaternion::Identical().
   Otherwise, if you want to rotate it around the z-axis (i.e. a
   rotation on the horizontal plane), then you should use 
   mcl::AxAng2Quat(0,0,1,angle) with angle in radians.
   */
  Receiver(
    Directivity<T> directivity_prototype,
    Point position,
    Quaternion orientation = Quaternion::Identity(),
    size_t max_num_incoming_waves = 1)
    : position_(position)
    , orientation_(orientation)
    , handedness_(mcl::kRightHanded)
    , directivity_instances_(max_num_incoming_waves, directivity_prototype)
  {
  }

  /** Returns current position of the microphone */
  Point position() const noexcept
  {
    return position_;
  }

  /** Set microphone position */
  virtual void SetPosition(
    const Point& position) noexcept
  {
    position_ = position;
  }

  /** Returns current orientation of the microphone */
  Quaternion orientation() const noexcept
  {
    return orientation_;
  }

  /** Set microphone orientation */
  virtual void SetOrientation(
    const Quaternion& orientation) noexcept
  {
    orientation_ = orientation;
  }

  /** Set handedness of reference system */
  void SetHandedness(
    mcl::Handedness handedness) noexcept
  {
    handedness_ = handedness;
  }

  /**
   We need to
   keep track of the wave_id because some microphones (e.g. kemar) need
   to do some filtering on the signal longer than one sample.
   The function Tick tells the microphone to advance in time.
   The microphone objects need to keep track of each plane wave. Hence,
   the first time it sees a new wave_id, it will allocate a new filter
   for it.
   */
  void ReceiveAndAddToBuffer(
    const mcl::Vector<T>& input,
    const Point& point,
    const size_t wave_id,
    Buffer<T>& output_buffer) noexcept
  {
    ASSERT_WITH_MESSAGE
    (
      wave_id>=directivity_instances_.size(),
      "Requested a wave id larger than the max num of incoming waves.");
    directivity_instances_[wave_id].ReceiveAndAddToBuffer
    (
      input,
      GetRelativePoint(point),
      output_buffer);
  }

  void ReceiveAndAddToBuffer(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output_buffer) noexcept
  {
    directivity_instances_[0].ReceiveAndAddToBuffer
    (
      input,
      GetRelativePoint(point),
      output_buffer);
  }

  /** Resets the state of the microphone (if any). */
  virtual void Reset() noexcept
  {
    for (
      auto iter=directivity_instances_.begin();
      iter != directivity_instances_.end();
      ++iter)
    {
      (*iter).Reset();
    }
  }


private:

  /** This method translates `point` in the reference system of the mic. */
  Point GetRelativePoint(
    const Point& point) const noexcept
  {
    if (mcl::IsEqual(point, position_))
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Microphone (%f, %f, %f) and observation point (%f, %f, %f) appear "
          "to be approximately in the same position. Behaviour undefined.",
          point.x(),
          point.y(),
          point.z(),
          position_.x(),
          position_.y(),
          position_.z());
    }

    // Instead of rotating the head, we are rotating the point in an opposite
    // direction (that's why the QuatInverse).
    Point rotated = mcl::QuatRotate
    (
      mcl::QuatInverse(orientation_),
      Point(point.x() - position_.x(),
      point.y() - position_.y(),
      point.z() - position_.z()),
      handedness_);
    return rotated;
  }
  
  Point position_;
  Quaternion orientation_;
  mcl::Handedness handedness_;
  
  mcl::Vector<Directivity<T>> directivity_instances_;
};




} // namespace sal
