/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <cassert>
#include <vector>

#include "comparisonop.h"
#include "mcltypes.h"
#include "randomop.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

bool RandomGenerator::Test() {
  RandomGenerator rand_gen;

  std::vector<Real> rand_vector_a = rand_gen.Randn(5);
  ASSERT(rand_vector_a.size() == 5);
  // Check whether seeds are different:
  std::vector<Real> rand_vector_b = rand_gen.Randn(5);
  ASSERT(!IsEqual(rand_vector_a, rand_vector_b));

  ASSERT(Abs(Mean(rand_gen.Randn(100000))) < 0.1);
  ASSERT(Abs(Std(rand_gen.Randn(100000)) - 1.0) < 0.1);

  // Testing uniform distribution
  ASSERT(!IsEqual(rand_gen.Rand(1), rand_gen.Rand(1)));
  ASSERT(!IsEqual(rand_gen.Rand(1), rand_gen.Rand(1)));

  // Test that output is between 0 and 1
  ASSERT(Abs(rand_gen.Rand(1)[0] - 0.5) < 0.5);
  ASSERT(Abs(rand_gen.Rand(1)[0] - 0.5) < 0.5);
  ASSERT(Abs(rand_gen.Rand(1)[0] - 0.5) < 0.5);
  ASSERT(Abs(rand_gen.Rand(1)[0] - 0.5) < 0.5);
  ASSERT(Abs(rand_gen.Rand(1)[0] - 0.5) < 0.5);

  ASSERT(rand_gen.Rand(5).size() == 5);
  ASSERT(Abs(Mean(rand_gen.Rand(100000)) - 0.5) < 0.05);

  // Test integer generator
  std::vector<Int> rand_int_vector;
  const Int num_samples = 1000000;
  const Int min_value = -2;
  const Int max_value = 3;
  std::vector<Int> num_occurrances(max_value - min_value + 1, 0);
  for (Int i = 0; i < num_samples; ++i) {
    Int output = rand_gen.RandInt(min_value, max_value);
    rand_int_vector.push_back(output);
    num_occurrances.at(output - min_value)++;
  }
  ASSERT(dsp::Min(rand_int_vector) >= min_value);
  ASSERT(dsp::Max(rand_int_vector) <= max_value);
  ASSERT(dsp::IsEqual(((Real)dsp::Min(num_occurrances)) / ((Real)num_samples),
                      ((Real)dsp::Max(num_occurrances)) / ((Real)num_samples),
                      1.0E-2));

  // Test integer generator
  std::vector<Int> rand_int_vector_b;
  const Int num_samples_b = 1000000;
  const Int min_value_b = 0;
  const Int max_value_b = 1;
  std::vector<Int> num_occurrances_b(max_value_b - min_value_b + 1, 0);
  for (Int i = 0; i < num_samples_b; ++i) {
    Int output = rand_gen.RandInt(min_value_b, max_value_b);
    rand_int_vector_b.push_back(output);
    num_occurrances_b.at(output - min_value_b)++;
  }
  ASSERT(dsp::Min(rand_int_vector_b) >= min_value_b);
  ASSERT(dsp::Max(rand_int_vector_b) <= max_value_b);
  ASSERT(dsp::IsEqual(
      ((Real)dsp::Min(num_occurrances_b)) / ((Real)num_samples_b),
      ((Real)dsp::Max(num_occurrances_b)) / ((Real)num_samples_b), 1.0E-2));

  return true;
}

} // namespace dsp

} // namespace sal
