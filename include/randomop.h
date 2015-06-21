/*
 randomgenerator.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_RANDOMGENERATOR_H
#define MCL_RANDOMGENERATOR_H

//#include <random>
//#include <sys/time.h>
//#include "mcltypes.h"
//#include <vector>
//
//
//namespace mcl {
//
//class RandomGenerator {
//public:
//  RandomGenerator() :
//    distribution_(std::normal_distribution<>(0,1)), 
//    generator_(std::mt19937(random_device_())) {}
//  
//  // Returns a vector containing pseudorandom values drawn from the standard
//  // normal distribution. Equivalent to Matlab's randn(size,1);
//  std::vector<Real> Randn(const UInt size);
//  
//  static bool Test();
//private:
//  std::random_device random_device_;
//  std::mt19937 generator_; //gen(rd());
//  std::normal_distribution<> distribution_; //d(5,2);
//  
//};
//  
//} // namespace mcl

// Using TR1 and old c++ library

#include "mcltypes.h"
#include <vector>
#include <random>


namespace mcl {
/**
 RandomGenerator class
 */
class RandomGenerator {
public:
  RandomGenerator();
  
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
