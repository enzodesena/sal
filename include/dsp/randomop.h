/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_RANDOMGENERATOR_H
#define MCL_RANDOMGENERATOR_H

// Using TR1 and old c++ library

#include <random>
#include <vector>

#include "mcltypes.h"

namespace sal {

namespace dsp {
/**
 RandomGenerator class
 */
class RandomGenerator {
 public:
  RandomGenerator();

  RandomGenerator(unsigned int seed);

  /**
   Returns a vector containing pseudorandom values drawn from the standard
   normal distribution. Equivalent to Matlab's randn(size,1);
   */
  std::vector<Real> Randn(const Int size);

  /**
   Returns a vector containing pseudorandom values drawn from the uniform
    distribution. Equivalent to Matlab's rand(size,1);
   */
  std::vector<Real> Rand(const Int size);

  /**
   Returns a single pseudorandom value drawn from the uniform
   distribution. Equivalent to Matlab's rand(1,1);
   */
  Real Rand() { return Rand(1)[0]; }

  /**
   Returns a single pseudorandom integer value, uniformly distributed
   between the given extrema (which are included).
   */
  Int RandInt(const Int& minimum, const Int& maximum);

  /** Set seed of the for random generator */
  void SetSeed(unsigned int seed) { generator_.seed(seed); }

  /**
   Testing
   */
  static bool Test();

 private:
  std::default_random_engine generator_;
  std::normal_distribution<double> distribution_norm_;
  std::uniform_real_distribution<double> distribution_uniform_;
};

} // namespace dsp

} // namespace sal

#endif
