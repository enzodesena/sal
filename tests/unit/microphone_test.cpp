/*
 microphone_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "kemarmic.h"
#include "microphone.h"
#include "monomics.h"
#include "signal.h"
#include "sphericalheadmic.h"

using sal::dsp::Point;
using sal::dsp::Quaternion;

namespace sal {

bool Microphone::Test() {
  using sal::dsp::IsEqual;

  //////////////////////////////////
  // OmniMic tests                //
  //////////////////////////////////

  OmniMic mic_a(Point(0.0, 0.0, 1.0));

  ASSERT(mic_a.position().Equals(Point(0.0, 0.0, 1.0)));

  ASSERT(IsEqual(mic_a.RecordPlaneWave(-1.0, Point(1.0, 0.0, 2.0)), -1.0));
  ASSERT(IsEqual(mic_a.RecordPlaneWave(2.0, Point(-1.0, 0.0, -2.0)), 2.0));

  //////////////////////////////////
  // TrigMic tests                //
  //////////////////////////////////
  std::vector<Sample> coefficients(3);
  coefficients[0] = 0.5;
  coefficients[1] = -1.0;
  coefficients[2] = -0.5;

  TrigMic mic_i(coefficients, Point(0.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 4.0));

  ASSERT(IsEqual(
      mic_i.RecordPlaneWave(0.5, Point(0.0, 1.0, 0.0)),
      0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0))));

  TrigMic mic_e(coefficients, Point(1.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0));
  ASSERT(IsEqual(
      mic_e.RecordPlaneWave(0.5, Point(0.0, 1.0, 0.0)),
      0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0))));

  TrigMic mic_f(coefficients, Point(2.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0));
  const Angle angle_f = PI / 2.0 - acos(2.0 / sqrt(5.0));
  ASSERT(
      IsEqual(mic_f.RecordPlaneWave(0.5, Point(0.0, 1.0, 0.0)),
              0.5 * (0.5 - 1.0 * cos(angle_f) - 0.5 * pow(cos(angle_f), 2.0))));

  TrigMic mic_g(coefficients, Point(0.0, 0.0, 0.0), dsp::Quaternion::Identity());
  ASSERT(IsEqual(mic_g.RecordPlaneWave(0.5, Point(1.0, 0.0, 0.0)),
                 0.5 * (0.5 - 1.0 - 0.5)));

  TrigMic mic_h(coefficients, Point(0.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 5.0));
  ASSERT(
      IsEqual(mic_h.RecordPlaneWave(0.5, Point(2.0, 2.0 * tan(PI / 5.0), 0.0)),
              0.5 * (0.5 - 1.0 - 0.5)));

  TrigMic mic_p(coefficients, Point(2.0, 1.2, 0.5), dsp::AxAng2Quat(0, 0, 1, 0));
  Sample sample =
      mic_p.RecordPlaneWave(0.5, Point(2.0 + cos(0), 1.2 + sin(0), 0.5 + 0.4));
  mic_p.SetOrientation(dsp::AxAng2Quat(0, 0, 1, PI / 10.0));
  ASSERT(IsEqual(
      mic_p.RecordPlaneWave(
          0.5, Point(2.0 + cos(PI / 10.0), 1.2 + sin(PI / 10.0), 0.5 + 0.4)),
      sample));

  //////////////////////////////////
  // GainMic tests                //
  //////////////////////////////////

  GainMic mic_v(0.5, Point(0.0, 0.0, 0.0));
  ASSERT(IsEqual(mic_v.RecordPlaneWave(-1.0, Point(1.0, 0.0, 2.0)), -0.5));
  ASSERT(IsEqual(mic_v.RecordPlaneWave(-2.0, Point(1.0, 3.0, 2.0)), -1.0));

  //////////////////////////////////
  // BypassMic tests                //
  //////////////////////////////////
  BypassMic mic_z(Point(0, 0, 0), 3);
  Buffer output_buffer(3, 1);
  mic_z.AddPlaneWave(0.2, Point(1.0, 1.0, 1.0), 0, output_buffer);
  mic_z.AddPlaneWave(0.5, Point(1.0, 1.0, 1.0), 0, output_buffer);
  mic_z.AddPlaneWave(0.3, Point(1.0, 1.0, 1.0), 1, output_buffer);
  ASSERT(IsEqual(output_buffer.GetSample(0, 0), 0.7));
  ASSERT(IsEqual(output_buffer.GetSample(1, 0), 0.3));
  ASSERT(IsEqual(output_buffer.GetSample(2, 0), 0.0));

  return true;
}

}  // namespace sal
