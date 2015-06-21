/*
 random_test.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "randomop.h"
#include "comparisonop.h"
#include "mcltypes.h"
#include <vector>
#include <cassert>
#include "vectorop.h"

namespace mcl {

bool RandomGenerator::Test() {
  
  RandomGenerator rand_gen;

  std::vector<Real> rand_vector_a = rand_gen.Randn(5);
  assert(rand_vector_a.size() == 5);
  // Check whether seeds are different:
  std::vector<Real> rand_vector_b = rand_gen.Randn(5);
  assert(!IsEqual(rand_vector_a, rand_vector_b));
  
  assert(Abs(Mean(rand_gen.Randn(100000))) < 0.1);
  assert(Abs(Std(rand_gen.Randn(100000))-1.0) < 0.1);
  
  // Testing uniform distribution
  assert(! IsEqual(rand_gen.Rand(1), rand_gen.Rand(1)));
  assert(! IsEqual(rand_gen.Rand(1), rand_gen.Rand(1)));
  
  // Test that output is between 0 and 1
  assert(Abs(rand_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(rand_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(rand_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(rand_gen.Rand(1)[0]-0.5) < 0.5);
  assert(Abs(rand_gen.Rand(1)[0]-0.5) < 0.5);
  
  assert(rand_gen.Rand(5).size() == 5);
  assert(Abs(Mean(rand_gen.Rand(100000))-0.5)<0.05);
  
  // Test integer generator
  std::vector<Int> rand_int_vector;
  const UInt num_samples = 1000000;
  const Int min_value = -2;
  const Int max_value = 3;
  std::vector<Int> num_occurrances(max_value-min_value+1, 0);
  for (UInt i=0; i<num_samples; ++i) {
    Int output = rand_gen.RandInt(min_value, max_value);
    rand_int_vector.push_back(output);
    num_occurrances.at(output-min_value)++;
  }
  assert(mcl::Min(rand_int_vector) >= min_value);
  assert(mcl::Max(rand_int_vector) <= max_value);
  assert(mcl::IsEqual(((Real) mcl::Min(num_occurrances)) / ((Real) num_samples),
                      ((Real) mcl::Max(num_occurrances)) / ((Real) num_samples),
                      1.0E-2));
  
                      
  
  return true;
}

} // namespace mcl