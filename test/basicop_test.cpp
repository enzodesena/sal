/*
 basicop_test.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "basicop.h"
#include "equalityop.h"
#include "mcltypes.h"
#include "vectorop.h"
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
  
  std::vector<Real> vector_h = Zeros<Real>(5);
  vector_h[3] = -1.0;
  vector_h[4] = 1.0;
  assert(MinIndex(vector_h) == 3);
  assert(MaxIndex(vector_h) == 4);
  
  std::vector<Int> vector_i = Zeros<Int>(2);
  vector_i[0] = 38;
  vector_i[1] = 6;
  assert(MinIndex(vector_i) == 1);
  
  
  // Testing find peaks
  std::vector<Real> vector_l(5);
  vector_l[0] = -0.3;
  vector_l[1] = 30.3;
  vector_l[2] = 2.4;
  vector_l[3] = 12.4;
  vector_l[4] = 12;
  std::vector<UInt> vector_l_peaks = FindPeaksIndexes(vector_l);
  std::vector<UInt> vector_l_peaks_cmp(2);
  vector_l_peaks_cmp[0] = 1;
  vector_l_peaks_cmp[1] = 3;
  assert(IsEqual(vector_l_peaks, vector_l_peaks_cmp));
  
  std::vector<UInt> vector_l_peaks_2 = FindPeaksIndexes(vector_l, 20.0);
  std::vector<UInt> vector_l_peaks_2_cmp(1);
  vector_l_peaks_2_cmp[0] = 1;
  assert(IsEqual(vector_l_peaks_2, vector_l_peaks_2_cmp));
  
  // Checks that it behaves like matlab
  assert(FindPeaksIndexes(Ones(3)).size() == 0);
  assert(FindPeaksIndexes(Zeros<Real>(3)).size() == 0);
  
  
  std::vector<UInt> vector_m(4);
  vector_m[0] = 3;
  vector_m[1] = 2;
  vector_m[2] = 1;
  vector_m[3] = 4;
  
  assert(MinIndex(vector_m) == 2);
  assert(MaxIndex(vector_m) == 3);
  
  assert(MinIndex(Zeros<UInt>(3)) == 0);
  assert(MaxIndex(Zeros<UInt>(3)) == 0);
  
  assert(MinIndex(Zeros<Int>(3)) == 0);
  assert(MaxIndex(Zeros<Int>(3)) == 0);
  
  assert(MinIndex(Zeros<Real>(3)) == 0);
  assert(MaxIndex(Zeros<Real>(3)) == 0);
  
  
  std::vector<Real> vector_n(4);
  vector_n[0] = 2.3;
  vector_n[1] = 2.1;
  vector_n[2] = 1.1;
  vector_n[3] = 4.0;
  
  assert(MinIndex(vector_n) == 2);
  
  return true;
}

} // namespace mcl