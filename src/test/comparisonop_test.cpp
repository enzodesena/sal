/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
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
  
  
  
  /* Returns true if num is nan */
  assert(IsNan(NAN));
  assert(!IsNan(1.0));
  assert(!IsNan(-1.0));
  
  /* Returns true if num is nan */
  std::vector<Real> input;
  input.push_back(1.0);
  input.push_back(NAN);
  std::vector<bool> output_a = IsNan(input);
  assert(!output_a[0]);
  assert(output_a[1]);
  
  /* Returns opposite bool as input */
  std::vector<bool> output_b = Not(output_a);
  assert(output_b[0]);
  assert(!output_b[1]);
  
  /* Returns true if all bools are true */
  assert(!All(output_b));
  assert(Any(output_b));
  output_b[1] = true;
  assert(All(output_b));
  assert(Any(output_b));
  assert(!Any(Not(output_b)));
  
  std::vector<bool> output_c;
  output_c.push_back(false);
  output_c.push_back(false);
  output_c.push_back(false);
  assert(None(output_c));
  
  output_c[0] = true;
  assert(!None(output_c));
  output_c[1] = true;
  output_c[2] = true;
  assert(!None(output_c));
  
  // Testing infs
  assert(IsInf(INFINITY));
  assert(IsInf(-INFINITY));
  
  std::vector<Real> input_d;
  input_d.push_back(INFINITY);
  input_d.push_back(-INFINITY);
  input_d.push_back(0.1);
  
  std::vector<bool> output_d = IsInf(input_d);
  assert(output_d[0] == true);
  assert(output_d[1] == true);
  assert(output_d[2] == false);
  
  
  return true;
}

  
} // namespace mcl
