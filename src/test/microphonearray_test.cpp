/*
 microphonearray_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "microphone.hpp"
#include "microphonearray.hpp"
#include "salconstants.hpp"
#include "salutilities.hpp"

namespace sal
{
using mcl::Point;


bool MicrophoneArrayTest()
{
  const Int num_microphones(5);
  const Length array_radius(0.5);

  TrigMic mic_prototype
  (
    Point(0, 0, 0),
    mcl::Quaternion::Identity(),
    mcl::Unarymcl::Vector<Sample>(1.0));

  CircularArray<TrigMic> microphone_array_a
  (
    Point(0.0, 0.0, 1.5),
    mcl::Quaternion::Identity(),
    mic_prototype,
    array_radius,
    UniformAngles(num_microphones, 0));

  ASSERT(mcl::IsEqual(microphone_array_a.position(), Point(0.0, 0.0, 1.5)));

  mcl::Vector<Microphone*> microphones_a = microphone_array_a.
    GetMicrophonePointers();

  ASSERT(microphones_a.size() == num_microphones);

  ASSERT(microphones_a[0]->position().Equals(Point(array_radius, 0.0, 1.5)));
  ASSERT
  (
    microphones_a[1]->position().Equals
    (
      Point
      (
        array_radius * cos(2.0 * PI / 5.0),
        array_radius * sin(2.0 * PI / 5.0),
        1.5)));

  ASSERT(
    mcl::IsEqual(microphones_a[0]->orientation(), mcl::AxAng2Quat(0, 0, 1, 0)));
  ASSERT(
    mcl::IsEqual(
      microphones_a[1]->orientation(),
      mcl::AxAng2Quat(0, 0, 1, 2.0 * PI / 5.0)));

  microphone_array_a.SetPosition(Point(1.0, 0.0, 1.5));
  ASSERT(
    microphones_a[0]->position().Equals(Point(1.0 + array_radius, 0.0, 1.5)));
  ASSERT
  (
    microphones_a[1]->position().Equals
    (
      Point
      (
        1.0 + array_radius * cos(2.0 * PI / 5.0),
        0.0 + array_radius * sin(2.0 * PI / 5.0),
        1.5)));

  CircularArray<TrigMic> microphone_array_b
  (
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat(0, 0, 1, PI / 2.0),
    mic_prototype,
    array_radius,
    UniformAngles(num_microphones, 0));

  mcl::Vector<Microphone*> microphones_b = microphone_array_b.
    GetMicrophonePointers();

  ASSERT
  (
    mcl::IsEqual
    (
      microphones_b[0]->orientation(),
      mcl::AxAng2Quat(0, 0, 1, PI / 2.0)));
  ASSERT
  (
    mcl::IsEqual
    (
      microphones_b[1]->orientation(),
      mcl::AxAng2Quat(0, 0, 1, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT(microphones_b[0]->position().Equals(Point(0.0, array_radius, 0.0)));
  ASSERT
  (
    microphones_b[1]->position().Equals
    (
      Point
      (
        array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
        array_radius * sin(2.0 * PI / 5.0 + PI / 2.0),
        0.0)));

  CircularArray<TrigMic> microphone_array_c
  (
    Point(0.0, 1.0, -1.0),
    mcl::AxAng2Quat(0, 0, 1, PI / 2.0),
    mic_prototype,
    array_radius,
    UniformAngles(num_microphones, 0));

  mcl::Vector<Microphone*> microphones_c = microphone_array_c.
    GetMicrophonePointers();

  ASSERT
  (
    mcl::IsEqual
    (
      microphones_c[0]->orientation(),
      mcl::AxAng2Quat(0, 0, 1, PI / 2.0)));
  ASSERT
  (
    mcl::IsEqual
    (
      microphones_c[1]->orientation(),
      mcl::AxAng2Quat(0, 0, 1, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT(
    microphones_c[0]->position().Equals(Point(0.0, 1.0 + array_radius, -1.0)));
  ASSERT
  (
    microphones_c[1]->position().Equals
    (
      Point
      (
        array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
        1.0 + array_radius * sin(2.0 * PI / 5.0 + PI / 2.0),
        -1.0)));

  StereoMic<TrigMic> stereo_mic
  (
    Point(0.2, 0.0, 1.5),
    mcl::Quaternion::Identity(),
    mic_prototype,
    1.0,
    PI / 4.0,
    +PI / 8.0);

  mcl::Vector<Microphone*> stereo_mics = stereo_mic.GetMicrophonePointers();

  ASSERT(stereo_mics.size() == 2);
  ASSERT(
    mcl::IsEqual(
      stereo_mics[0]->position(),
      Point(0.2 + 1.0 * cos(0), 1.0 * sin(0), 1.5)));
  ASSERT(
    mcl::IsEqual(
      stereo_mics[1]->position(), Point(
        0.2 + 1.0 * cos(PI / 4.0), 1.0 * sin(PI / 4.0), 1.5)));

  return true;
}
} // namespace sal
