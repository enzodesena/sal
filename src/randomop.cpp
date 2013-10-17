/*
 randomgenerator.cpp
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

//#include <random>
//#include "randomop.h"
//#include "mcltypes.h"
//#include <vector>
//
//
//namespace mcl {
//
//  
//std::vector<Real> RandomGenerator::Randn(const UInt size) {
//  std::vector<Real> output(size);
//  for (UInt i = 0; i < size; i++) {
//    output[i] = distribution_(generator_);
//  }
//  return output;
//}
//  
//} // namespace mcl

#include "randomop.h"
#include <sys/time.h>
#include "mcltypes.h"
#include <vector>


namespace mcl {
  
RandomGenerator::RandomGenerator() : distribution_(std::normal_distribution<double>(0.0,1.0)) {
  generator_.seed((unsigned int) time(NULL));
}

  
std::vector<Real> RandomGenerator::Randn(const UInt size) {
  std::vector<Real> output(size);
  for (UInt i = 0; i < size; i++) { output[i] = distribution_(generator_); }
  return output;
}

} // namespace mcl


