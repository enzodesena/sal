/*
 propagationline_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "comparisonop.h"
#include "propagationline.h"
#include "salconstants.h"
#include "saltypes.h"

using sal::dsp::IsEqual;
using sal::dsp::Point;
using sal::Length;
using sal::Sample;
using sal::Time;

namespace sal {

bool PropagationLine::Test() {
  Time FS = 40000;
  Length distance = (Length)3.0 * SOUND_SPEED / FS;

  Point point_a(0, 0, 0);
  Point point_b(distance, 0, 0);

  ASSERT(IsEqual(Distance(point_a, point_b), distance));

  Sample attenuation = (Sample)(SOUND_SPEED / FS) / (distance);
  Time delay = distance / SOUND_SPEED;
  Time latency = delay * FS;

  ASSERT(IsEqual(latency, 3.0));

  PropagationLine prop_line_a = PropagationLine(distance, FS);

  prop_line_a.Write(1.0);
  ASSERT(IsEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  ASSERT(IsEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  ASSERT(IsEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(IsEqual(prop_line_a.Read(), 1.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(IsEqual(prop_line_a.Read(), 2.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(IsEqual(prop_line_a.Read(), 3.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(IsEqual(prop_line_a.Read(), -1.0 * attenuation));

  //
  prop_line_a.ResetState();
  prop_line_a.SetDistance((Length)2.0 * SOUND_SPEED / FS);
  attenuation = 1.0 / 2.0;

  prop_line_a.Tick();
  prop_line_a.Write(1.0);
  ASSERT(IsEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  ASSERT(IsEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  ASSERT(IsEqual(prop_line_a.Read(), 1.0 * attenuation));

  //
  PropagationLine prop_line_b =
      PropagationLine((Length)5.0 * SOUND_SPEED / FS, FS, 100.0,
                      sal::InterpolationType::kRounding);
  attenuation = 1.0 / 5.0;

  prop_line_b.Tick();
  prop_line_b.Write(1.0);
  for (Int i = 0; i < 5; ++i) {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  ASSERT(IsEqual(prop_line_b.Read(), 1.0 * attenuation));

  prop_line_b.SetDistance((Length)2.0 * SOUND_SPEED / FS);
  attenuation = 1.0 / 2.0;
  for (Int i = 0; i < 20; ++i) {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  prop_line_b.Tick();
  prop_line_b.Write(1.0);
  for (Int i = 0; i < 2; ++i) {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  ASSERT(IsEqual(prop_line_b.Read(), 1.0 * attenuation));

  // Testing batch processing
  const Int latency_samples = 3;
  const Int num_samples = 10;
  std::vector<Sample> input_samples =
      dsp::ColonOperator(1.0, 1.0, (Sample)num_samples + 1);
  std::vector<Sample> output_samples = dsp::Concatenate(
      dsp::Zeros<Sample>(latency_samples),
      dsp::Elements(input_samples, 0, num_samples - latency_samples));
  output_samples = dsp::Multiply<sal::Sample>(output_samples, 1.0 / 3.0);
  assert(input_samples.size() == output_samples.size());

  PropagationLine prop_line_c =
      PropagationLine(((Length)latency_samples) * SOUND_SPEED / FS, FS, 1.0,
                      sal::InterpolationType::kRounding);

  for (Int i = 0; i < num_samples; ++i) {
    prop_line_c.Write(input_samples[i]);
    ASSERT(IsEqual(prop_line_c.Read(), output_samples[i]));
    prop_line_c.Tick();
  }

  prop_line_c.ResetState();
  Int stride = 2;
  for (Int i = 0; i < num_samples; i += stride) {
    prop_line_c.Write(std::span(input_samples.begin() + i, stride));
    std::vector<Sample> cmp_samples(stride);
    prop_line_c.Read(cmp_samples);
    ASSERT(IsEqual(cmp_samples, std::span(output_samples.begin() + i, stride)));
    prop_line_c.Tick(stride);
  }

  prop_line_c.ResetState();
  stride = 3;
  for (Int i = 0; (i + stride) < num_samples; i += stride) {
    prop_line_c.Write(std::span(input_samples.begin() + i, stride));
    std::vector<Sample> cmp_samples(stride);
    prop_line_c.Read(cmp_samples);
    ASSERT(IsEqual(cmp_samples, std::span(output_samples.begin() + i, stride)));
    prop_line_c.Tick(stride);
  }

  return true;
}

}  // namespace sal
