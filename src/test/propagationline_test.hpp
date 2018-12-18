/*
 propagationline_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "propagationline.hpp"
#include "saltypes.hpp"
#include "salconstants.hpp"
#include "comparisonop.hpp"


namespace sal
{
inline bool PropagationLineTest()
{
  Time FS = 40000;
  Length distance = (Length)3.0 * SOUND_SPEED / FS;

  Point point_a(0, 0, 0);
  Point point_b(distance, 0, 0);

  ASSERT(mcl::IsApproximatelyEqual(Distance(point_a, point_b), distance));

  Sample attenuation = (Sample)(SOUND_SPEED / FS) / (distance);
  Time delay = distance / SOUND_SPEED;
  Time latency = delay * FS;

  ASSERT(mcl::IsApproximatelyEqual(latency, 3.0));

  PropagationLine<Sample> prop_line_a(distance, FS);

  prop_line_a.Write(1.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 0.0));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 1.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 2.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 3.0 * attenuation));

  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), -1.0 * attenuation));

  //
  prop_line_a.Reset();
  prop_line_a.SetDistance((Length)2.0 * SOUND_SPEED / FS);
  attenuation = 1.0 / 2.0;

  prop_line_a.Tick();
  prop_line_a.Write(1.0);
  ASSERT(prop_line_a.Read() == 0.0);

  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  ASSERT(prop_line_a.Read() == 0.0);

  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  ASSERT(mcl::IsApproximatelyEqual(prop_line_a.Read(), 1.0 * attenuation));

  //
  PropagationLine<Sample> prop_line_b
  (
    (Length)5.0 * SOUND_SPEED / FS,
    FS,
    100.0,
    sal::InterpolationType::kRounding);
  attenuation = 1.0 / 5.0;

  prop_line_b.Tick();
  prop_line_b.Write(1.0);
  for (Int i = 0; i < 5; ++i)
  {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  ASSERT(mcl::IsApproximatelyEqual(prop_line_b.Read(), 1.0 * attenuation));

  prop_line_b.SetDistance((Length)2.0 * SOUND_SPEED / FS);
  attenuation = 1.0 / 2.0;
  for (Int i = 0; i < 20; ++i)
  {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  prop_line_b.Tick();
  prop_line_b.Write(1.0);
  for (Int i = 0; i < 2; ++i)
  {
    prop_line_b.Tick();
    prop_line_b.Write(0.0);
  }
  ASSERT(mcl::IsApproximatelyEqual(prop_line_b.Read(), 1.0 * attenuation));

  // Testing batch processing
  const Int latency_samples = 3;
  const Int num_samples = 10;
  mcl::Vector<Sample> input_samples = mcl::ColonOperator(
    1.0, 1.0, (Sample)num_samples + 1);
  mcl::Vector<Sample> output_samples = mcl::Concatenate
  (
    mcl::Zeros<Sample>(latency_samples),
    mcl::Elements
    (
      input_samples,
      0,
      num_samples - latency_samples));
  output_samples = mcl::Multiply<sal::Sample>(output_samples, 1.0 / 3.0);
  assert(input_samples.size() == output_samples.size());

  PropagationLine<Sample> prop_line_c
  (
    ((Length)latency_samples) * SOUND_SPEED / FS,
    FS,
    1.0,
    sal::InterpolationType::kRounding);

  for (Int i = 0; i < num_samples; ++i)
  {
    prop_line_c.Write(input_samples[i]);
    ASSERT(mcl::IsApproximatelyEqual(prop_line_c.Read(), output_samples[i]));
    prop_line_c.Tick();
  }

  prop_line_c.Reset();
  Int stride = 2;
  for (Int i = 0; i < num_samples; i += stride)
  {
    prop_line_c.Write(MakeReference(input_samples, i, stride));
    mcl::Vector<Sample> cmp_samples(stride);
    prop_line_c.Read(cmp_samples);
    ASSERT(mcl::IsApproximatelyEqual(cmp_samples, MakeReference(output_samples, i, stride)));
    prop_line_c.Tick(stride);
  }

  prop_line_c.Reset();
  stride = 3;
  for (Int i = 0; (i + stride) < num_samples; i += stride)
  {
    prop_line_c.Write(MakeReference(input_samples, i, stride));
    mcl::Vector<Sample> cmp_samples(stride);
    prop_line_c.Read(cmp_samples);
    ASSERT(mcl::IsApproximatelyEqual(cmp_samples, MakeReference(output_samples, i, stride)));
    prop_line_c.Tick(stride);
  }

  return true;
}
} // namespace sal
