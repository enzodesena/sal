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
    const Directivity<T>& directivity_prototype,
    const Point& position,
    const Quaternion& orientation = Quaternion::Identity(),
    const size_t max_num_incoming_waves = 1);

  /** Returns current position of the microphone */
  Point position() const noexcept;

  /** Set microphone position */
  virtual void SetPosition(
    const Point& position) noexcept;

  /** Returns current orientation of the microphone */
  Quaternion orientation() const noexcept;

  /** Set microphone orientation */
  virtual void SetOrientation(
    const Quaternion& orientation) noexcept;

  /** Set handedness of reference system */
  void SetHandedness(
    mcl::Handedness handedness) noexcept;
  
  /** Allows to bypass the directivity pattern.
  @param[in] bypass if true, every call to `ReceiveAdd` will
  simply copy the input to all the channels of the output buffer */
  void SetBypass(
    bool bypass) noexcept;

  /**
   We need to
   keep track of the wave_id because some microphones (e.g. kemar) need
   to do some filtering on the signal longer than one sample.
   The function Tick tells the microphone to advance in time.
   The microphone objects need to keep track of each plane wave. Hence,
   the first time it sees a new wave_id, it will allocate a new filter
   for it.
   */
  void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    const size_t wave_id,
    Buffer<T>& output_buffer) noexcept;

  void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output_buffer) noexcept;

  /** Resets the state of the microphone (if any). */
  virtual void ResetState() noexcept;


private:

  /** This method translates `point` in the reference system of the mic. */
  Point GetRelativePoint(
    const Point& point) const noexcept;
  
  Point position_;
  Quaternion orientation_;
  mcl::Handedness handedness_;
  bool bypass_;
  
  mcl::Vector<Directivity<T>> directivity_instances_;
};




} // namespace sal

#include "receiver.ipp"
