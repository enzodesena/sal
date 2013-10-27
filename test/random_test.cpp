/*
 random_test.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "randomop.h"
#include "equalityop.h"
#include "mcltypes.h"
#include <vector>
#include <cassert>
#include "vectorop.h"

namespace mcl {

bool RandomGenerator::Test() {
  
  RandomGenerator randn_gen;

  std::vector<Real> rand_vector_a = randn_gen.Randn(5);
  assert(rand_vector_a.size() == 5);
  // Check whether seeds are different:
  std::vector<Real> rand_vector_b = randn_gen.Randn(5);
  assert(!IsEqual(rand_vector_a, rand_vector_b));
  
  assert(Abs(Mean(randn_gen.Randn(100000))) < 0.1);
  assert(Abs(Std(randn_gen.Randn(100000))-1.0) < 0.1);
  
  // Testing uniform distribution
  assert(! IsEqual(randn_gen.Rand(1), randn_gen.Rand(1)));
  assert(! IsEqual(randn_gen.Rand(1), randn_gen.Rand(1)));
  
  // Test that output is between 0 and 1
  assert(Abs(randn_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(randn_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(randn_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(randn_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(randn_gen.Rand(1)[0]-0.5) < 0.5);
  
  assert(randn_gen.Rand(5).size() == 5);
  assert(Abs(Mean(randn_gen.Rand(100000))-0.5)<0.05);
  
  return true;
}

} // namespace mcl