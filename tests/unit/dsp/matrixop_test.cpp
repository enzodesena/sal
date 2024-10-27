/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "matrixop.h"

namespace mcl {

bool MatrixOpTest() {
  Matrix<Real> matrix_a(3, 2);
  matrix_a.SetElement(0, 1, 1.0);
  matrix_a.SetElement(2, 1, 1.5);
  ASSERT(IsEqual(matrix_a.GetElement(0, 1), 1.0));
  ASSERT(IsEqual(matrix_a.GetElement(0, 0), 0.0));
  ASSERT(IsEqual(matrix_a.GetElement(2, 1), 1.5));

  matrix_a.SetElement(2, 1, 0.5);
  ASSERT(IsEqual(matrix_a.GetElement(2, 1), 0.5));

  matrix_a.SetElement(0, 0, -0.3);

  std::vector<Real> compare_row_0(2);
  compare_row_0[0] = -0.3;
  compare_row_0[1] = 1.0;
  ASSERT(IsEqual(compare_row_0, matrix_a.GetRow(0)));
  std::vector<Real> compare_row_1(2);
  compare_row_1[0] = 0.0;
  compare_row_1[1] = 0.0;
  ASSERT(IsEqual(compare_row_1, matrix_a.GetRow(1)));

  std::vector<Real> compare_column_0(3);
  compare_column_0[0] = -0.3;
  compare_column_0[1] = 0.0;
  compare_column_0[2] = 0.0;
  ASSERT(IsEqual(compare_column_0, matrix_a.GetColumn(0)));
  std::vector<Real> compare_column_1(3);
  compare_column_1[0] = 1.0;
  compare_column_1[1] = 0.0;
  compare_column_1[2] = 0.5;
  ASSERT(IsEqual(compare_column_1, matrix_a.GetColumn(1)));

  std::vector<Real> set_column_1(3);
  set_column_1[0] = 1.0;
  set_column_1[1] = -2.0;
  set_column_1[2] = 0.6;
  matrix_a.SetColumn(1, set_column_1);
  ASSERT(IsEqual(set_column_1, matrix_a.GetColumn(1)));
  ASSERT(IsEqual(matrix_a.GetElement(1, 1), -2.0));

  std::vector<Real> set_row_0(2);
  set_row_0[0] = 0.7;
  set_row_0[1] = -1.0;
  matrix_a.SetRow(0, set_row_0);
  ASSERT(IsEqual(set_row_0, matrix_a.GetRow(0)));
  ASSERT(IsEqual(matrix_a.GetElement(0, 0), 0.7));

  Matrix<Real> matrix_b(3, 2);
  matrix_b.SetElement(0, 0, -1.0);
  matrix_b.SetElement(0, 1, 1.0);
  matrix_b.SetElement(2, 1, 2);
  Matrix<Real> matrix_c = Transpose(matrix_b);
  ASSERT(matrix_c.num_columns() == 3);
  ASSERT(matrix_c.num_rows() == 2);
  ASSERT(IsEqual(matrix_c.GetElement(0, 0), -1.0));
  ASSERT(IsEqual(matrix_c.GetElement(1, 0), 1.0));
  ASSERT(IsEqual(matrix_c.GetElement(0, 1), 0.0));
  ASSERT(IsEqual(matrix_c.GetElement(0, 2), 0.0));
  ASSERT(IsEqual(matrix_c.GetElement(1, 2), 2));

  Matrix<Real> matrix_d = Multiply(matrix_c, (Real)0.1);
  ASSERT(matrix_d.num_columns() == 3);
  ASSERT(matrix_d.num_rows() == 2);
  ASSERT(IsEqual(matrix_d.GetElement(0, 0), -1.0 * 0.1));
  ASSERT(IsEqual(matrix_d.GetElement(1, 0), 1.0 * 0.1));
  ASSERT(IsEqual(matrix_d.GetElement(0, 1), 0.0 * 0.1));
  ASSERT(IsEqual(matrix_d.GetElement(0, 2), 0.0 * 0.1));
  ASSERT(IsEqual(matrix_d.GetElement(1, 2), 2 * 0.1));

  // Testing matrix multiplication
  Matrix<Real> matrix_e(2, 2);
  matrix_e.SetElement(0, 0, -1.0);
  matrix_e.SetElement(0, 1, 1);
  matrix_e.SetElement(1, 0, 2);
  matrix_e.SetElement(1, 1, 3);
  Matrix<Real> mult_matrix_b_e = Multiply(matrix_b, matrix_e);
  ASSERT(mult_matrix_b_e.num_rows() == 3);
  ASSERT(mult_matrix_b_e.num_columns() == 2);
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(0, 0), 3.0));
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(0, 1), 2.0));
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(1, 0), 0.0));
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(1, 1), 0.0));
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(2, 0), 4.0));
  ASSERT(IsEqual(mult_matrix_b_e.GetElement(2, 1), 6.0));

  // Testing inverse matrix
  Matrix<Real> matrix_e_inverse = Inverse(matrix_e);
  ASSERT(IsEqual(matrix_e_inverse.GetElement(0, 0), -0.6));
  ASSERT(IsEqual(matrix_e_inverse.GetElement(0, 1), 0.2));
  ASSERT(IsEqual(matrix_e_inverse.GetElement(1, 0), 0.4));
  ASSERT(IsEqual(matrix_e_inverse.GetElement(1, 1), 0.2));

  // Testing matrix-vector multiplication
  std::vector<Real> vector_a(2);
  vector_a[0] = -3.0;
  vector_a[1] = 2.0;
  std::vector<Real> mult_matrix_vector = Multiply(matrix_e, vector_a);
  ASSERT(IsEqual(mult_matrix_vector[0], 5.0));
  ASSERT(IsEqual(mult_matrix_vector[1], 0.0));

  // Testing matrix equality
  ASSERT(!IsEqual(matrix_b, matrix_e));
  ASSERT(!IsEqual(matrix_e, matrix_a));
  ASSERT(IsEqual(matrix_e, matrix_e));
  ASSERT(IsEqual(matrix_b, matrix_b));

