/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "basicop.h"
#include "comparisonop.h"
#include "mcltypes.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

bool BasicOpTest() {
  std::vector<Real> vector_d(4);
  vector_d[0] = -0.3;
  vector_d[1] = 0.3;
  vector_d[2] = 2.4;
  vector_d[3] = -12.4;

  ASSERT(IsEqual(Max(vector_d), 2.4));
  ASSERT(MaxIndex(vector_d) == 2);

  ASSERT(IsEqual(Min(vector_d), -12.4));
  ASSERT(MinIndex(vector_d) == 3);

  std::vector<Real> vector_e(4);
  vector_e[0] = -0.3;
  vector_e[1] = 30.3;
  vector_e[2] = 2.4;
  vector_e[3] = 12.4;

  ASSERT(IsEqual(Max(vector_e), 30.3));
  ASSERT(MaxIndex(vector_e) == 1);

  ASSERT(IsEqual(Min(vector_e), -0.3));
  ASSERT(MinIndex(vector_e) == 0);

  std::vector<Real> vector_f(1);
  vector_f[0] = 1.0;
  ASSERT(MinIndex(vector_f) == 0);
  ASSERT(MaxIndex(vector_f) == 0);

  std::vector<Real> vector_g(1);
  vector_g[0] = -1.0;
  ASSERT(MinIndex(vector_g) == 0);
  ASSERT(MaxIndex(vector_g) == 0);

  std::vector<Real> vector_h = Zeros<Real>(5);
  vector_h[3] = -1.0;
  vector_h[4] = 1.0;
  ASSERT(MinIndex(vector_h) == 3);
  ASSERT(MaxIndex(vector_h) == 4);

  std::vector<Int> vector_i = Zeros<Int>(2);
  vector_i[0] = 38;
  vector_i[1] = 6;
  ASSERT(MinIndex(vector_i) == 1);

  std::vector<UInt> vector_z(4);
  vector_z[0] = 64286;
  vector_z[1] = 64286;
  vector_z[2] = 0;
  vector_z[3] = 3;
  ASSERT(Max(vector_z) == 64286);

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
  ASSERT(IsEqual(vector_l_peaks, vector_l_peaks_cmp));

  std::vector<UInt> vector_l_peaks_2 = FindPeaksIndexes(vector_l, 20.0);
  std::vector<UInt> vector_l_peaks_2_cmp(1);
  vector_l_peaks_2_cmp[0] = 1;
  ASSERT(IsEqual(vector_l_peaks_2, vector_l_peaks_2_cmp));

  // Checks that it behaves like matlab
  ASSERT(FindPeaksIndexes(Ones(3)).size() == 0);
  ASSERT(FindPeaksIndexes(Zeros<Real>(3)).size() == 0);

  std::vector<UInt> vector_m(4);
  vector_m[0] = 3;
  vector_m[1] = 2;
  vector_m[2] = 1;
  vector_m[3] = 4;

  ASSERT(MinIndex(vector_m) == 2);
  ASSERT(MaxIndex(vector_m) == 3);

  ASSERT(MinIndex(Zeros<UInt>(3)) == 0);
  ASSERT(MaxIndex(Zeros<UInt>(3)) == 0);

  ASSERT(MinIndex(Zeros<Int>(3)) == 0);
  ASSERT(MaxIndex(Zeros<Int>(3)) == 0);

  ASSERT(MinIndex(Zeros<Real>(3)) == 0);
  ASSERT(MaxIndex(Zeros<Real>(3)) == 0);

  std::vector<Real> vector_n(4);
  vector_n[0] = 2.3;
  vector_n[1] = 2.1;
  vector_n[2] = 1.1;
  vector_n[3] = 4.0;

  ASSERT(MinIndex(vector_n) == 2);

  return true;
}

} // namespace dsp

} // namespace sal
