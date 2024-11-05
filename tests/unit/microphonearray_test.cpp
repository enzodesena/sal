/*
 microphonearray_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "microphone.h"
#include "microphonearray.h"
#include "salconstants.h"
#include "salutilities.h"

namespace sal {

using sal::dsp::Point;

bool MicrophoneArrayTest() {
  const Int num_microphones(5);
  const Length array_radius(0.5);

  TrigMic mic_prototype(dsp::UnaryVector<Sample>(1.0), Point(0, 0, 0), dsp::Quaternion::Identity());

  CircularArray<TrigMic> microphone_array_a(
      Point(0.0, 0.0, 1.5), dsp::Quaternion::Identity(), mic_prototype,
      array_radius, UniformAngles<Angle>(num_microphones, 0));

  ASSERT(dsp::IsEqual(microphone_array_a.position(), Point(0.0, 0.0, 1.5)));

  ASSERT(microphone_array_a.GetMicrophoneRef(0).position().Equals(
      Point(array_radius, 0.0, 1.5)));
  ASSERT(microphone_array_a.GetMicrophoneRef(1).position().Equals(
      Point(array_radius * cos(2.0 * PI / 5.0),
            array_radius * sin(2.0 * PI / 5.0), 1.5)));

  ASSERT(dsp::IsEqual(microphone_array_a.GetMicrophoneRef(0).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, 0)));
  ASSERT(dsp::IsEqual(microphone_array_a.GetMicrophoneRef(1).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, 2.0 * PI / 5.0)));

  microphone_array_a.SetPosition(Point(1.0, 0.0, 1.5));
  ASSERT(microphone_array_a.GetMicrophoneRef(0).position().Equals(
      Point(1.0 + array_radius, 0.0, 1.5)));
  ASSERT(microphone_array_a.GetMicrophoneRef(1).position().Equals(
      Point(1.0 + array_radius * cos(2.0 * PI / 5.0),
            0.0 + array_radius * sin(2.0 * PI / 5.0), 1.5)));

  CircularArray<TrigMic> microphone_array_b(
      Point(0.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0), mic_prototype,
      array_radius, UniformAngles<Angle>(num_microphones, 0));

  ASSERT(dsp::IsEqual(microphone_array_b.GetMicrophoneRef(0).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, PI / 2.0)));
  ASSERT(dsp::IsEqual(microphone_array_b.GetMicrophoneRef(1).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT(microphone_array_b.GetMicrophoneRef(0).position().Equals(
      Point(0.0, array_radius, 0.0)));
  ASSERT(microphone_array_b.GetMicrophoneRef(1).position().Equals(
      Point(array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
            array_radius * sin(2.0 * PI / 5.0 + PI / 2.0), 0.0)));

  CircularArray<TrigMic> microphone_array_c(
      Point(0.0, 1.0, -1.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0), mic_prototype,
      array_radius, UniformAngles<Angle>(num_microphones, 0));

  ASSERT(dsp::IsEqual(microphone_array_c.GetMicrophoneRef(0).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, PI / 2.0)));
  ASSERT(dsp::IsEqual(microphone_array_c.GetMicrophoneRef(1).orientation(),
                      dsp::AxAng2Quat(0, 0, 1, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT(microphone_array_c.GetMicrophoneRef(0).position().Equals(
      Point(0.0, 1.0 + array_radius, -1.0)));
  ASSERT(microphone_array_c.GetMicrophoneRef(1).position().Equals(
      Point(array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
            1.0 + array_radius * sin(2.0 * PI / 5.0 + PI / 2.0), -1.0)));

  StereoMic<TrigMic> stereo_mic(Point(0.2, 0.0, 1.5),
                                dsp::Quaternion::Identity(), mic_prototype, 1.0,
                                PI / 4.0, +PI / 8.0);

  ASSERT(dsp::IsEqual(stereo_mic.GetMicrophoneRef(0).position(),
                      Point(0.2 + 1.0 * cos(0), 1.0 * sin(0), 1.5)));
  ASSERT(
      dsp::IsEqual(stereo_mic.GetMicrophoneRef(1).position(),
                   Point(0.2 + 1.0 * cos(PI / 4.0), 1.0 * sin(PI / 4.0), 1.5)));

  return true;
}

}  // namespace sal
