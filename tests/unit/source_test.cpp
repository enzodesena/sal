/*
 source_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2024, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "source.h"

using sal::dsp::Point;
using sal::dsp::Quaternion;

namespace sal {

Sample ProcessSample(Source* source, Sample input, Point point) {
  std::vector<Sample> input_data(1);
  input_data[0] = input;
  std::vector<Sample> output_data(1);
  source->ProcessAudio(input_data, point, 0, output_data);
  return output_data[0];
}

bool Source::Test() {
  using sal::dsp::IsEqual;

  //////////////////////////////////
  // OmniMic tests                //
  //////////////////////////////////

  OmniSource source_a(Point(0.0, 0.0, 1.0));

  ASSERT(source_a.position().Equals(Point(0.0, 0.0, 1.0)));

  ASSERT(IsEqual(ProcessSample(&source_a, -1.0, Point(1.0, 0.0, 2.0)), -1.0));
  ASSERT(IsEqual(ProcessSample(&source_a, 2.0, Point(-1.0, 0.0, 2.0)), 2.0));

  //////////////////////////////////
  // TrigSource tests                //
  //////////////////////////////////
  std::vector<Sample> coefficients(3);
  coefficients[0] = 0.5;
  coefficients[1] = -1.0;
  coefficients[2] = -0.5;

  TrigSource source_i(coefficients, Point(0.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 4.0));
  
  ASSERT(IsEqual(
      ProcessSample(&source_i, 0.5, Point(0.0, 1.0, 0.0)),
      0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0))));

  TrigSource source_e(coefficients, Point(1.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0));
  ASSERT(IsEqual(ProcessSample(&source_e, 0.5, Point(0.0, 1.0, 0.0)),
      0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0))));

  TrigSource source_f(coefficients, Point(2.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 2.0));
  const Angle angle_f = PI / 2.0 - acos(2.0 / sqrt(5.0));
  ASSERT(
      IsEqual(ProcessSample(&source_f, 0.5, Point(0.0, 1.0, 0.0)),
              0.5 * (0.5 - 1.0 * cos(angle_f) - 0.5 * pow(cos(angle_f), 2.0))));

  TrigSource source_g(coefficients, Point(0.0, 0.0, 0.0), dsp::Quaternion::Identity());
  ASSERT(IsEqual(ProcessSample(&source_g, 0.5, Point(1.0, 0.0, 0.0)),
                 0.5 * (0.5 - 1.0 - 0.5)));

  TrigSource source_h(coefficients, Point(0.0, 0.0, 0.0), dsp::AxAng2Quat(0, 0, 1, PI / 5.0));
  ASSERT(
      IsEqual(ProcessSample(&source_h, 0.5, Point(2.0, 2.0 * tan(PI / 5.0), 0.0)),
              0.5 * (0.5 - 1.0 - 0.5)));

  TrigSource source_p(coefficients, Point(2.0, 1.2, 0.5), dsp::AxAng2Quat(0, 0, 1, 0));
  Sample sample =
  ProcessSample(&source_p, 0.5, Point(2.0 + cos(0), 1.2 + sin(0), 0.5 + 0.4));
  source_p.SetOrientation(dsp::AxAng2Quat(0, 0, 1, PI / 10.0));
  ASSERT(IsEqual(
                 ProcessSample(&source_p, 0.5, Point(2.0 + cos(PI / 10.0), 1.2 + sin(PI / 10.0), 0.5 + 0.4)), sample));

  return true;
}

}  // namespace sal
