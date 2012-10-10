/*
 randomgenerator.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
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

#include <tr1/random>
#include <sys/time.h>
#include "mcltypes.h"
#include <vector>


namespace mcl {

class RandomGenerator {
public:
  RandomGenerator();
  
  // Returns a vector containing pseudorandom values drawn from the standard 
  // normal distribution. Equivalent to Matlab's randn(size,1);
  std::vector<Real> Randn(const UInt size);
  
  static bool Test();
private:
  std::tr1::variate_generator<std::tr1::mt19937, 
  std::tr1::normal_distribution<double> > randn_;
  std::tr1::variate_generator<std::tr1::mt19937, 
  std::tr1::normal_distribution<double> > CreateGenerator();
  
};

} // namespace mcl



#endif
