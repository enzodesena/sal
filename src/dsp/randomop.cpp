/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <cassert>
#include <vector>

#include "mcltypes.h"
#include "randomop.h"

namespace sal {

namespace dsp {

RandomGenerator::RandomGenerator(unsigned int seed)
    : distribution_norm_(std::normal_distribution<double>(0.0, 1.0)),
      distribution_uniform_(std::uniform_real_distribution<double>(0.0, 1.0)) {
  generator_.seed(seed);
}

RandomGenerator::RandomGenerator()
    : distribution_norm_(std::normal_distribution<double>(0.0, 1.0)),
      distribution_uniform_(std::uniform_real_distribution<double>(0.0, 1.0)) {
  generator_.seed(1u);
}

std::vector<Real> RandomGenerator::Randn(const Int size) {
  std::vector<Real> output(size);
  for (Int i = 0; i < size; i++) {
    output[i] = distribution_norm_(generator_);
  }
  return output;
}

std::vector<Real> RandomGenerator::Rand(const Int size) {
  std::vector<Real> output(size);
  for (Int i = 0; i < size; i++) {
    output[i] = distribution_uniform_(generator_);
  }
  return output;
}

Int RandomGenerator::RandInt(const Int& minimum, const Int& maximum) {
  // number of possible outcomes
  // e.g. max = 2, min = 0 => num_outcomes = 3
  const Int num_outcomes = maximum - minimum + 1;
  Int output = (Int)floor((double)(distribution_uniform_(generator_) *
                                   ((double)num_outcomes))) +
               minimum;

  ASSERT(output >= minimum);
  ASSERT(output <= maximum);
  return output;
}

} // namespace dsp

} // namespace sal
