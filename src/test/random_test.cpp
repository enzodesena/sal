/*
 random_test.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 93 $
 Last changed date:  $Date: 2012-06-07 09:58:54 +0100 (Thu, 07 Jun 2012) $
 */


#include "random.h"
#include "equalityop.h"
#include "mcltypes.h"
#include <vector>
#include <cassert>

namespace mcl {

bool RandomGenerator::Test() {
  
  RandomGenerator randn_gen = RandomGenerator();
  
  std::vector<Real> rand_vector_a = randn_gen.Randn(5);
  assert(rand_vector_a.size() == 5);
  // Check whether seeds are different:
  std::vector<Real> rand_vector_b = randn_gen.Randn(5);
  assert(!IsEqual(rand_vector_a, rand_vector_b));

  return true;
}

} // namespace mcl