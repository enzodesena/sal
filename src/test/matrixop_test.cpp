/*
 vectorop_test.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 93 $
 Last changed date:  $Date: 2012-06-07 09:58:54 +0100 (Thu, 07 Jun 2012) $
 */


#include "matrixop.h"

namespace mcl {
  
bool MatrixOpTest() {
    
  Matrix<Real> matrix_a(3,2);
  matrix_a.set_element(0, 1, 1.0);
  matrix_a.set_element(2, 1, 1.5);
  assert(IsEqual(matrix_a.element(0,1), 1.0));
  assert(IsEqual(matrix_a.element(0,0), 0.0));
  assert(IsEqual(matrix_a.element(2,1), 1.5));

  matrix_a.set_element(2, 1, 0.5);
  assert(IsEqual(matrix_a.element(2,1), 0.5));

  matrix_a.set_element(0, 0, -0.3);

  std::vector<Real> compare_row_0(2);
  compare_row_0[0] = -0.3;
  compare_row_0[1] = 1.0;
  assert(IsEqual(compare_row_0, matrix_a.row(0)));
  std::vector<Real> compare_row_1(2);
  compare_row_1[0] = 0.0;
  compare_row_1[1] = 0.0;
  assert(IsEqual(compare_row_1, matrix_a.row(1)));

  std::vector<Real> compare_column_0(3);
  compare_column_0[0] = -0.3;
  compare_column_0[1] = 0.0;
  compare_column_0[2] = 0.0;
  assert(IsEqual(compare_column_0, matrix_a.column(0)));
  std::vector<Real> compare_column_1(3);
  compare_column_1[0] = 1.0;
  compare_column_1[1] = 0.0;
  compare_column_1[2] = 0.5;
  assert(IsEqual(compare_column_1, matrix_a.column(1)));

  std::vector<Real> set_column_1(3);
  set_column_1[0] = 1.0;
  set_column_1[1] = -2.0;
  set_column_1[2] = 0.6;
  matrix_a.set_column(1, set_column_1);
  assert(IsEqual(set_column_1, matrix_a.column(1)));
  assert(IsEqual(matrix_a.element(1,1), -2.0));

  std::vector<Real> set_row_0(2);
  set_row_0[0] = 0.7;
  set_row_0[1] = -1.0;
  matrix_a.set_row(0, set_row_0);
  assert(IsEqual(set_row_0, matrix_a.row(0)));
  assert(IsEqual(matrix_a.element(0,0), 0.7));

  Matrix<Real> matrix_b(3,2);
  matrix_b.set_element(0, 0, -1.0);
  matrix_b.set_element(0, 1, 1.0);
  matrix_b.set_element(2, 1, 2);
  Matrix<Real> matrix_c = Transpose(matrix_b);
  assert(matrix_c.num_columns() == 3);
  assert(matrix_c.num_rows() == 2);
  assert(IsEqual(matrix_c.element(0, 0), -1.0));
  assert(IsEqual(matrix_c.element(1, 0), 1.0));
  assert(IsEqual(matrix_c.element(0, 1), 0.0));
  assert(IsEqual(matrix_c.element(0, 2), 0.0));
  assert(IsEqual(matrix_c.element(1, 2), 2));

  Matrix<Real> matrix_d = Multiply<Real>(matrix_c, (Real) 0.1);
  assert(matrix_d.num_columns() == 3);
  assert(matrix_d.num_rows() == 2);
  assert(IsEqual(matrix_d.element(0, 0), -1.0*0.1));
  assert(IsEqual(matrix_d.element(1, 0), 1.0*0.1));
  assert(IsEqual(matrix_d.element(0, 1), 0.0*0.1));
  assert(IsEqual(matrix_d.element(0, 2), 0.0*0.1));
  assert(IsEqual(matrix_d.element(1, 2), 2*0.1));

  // Testing matrix multiplication
  Matrix<Real> matrix_e(2,2);
  matrix_e.set_element(0, 0, -1.0);
  matrix_e.set_element(0, 1, 1);
  matrix_e.set_element(1, 0, 2);
  matrix_e.set_element(1, 1, 3);
  Matrix<Real> mult_matrix_b_e = Multiply(matrix_b, matrix_e);
  assert(mult_matrix_b_e.num_rows() == 3);
  assert(mult_matrix_b_e.num_columns() == 2);
  assert(IsEqual(mult_matrix_b_e.element(0, 0), 3.0));
  assert(IsEqual(mult_matrix_b_e.element(0, 1), 2.0));
  assert(IsEqual(mult_matrix_b_e.element(1, 0), 0.0));
  assert(IsEqual(mult_matrix_b_e.element(1, 1), 0.0));
  assert(IsEqual(mult_matrix_b_e.element(2, 0), 4.0));
  assert(IsEqual(mult_matrix_b_e.element(2, 1), 6.0));
  
  // Testing matrix-vector multiplication
  std::vector<Real> vector_a(2);
  vector_a[0] = -3.0;
  vector_a[1] = 2.0;
  std::vector<Real> mult_matrix_vector = Multiply(matrix_e, vector_a);
  assert(IsEqual(mult_matrix_vector[0], 5.0));
  assert(IsEqual(mult_matrix_vector[1], 0.0));
  
  
  // Testing matrix equality
  assert(! IsEqual(matrix_b, matrix_e));
  assert(! IsEqual(matrix_e, matrix_a));
  assert(IsEqual(matrix_e, matrix_e));
  assert(IsEqual(matrix_b, matrix_b));
  
  // Testing eigenvalues and eigenvectors
  EigOutput eig_e = Eig(matrix_e);
  assert(eig_e.eigen_values.size() == 2);
  assert(eig_e.eigen_vectors.size() == 2);
  
  assert(IsEqual(eig_e.eigen_values[0], Complex(-1.449489742783179, 0.0)));
  assert(IsEqual(eig_e.eigen_values[1], Complex(3.449489742783178, 0.0)));
  
  std::vector<Complex> eig_e_vector_0(2);
  eig_e_vector_0[0] = Complex(-0.912095586463013, 0.0);
  eig_e_vector_0[1] = Complex(0.409977610552932, 0.0);
  assert(IsEqual(eig_e.eigen_vectors[0], eig_e_vector_0));
  std::vector<Complex> eig_e_vector_1(2);
  eig_e_vector_1[0] = Complex(-0.219275263435463, 0.0);
  eig_e_vector_1[1] = Complex(-0.975663035502170, 0.0);
  assert(IsEqual(eig_e.eigen_vectors[1], eig_e_vector_1));
  
  
  return true;
}

} // namespace mcl
