/*
 randomgenerator.cpp
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
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
#include "mcltypes.h"
#include <vector>


namespace mcl {
  
RandomGenerator::RandomGenerator() : randn_(CreateGenerator()) {}

std::tr1::variate_generator<std::tr1::mt19937, 
std::tr1::normal_distribution<double> > RandomGenerator::CreateGenerator() {
  std::tr1::mt19937 prng(time(NULL));
  std::tr1::normal_distribution<double> normal;
  std::tr1::variate_generator<std::tr1::mt19937, 
        std::tr1::normal_distribution<double> > randn(prng,normal);
  return randn;
}

std::vector<Real> RandomGenerator::Randn(const UInt size) {
  std::vector<Real> output(size);
  for (UInt i = 0; i < size; i++) { output[i] = randn_(); }
  return output;
}

} // namespace mcl


