/*
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#pragma once

#include "receiver.hpp"
#include "point.hpp"
#include "source.hpp"
#include "receiver.hpp"
#include "saltypes.hpp"
#include "salconstants.hpp"
#include "directivity.hpp"

namespace sal
{

/** This is a microhone array, containing pointers to other microphones. A call
 to AddPlaneWave will call the AddPlaneWave of each microphone.
 You can then access the streams of each microphone by first extracting
 pointers to the microphone with the microphones() method and then accessing
 each one's stream. Alternatively, the object also has a multichannel stream
 object, which contains pointers to the individual streams. */
template<typename T>
class ReceiverArray
{
public:
  
  Point GetArrayCentre() const noexcept
  {
    return array_centre_;
  }
  
  /**
   This method will move all the internal microphones to a new position.
   The relative positions of the different microphones will stay unchanged.
   */
  void SetArrayCentre(
    const Point& position) noexcept
  {
    Point position_delta
    (
      position.x() - array_centre_.x(),
      position.y() - array_centre_.y(),
      position.z() - array_centre_.z());
    for (size_t i = 0; i < receivers_.size(); ++i)
    {
      Point old_position = receivers_[i].position();
      Point new_position
      (
        old_position.x() + position_delta.x(),
        old_position.y() + position_delta.y(),
        old_position.z() + position_delta.z());
      receivers_[i].SetPosition(new_position);
    }
  }


  void SetArrayOrientation(
    const Quaternion& orientation) noexcept
  {
    for (size_t i = 0; i < receivers_.size(); ++i)
    {
      Point relative_position = mcl::QuatRotate
      (
        orientation,
        mcl::Subtract(original_positions_[i], array_centre_));
      receivers_[i].SetPosition(mcl::Sum(relative_position, array_centre_));
      
      receivers_[i].SetOrientation(mcl::QuatMultiply(orientation, original_orientations_[i]));
    }
  }


  /**
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  bool IsCoincident() const noexcept
  {
    const size_t num_microphones = receivers_.size();

    if (num_microphones == 0 || num_microphones == 1)
    {
      return true;
    }

    Point position(receivers_[0]->position());
    for (size_t i = 1; i < num_microphones; ++i)
    {
      if (! IsEqual(receivers_[i]->position(), position))
      {
        return false;
      }
    }
    return true;
  }

  size_t size() const noexcept
  {
    return receivers_.size();
  }

  Receiver<T>& operator[](
    const size_t index) noexcept
  {
    ASSERT(index>=0 && index < size());
    return receivers_[index];
  }

  const Receiver<T>& operator[](
    const size_t index) const noexcept
  {
    ASSERT(index>=0 && index < size());
    return receivers_[index];
  }


  static ReceiverArray<T> MakeCircularArray(
    const Directivity<T>& directivity_prototype,
    const Point& position,
    const Quaternion& orientation,
    const Length radius,
    const mcl::Vector<Angle>& angles,
    const size_t max_num_incoming_waves = 1) noexcept
  {
    mcl::Vector<Receiver<T>> receivers
    (
      angles.size(),
      Receiver<T>
      (
        directivity_prototype,
        position,
        Quaternion::Identity(),
        max_num_incoming_waves));
    
    for (size_t i=0; i<receivers.size(); ++i)
    {
      receivers[i].SetPosition(GetCircularPosition(position, radius, angles[i]));
      receivers[i].SetOrientation(mcl::AxAng2Quat(0.0, 0.0, 1.0, angles[i]));
    }
    return std::move(ReceiverArray(receivers, position, orientation));
  }

  static ReceiverArray<T> MakeStereoArray(
    const Directivity<T>& directivity_prototype,
    const Point& position,
    const Quaternion& orientation,
    const Length radius,
    const Angle base_angle,
    const Angle center_angle,
    const size_t max_num_incoming_waves = 1)
  {
    mcl::Vector<Angle> angles(2);
    angles[0] = center_angle - base_angle / 2.0;
    angles[1] = center_angle + base_angle / 2.0;
    
    return MakeCircularArray(
      directivity_prototype,
      position,
      orientation,
      radius,
      angles,
      max_num_incoming_waves);
  }

private:

  ReceiverArray(
    const mcl::Vector<Receiver<T>>& receivers,
    const Point& array_centre,
    const Quaternion& array_orientation)
    : receivers_(receivers)
    , array_centre_(array_centre)
    , original_positions_(receivers.size())
    , original_orientations_(receivers.size())
  {
    for (size_t i=0; i<receivers.size(); ++i)
    {
      original_positions_[i] = receivers[i].position();
      original_orientations_[i] = receivers[i].orientation();
    }
    SetArrayOrientation(array_orientation);
  }
  
  static Point GetCircularPosition(
    const Point& position,
    const Length radius,
    const Angle angle)
  {
    return Point
    (
      position.x() + radius * cos(angle),
      position.y() + radius * sin(angle),
      position.z());
  }

  
  mcl::Vector<Receiver<T>> receivers_;
  Point array_centre_;
  mcl::Vector<Point> original_positions_;
  mcl::Vector<Quaternion> original_orientations_;
};

} // namespace sal
