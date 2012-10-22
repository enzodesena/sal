/*
 basicop_test.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 93 $
 Last changed date:  $Date: 2012-06-07 09:58:54 +0100 (Thu, 07 Jun 2012) $
 */


#include "basicop.h"
#include "equalityop.h"
#include "mcltypes.h"
#include <vector>

namespace mcl {
  
bool BasicOpTest() {
  std::vector<Real> vector_d(4);
  vector_d[0] = -0.3;
  vector_d[1] = 0.3;
  vector_d[2] = 2.4;
  vector_d[3] = -12.4;
  
  assert(IsEqual(Max(vector_d), 2.4));
  assert(MaxIndex(vector_d) == 2);
  
  assert(IsEqual(Min(vector_d), -12.4));
  assert(MinIndex(vector_d) == 3);
  
  std::vector<Real> vector_e(4);
  vector_e[0] = -0.3;
  vector_e[1] = 30.3;
  vector_e[2] = 2.4;
  vector_e[3] = 12.4;
  
  assert(IsEqual(Max(vector_e), 30.3));
  assert(MaxIndex(vector_e) == 1);
  
  assert(IsEqual(Min(vector_e), -0.3));
  assert(MinIndex(vector_e) == 0);

  std::vector<Real> vector_f(1);
  vector_f[0] = 1.0;
  assert(MinIndex(vector_f) == 0);
  assert(MaxIndex(vector_f) == 0);
  
  std::vector<Real> vector_g(1);
  vector_g[0] = -1.0;
  assert(MinIndex(vector_g) == 0);
  assert(MaxIndex(vector_g) == 0);
  
  return true;
}

} // namespace mcl