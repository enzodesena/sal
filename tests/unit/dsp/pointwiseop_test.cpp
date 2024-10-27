/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "comparisonop.h"
#include "mcltypes.h"
#include "pointwiseop.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

bool PointWiseOpTest() {
  std::vector<Complex> vector_a(3);
  vector_a[0] = Complex(1.0, 0.0);
  vector_a[1] = Complex(0.0, 1.0);
  vector_a[2] = Complex(1.0, 0.5);

  ASSERT(!IsReal(vector_a));

  std::vector<Real> abs_vector_a = Abs(vector_a);
  std::vector<Real> abs_vector_a_cmp(3);
  abs_vector_a_cmp[0] = 1.0;
  abs_vector_a_cmp[1] = 1.0;
  abs_vector_a_cmp[2] = 1.118033988749895;

  ASSERT(IsEqual(abs_vector_a, abs_vector_a_cmp));

  std::vector<Complex> vector_b(3);
  vector_b[0] = Complex(0.5, 1.0);
  vector_b[1] = Complex(0.2, -1.0);
  vector_b[2] = Complex(-1.0, -0.5);

  std::vector<Complex> mult_vector = Multiply(vector_a, vector_b);
  std::vector<Complex> mult_vector_cmp(3);
  mult_vector_cmp[0] = Complex(0.5, 1.0);
  mult_vector_cmp[1] = Complex(1.0, 0.2);
  mult_vector_cmp[2] = Complex(-0.75, -1.0);

  ASSERT(IsEqual(mult_vector, mult_vector_cmp));

  std::vector<Complex> add_vector = Add(vector_a, vector_b);
  std::vector<Complex> add_vector_cmp(3);
  add_vector_cmp[0] = Complex(1.5, 1.0);
  add_vector_cmp[1] = Complex(0.2, 0.0);
  add_vector_cmp[2] = Complex(0.0, 0.0);

  ASSERT(IsEqual(add_vector, add_vector_cmp));

  std::vector<Complex> sub_vector = Subtract(vector_a, vector_b);
  std::vector<Complex> sub_vector_cmp(3);
  sub_vector_cmp[0] = Complex(0.5, -1.0);
  sub_vector_cmp[1] = Complex(-0.2, +2.0);
  sub_vector_cmp[2] = Complex(2.0, 1.0);

  ASSERT(IsEqual(sub_vector, sub_vector_cmp));

  std::vector<Real> vector_c(3);
  vector_c[0] = -0.3;
  vector_c[1] = 0.3;
  vector_c[2] = 2.4;

  std::vector<Real> vector_c_inv = Inverse(vector_c);
  std::vector<Real> vector_c_inv_cmp(3);
  vector_c_inv_cmp[0] = 1.0 / (-0.3);
  vector_c_inv_cmp[1] = 1.0 / (0.3);
  vector_c_inv_cmp[2] = 1.0 / (2.4);
  ASSERT(IsEqual(vector_c_inv, vector_c_inv_cmp));

  std::vector<Complex> vector_cc = ComplexVector(vector_c);
  std::vector<Complex> vector_cc_cmp(3);
  vector_cc_cmp[0] = Complex(-0.3, 0.0);
  vector_cc_cmp[1] = Complex(0.3, 0.0);
  vector_cc_cmp[2] = Complex(2.4, 0.0);

  ASSERT(IsReal(vector_cc));

  ASSERT(IsEqual(vector_cc, vector_cc_cmp));

  std::vector<Real> vector_e(4);
  vector_e[0] = -0.3;
  vector_e[1] = 30.3;
  vector_e[2] = 2.4;
  vector_e[3] = 12.4;

  // Testing Pow
  std::vector<Real> pow_vector_e = Pow(vector_e, 3.0);

  std::vector<Real> pow_vector_e_cmp(4);
  pow_vector_e_cmp[0] = pow(vector_e[0], 3.0);
  pow_vector_e_cmp[1] = pow(vector_e[1], 3.0);
  pow_vector_e_cmp[2] = pow(vector_e[2], 3.0);
  pow_vector_e_cmp[3] = pow(vector_e[3], 3.0);

  ASSERT(IsEqual(pow_vector_e, pow_vector_e_cmp));

  std::vector<Real> abs_vector_e = Abs(vector_e);
  std::vector<Real> abs_vector_e_cmp(4);
  abs_vector_e_cmp[0] = 0.3;
  abs_vector_e_cmp[1] = 30.3;
  abs_vector_e_cmp[2] = 2.4;
  abs_vector_e_cmp[3] = 12.4;

  ASSERT(IsEqual(abs_vector_e, abs_vector_e_cmp));

  std::vector<Real> abs_pow_vector_e = Pow(abs_vector_e, -1.0 / 3.0);

  std::vector<Real> abs_pow_vector_e_cmp(4);
  abs_pow_vector_e_cmp[0] = pow(abs_vector_e[0], -1.0 / 3.0);
  abs_pow_vector_e_cmp[1] = pow(abs_vector_e[1], -1.0 / 3.0);
  abs_pow_vector_e_cmp[2] = pow(abs_vector_e[2], -1.0 / 3.0);
  abs_pow_vector_e_cmp[3] = pow(abs_vector_e[3], -1.0 / 3.0);

  ASSERT(IsEqual(abs_pow_vector_e, abs_pow_vector_e_cmp));

  std::vector<Real> hw_vector_e = HalfWave(vector_e);
  std::vector<Real> hw_vector_e_cmp(4);
  hw_vector_e_cmp[0] = 0.0;
  hw_vector_e_cmp[1] = 30.3;
  hw_vector_e_cmp[2] = 2.4;
  hw_vector_e_cmp[3] = 12.4;

  ASSERT(IsEqual(hw_vector_e, hw_vector_e_cmp));

  std::vector<Real> vector_o = Zeros<Real>(3);
  vector_o[0] = 1.0;
  vector_o[1] = 2.5;
  vector_o[2] = 4.2;

  std::vector<Real> log_vector_o = Log(vector_o);
  std::vector<Real> log_vector_o_cmp(3);
  log_vector_o_cmp[0] = 0.0;
  log_vector_o_cmp[1] = 0.916290731874155;
  log_vector_o_cmp[2] = 1.435084525289323;
  ASSERT(IsEqual(log_vector_o_cmp, log_vector_o));

  std::vector<Real> log10_vector_o = Log10(vector_o);
  std::vector<Real> log10_vector_o_cmp(3);
  log10_vector_o_cmp[0] = 0.0;
  log10_vector_o_cmp[1] = 0.397940008672038;
  log10_vector_o_cmp[2] = 0.623249290397900;
  ASSERT(IsEqual(log10_vector_o_cmp, log10_vector_o));

  std::vector<Complex> exp_vector_a = Exp(vector_a);
  std::vector<Complex> exp_vector_a_cmp(3);
  exp_vector_a_cmp[0] = Complex(2.718281828459046, 0.0);
  exp_vector_a_cmp[1] = Complex(0.5403023058681398, 0.8414709848078965);
  exp_vector_a_cmp[2] = Complex(2.385516730959136, 1.303213729686996);
  ASSERT(IsEqual(exp_vector_a_cmp, exp_vector_a));

  std::vector<Real> colonop_a = ColonOperator<Real>(2, 4);
  std::vector<Real> colonop_a_cmp = Zeros<Real>(3);
  colonop_a_cmp[0] = 2.0;
  colonop_a_cmp[1] = 3.0;
  colonop_a_cmp[2] = 4.0;

  std::vector<Real> cosvector = Cos(colonop_a_cmp);
  std::vector<Real> cosvector_cmp(3);
  cosvector_cmp[0] = cos(2.0);
  cosvector_cmp[1] = cos(3.0);
  cosvector_cmp[2] = cos(4.0);
  ASSERT(IsEqual(cosvector_cmp, cosvector));

  std::vector<Real> sinvector = Sin(colonop_a_cmp);
  std::vector<Real> sinvector_cmp(3);
  sinvector_cmp[0] = sin(2.0);
  sinvector_cmp[1] = sin(3.0);
  sinvector_cmp[2] = sin(4.0);
  ASSERT(IsEqual(sinvector_cmp, sinvector));

  // Testing Divide
  //  std::vector<Real> vector_o = Zeros<Real>(3);
  //  vector_o[0] = 1.0;
  //  vector_o[1] = 2.5;
  //  vector_o[2] = 4.2;
  std::vector<Real> vector_p(3);
  vector_p[0] = -1.4;
  vector_p[1] = 2.3;
  vector_p[2] = 4.2;
  std::vector<Real> division_o_p = Divide(vector_o, vector_p);
  std::vector<Real> division_o_p_cmp(3);
  division_o_p_cmp[0] = 1.0 / (-1.4);
  division_o_p_cmp[1] = 2.5 / 2.3;
  division_o_p_cmp[2] = 4.2 / 4.2;
  ASSERT(IsEqual(division_o_p_cmp, division_o_p));

  return true;
}
} // namespace dsp

} // namespace sal
