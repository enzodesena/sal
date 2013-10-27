/*
 comparisonop_test.cpp
 MCL
 Copyright (c) 2013, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "comparisonop.h"
#include <cassert>

namespace mcl {
  

bool ComparisonOpTest() {
  assert(IsEqual(0.2, 0.2));
  assert(IsEqual(-0.25, -0.25));
  
  assert(IsSmallerOrEqual(0.1, 0.5));
  assert(IsSmallerOrEqual(-0.5, -0.1));
  
  std::vector<Real> vector_a(3);
  vector_a[0] = 0.1;
  vector_a[1] = -0.1;
  vector_a[2] = -2.6;
  std::vector<Real> vector_b(3);
  vector_b[0] = 0.1;
  vector_b[1] = -0.5;
  vector_b[2] = -3.0;
  
  assert(AreAllSmallerOrEqual(vector_b, vector_a));
  assert(!AreAllSmallerOrEqual(vector_a, vector_b));
  
  std::vector<Real> vector_c(3);
  vector_c[0] = 0.1;
  vector_c[1] = -0.5;
  vector_c[2] = 2.1;
  
  assert(! AreAllSmallerOrEqual(vector_c, vector_a));
  
  return true;
}

  
} // namespace mcl