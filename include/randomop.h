/*
 randomgenerator.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_RANDOMGENERATOR_H
#define MCL_RANDOMGENERATOR_H


// Using TR1 and old c++ library

#include "mcltypes.h"
#include <vector>
#include <random>

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {
/**
 RandomGenerator class
 */
class MCL_API RandomGenerator {
public:
  RandomGenerator();
  
  RandomGenerator(unsigned int seed);
  
  /** 
   Returns a vector containing pseudorandom values drawn from the standard
   normal distribution. Equivalent to Matlab's randn(size,1);
   */
  std::vector<Real> Randn(const UInt size);
  
  /**
   Returns a vector containing pseudorandom values drawn from the uniform
    distribution. Equivalent to Matlab's rand(size,1);
   */
  std::vector<Real> Rand(const UInt size);
  
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
  void set_seed(unsigned int seed) { generator_.seed(seed); }
  
  /** 
   Testing
   */
  static bool Test();
private:
  std::default_random_engine generator_;
  std::normal_distribution<double> distribution_norm_;
  std::uniform_real_distribution<double> distribution_uniform_;
  
  
};

} // namespace mcl



#endif
