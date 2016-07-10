/*
 transformop_test.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "transformop.h"
#include "comparisonop.h"
#include "vectorop.h"
#include <vector>
#include <cassert>

namespace mcl {
  
bool TransformOpTest() {
  std::vector<Complex> vector_a(3);
  vector_a[0] = Complex(1.0, 0.0);
  vector_a[1] = Complex(0.0, 1.0);
  vector_a[2] = Complex(1.0, 0.5);
  
  std::vector<Complex> vector_b(3);
  vector_b[0] = Complex(0.5, 1.0);
  vector_b[1] = Complex(0.2, -1.0);
  vector_b[2] = Complex(-1.0, -0.5);
  
  std::vector<Complex> fft_vector_a = Fft(vector_a, 3);
  std::vector<Complex> fft_vector_cmp_a(3);
  fft_vector_cmp_a[0] = Complex(2.0, 1.5);
  fft_vector_cmp_a[1] = Complex(0.933012701892219, 0.116025403784439);
  fft_vector_cmp_a[2] = Complex(0.066987298107781, -1.616025403784439);
  
  assert(IsEqual(fft_vector_a, fft_vector_cmp_a));
  
  std::vector<Complex> fft_vector_b = Fft(vector_b, 3);
  std::vector<Complex> fft_vector_cmp_b(3);
  fft_vector_cmp_b[0] = Complex(-0.300000000000000, -0.500000000000000);
  fft_vector_cmp_b[1] = Complex(0.466987298107781, 0.710769515458674);
  fft_vector_cmp_b[2] = Complex(1.333012701892219, 2.789230484541326);
  
  assert(IsEqual(fft_vector_b, fft_vector_cmp_b));
  assert(IsEqual(vector_a, Ifft(Fft(vector_a,3),3)));
  assert(IsEqual(vector_b, Ifft(Fft(vector_b,3),3)));
  
  // Check that when n_points is larger than the size of the vector,
  // then the result is the fft of a zero padded version of the input vector.
  std::vector<Complex> fft_vector_a_4 = Fft(vector_a, 4);
  assert(fft_vector_a_4.size() == 4);
  std::vector<Complex> fft_vector_a_4_cmp(4);
  fft_vector_a_4_cmp[0] = Complex(2.0000, 1.5000);
  fft_vector_a_4_cmp[1] = Complex(1.0000, -0.5000);
  fft_vector_a_4_cmp[2] = Complex(2.0000, -0.5000);
  fft_vector_a_4_cmp[3] = Complex(-1.0000,-0.5000);
  assert(IsEqual(fft_vector_a_4, fft_vector_a_4_cmp));
  
  // Check that if n_points is smaller than the size of the vector,
  // then the result is the fft of the cut vector
  assert(IsEqual(Fft(vector_a, 2), Fft(Elements(vector_a, 0, 1), 2)));
  
  // Test real fft
  std::vector<Real> vector_h = {-1.0, 2.0, 3.0};
  assert(IsEqual(Rfft(vector_h, 3),
                 Elements(Fft(ConvertToComplex(vector_h), 3), 0, 1)));
  assert(IsEqual(Rfft(vector_h, 4),
                 Elements(Fft(ConvertToComplex(vector_h), 4), 0, 2)));
  
  std::vector<Real> vector_i = {-1.0, 2.0, 3.0, 4.0};
  assert(IsEqual(Rfft(vector_h, 4),
                 Elements(Fft(ConvertToComplex(vector_h), 4), 0, 2)));
  assert(IsEqual(Rfft(vector_h, 5),
                 Elements(Fft(ConvertToComplex(vector_h), 5), 0, 2)));
  
  // Test Irfft
  assert(IsEqual(Irfft(Rfft(vector_h, 3), 3), vector_h));
  assert(IsEqual(Irfft(Rfft(vector_i, 4), 4), vector_i));
  
  // Test Irfft in strange cases
  std::vector<Complex> vector_l(3);
  vector_l[0] = Complex(1.0, 0.0);
  vector_l[1] = Complex(0.0, 1.0);
  vector_l[2] = Complex(2.0, 0.0);
  std::vector<Real> vector_l_3_cmp = {0.333333333333333,
                                      -0.244016935856292,
                                      0.910683602522959};
  assert(IsEqual(Irfft(vector_l, 3), vector_l_3_cmp));
  std::vector<Real> vector_l_6_cmp = {0.833333333333333, -0.455341801261480,
                                      -0.455341801261479, 0.833333333333333,
                                      0.122008467928146, 0.122008467928146};
  assert(IsEqual(Irfft(vector_l, 6), vector_l_6_cmp));
  
  std::vector<Complex> vector_m(vector_l); // Try out with even input
  vector_m.push_back(Complex(3.0, 0.0));
  std::vector<Real> vector_m_4_cmp = {0.75, -0.75, 0.75, 0.25};
  assert(IsEqual(Irfft(vector_m, 4), vector_m_4_cmp));
  std::vector<Real> vector_m_3_cmp = {0.333333333333333, -0.244016935856292,
                                      0.910683602522959};
  assert(IsEqual(Irfft(vector_m, 3), vector_m_3_cmp));
  std::vector<Real> vector_m_8_cmp = {1.375000000000000, -0.582106781186547,
                                      -0.625000000000000, 0.478553390593274,
                                      -0.125000000000000, 0.832106781186547,
                                      -0.125000000000000, -0.228553390593274};
  assert(IsEqual(Irfft(vector_m, 8), vector_m_8_cmp));
  std::vector<Real> vector_m_9_cmp = {1.222222222222222, -0.287886945411706,
                                      -0.858709554352006, 0.363105465825680,
                                      0.042237498424953, 0.194246451014139,
                                      0.748005645285431, -0.421017219679913,
                                      -0.002203563328800};
  assert(IsEqual(Irfft(vector_m, 9), vector_m_9_cmp));
  
  
  // Test hilbert transform
  std::vector<Real> vector_c(3);
  vector_c[0] = -0.3;
  vector_c[1] = 0.3;
  vector_c[2] = 2.4;
  
  
  std::vector<Complex> hilbert_vector_c = Hilbert(vector_c);
  std::vector<Complex> hilbert_vector_cmp_c(3);
  hilbert_vector_cmp_c[0] = Complex(-0.3000000000000000, 1.212435565298214);
  hilbert_vector_cmp_c[1] = Complex(0.300000000000000, -1.558845726811989);
  hilbert_vector_cmp_c[2] = Complex(2.400000000000000, 0.346410161513775);
  
  assert(IsEqual(hilbert_vector_c, hilbert_vector_cmp_c));
  
  
  std::vector<Real> vector_d(4);
  vector_d[0] = -0.3;
  vector_d[1] = 0.3;
  vector_d[2] = 2.4;
  vector_d[3] = -12.4;
  
  
  std::vector<Complex> hilbert_vector_d = Hilbert(vector_d);
  std::vector<Complex> hilbert_vector_cmp_d(4);
  hilbert_vector_cmp_d[0] = Complex(-0.300000000000000, -6.350000000000001);
  hilbert_vector_cmp_d[1] = Complex(0.300000000000001, -1.350000000000000);
  hilbert_vector_cmp_d[2] = Complex(2.399999999999999, 6.350000000000001);
  hilbert_vector_cmp_d[3] = Complex(-12.400000000000000, 1.350000000000000);
  
  assert(IsEqual(hilbert_vector_d, hilbert_vector_cmp_d));

  
  std::vector<Real> vector_f(4);
  vector_f[0] = 2.5;
  vector_f[1] = 1.3;
  vector_f[2] = -2.4;
  vector_f[3] = -1.0;
  
  
  std::vector<Real> rceps_vector_f = RCeps(vector_f);
  std::vector<Real> rceps_vector_f_cmp(4);
  rceps_vector_f_cmp[0] = 2.129647179286916e-01;
  rceps_vector_f_cmp[1] = 1.732867951399864e-01;
  rceps_vector_f_cmp[2] = -1.475829040082819e+00;
  rceps_vector_f_cmp[3] = 1.732867951399864e-01;
  assert(IsEqual(rceps_vector_f_cmp, rceps_vector_f));
  
  std::vector<Real> mphase_vector_f = MinPhase(vector_f);
  std::vector<Real> mphase_vector_f_cmp(4);
  mphase_vector_f_cmp[0] = 2.695552489391163e+00;
  mphase_vector_f_cmp[1] = 9.693272125606057e-01;
  mphase_vector_f_cmp[2] = -2.395552489391163e+00;
  mphase_vector_f_cmp[3] = -8.693272125606056e-01;
  assert(IsEqual(mphase_vector_f_cmp, mphase_vector_f));
  
  
  std::vector<Real> vector_g(3);
  vector_g[0] = 2.5;
  vector_g[1] = 0.0;
  vector_g[2] = -2.4;
  
  std::vector<Real> rceps_vector_g = RCeps(vector_g);
  std::vector<Real> rceps_vector_g_cmp(3);
  rceps_vector_g_cmp[0] = 1.961140220646066e-01;
  rceps_vector_g_cmp[1] = -1.249349557529326e+00;
  rceps_vector_g_cmp[2] = -1.249349557529326e+00;
  assert(IsEqual(rceps_vector_g_cmp, rceps_vector_g));
  
  std::vector<Real> mphase_vector_g = MinPhase(vector_g);
  std::vector<Real> mphase_vector_g_cmp(3);
  mphase_vector_g_cmp[0] = -1.548105780039157e+00;
  mphase_vector_g_cmp[1] = -1.207601874293259e+00;
  mphase_vector_g_cmp[2] = 2.855707654332416e+00;
  assert(IsEqual(mphase_vector_g_cmp, mphase_vector_g));

  
  return true;
}
  
} // namespace mcl