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
    This represents a receiver (e.g. virtual microphone).
   
   @param[in] directivity_prototype is the prototype of the directivity pattern.
   The receiver will then create a certain number of copies of the directivity
   pattern, each associated to a given incoming wave.
   @param[in] position is the position of the receiver.
   @param[in] orientation is the quaternion orientation of the receiver.
   The default is Quaternion identity.
   @param[in] max_num_incoming_waves is the number of incoming waves. The
   receiver will assert if it receives more incoming waves. The default is a
   single incoming wave.
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
    , bypass_(false)
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
  
  /** Allows to bypass the directivity pattern.
  @param[in] bypass if true, every call to `ReceiveAndAddToBuffer` will
  simply copy the input to all the channels of the output buffer */
  void SetBypass(
    bool bypass) noexcept
  {
    bypass_ = bypass;
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
      wave_id<directivity_instances_.size(),
      "Requested a wave id larger than the max num of incoming waves.");
    if (! bypass_)
    {
      directivity_instances_[wave_id].ReceiveAndAddToBuffer
      (
        input,
        GetRelativePoint(point),
        output_buffer);
    }
    else
    {
      for (size_t i=0; i<output_buffer.num_channels(); ++i)
      {
        std::copy(input.begin(), input.end(), output_buffer.begin(i));
      }
    }
  }

  void ReceiveAndAddToBuffer(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output_buffer) noexcept
  {
    if (! bypass_)
    {
      directivity_instances_[0].ReceiveAndAddToBuffer
      (
        input,
        GetRelativePoint(point),
        output_buffer);
    }
    else
    {
      for (size_t i=0; i<output_buffer.num_channels(); ++i)
      {
        std::copy(input.begin(), input.end(), output_buffer.begin(i));
      }
    }
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
  bool bypass_;
  
  mcl::Vector<Directivity<T>> directivity_instances_;
};




} // namespace sal