#if MCL_LOAD_EIGEN
  // Testing eigenvalues and eigenvectors
  EigOutput eig_e = Eig(matrix_e);
  ASSERT(eig_e.eigen_values.size() == 2);
  ASSERT(eig_e.eigen_vectors.size() == 2);

  ASSERT(IsEqual(eig_e.eigen_values[0], Complex(-1.449489742783179, 0.0)));
  ASSERT(IsEqual(eig_e.eigen_values[1], Complex(3.449489742783178, 0.0)));

  std::vector<Complex> eig_e_vector_0(2);
  eig_e_vector_0[0] = Complex(-0.912095586463013, 0.0);
  eig_e_vector_0[1] = Complex(0.409977610552932, 0.0);
  ASSERT(IsEqual(eig_e.eigen_vectors[0], eig_e_vector_0));
  std::vector<Complex> eig_e_vector_1(2);
  eig_e_vector_1[0] = Complex(-0.219275263435463, 0.0);
  eig_e_vector_1[1] = Complex(-0.975663035502170, 0.0);
  ASSERT(IsEqual(eig_e.eigen_vectors[1], eig_e_vector_1));
#endif

  // Testing matrix of ones
  Matrix<Real> matrix_f = Matrix<Real>::Ones(2, 3);
  Matrix<Real> matrix_f_cmp(2, 3);
  matrix_f_cmp.SetElement(0, 0, 1.0);
  matrix_f_cmp.SetElement(1, 0, 1.0);
  matrix_f_cmp.SetElement(0, 1, 1.0);
  matrix_f_cmp.SetElement(1, 1, 1.0);
  matrix_f_cmp.SetElement(0, 2, 1.0);
  matrix_f_cmp.SetElement(1, 2, 1.0);
  ASSERT(IsEqual(matrix_f, matrix_f_cmp));

  // Testing serialisation
  Matrix<Real> matrix_g(3, 2);
  matrix_g.SetElement(0, 1, 1.0);
  matrix_g.SetElement(2, 1, 1.5);
  std::vector<Real> serial_g = matrix_g.Serial();
  ASSERT(serial_g.size() == 6);
  std::vector<Real> serial_g_cmp(6, 0.0);
  serial_g_cmp[3] = 1.0;
  serial_g_cmp[5] = 1.5;
  ASSERT(IsEqual(serial_g, serial_g_cmp));

  // Testing matrix Max
  ASSERT(IsEqual(Max(matrix_g), 1.5));
  Matrix<Int> matrix_h(3, 2);
  matrix_h.SetElement(0, 1, 2);
  matrix_h.SetElement(2, 1, -5);
  ASSERT(Max(matrix_h) == 2);

  return true;
}

}  // namespace mcl
