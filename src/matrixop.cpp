/*
 matrixop.h
 Matlab Cpp Library (MCL)
 
 This contains matrix operations.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "matrixop.h"
#include "equalityop.h"

namespace mcl {
  
Matrix<Real> RealPart(const Matrix<Complex>& input) {
  Matrix<Real> output(input.num_rows(), input.num_columns());
  for (UInt i=0; i<input.num_rows(); ++i) {
    for (UInt j=0; j<input.num_columns(); ++j) {
      output.set_element(i, j, input.element(i, j).real());
    }
  }
  return output;
}
  
#if LOAD_EIGEN
Eigen::MatrixXd ConvertToEigen(const Matrix<Real>& input) {
  Eigen::MatrixXd output(input.num_rows(), input.num_columns());
  for (UInt i=0; i<input.num_rows(); ++i) {
    for (UInt j=0; j<input.num_columns(); ++j) {
      output(i, j) = input.element(i, j);
    }
  }
  return output;
}

  
  
EigOutput Eig(const Matrix<Real>& matrix) {
  assert(matrix.num_columns() == matrix.num_rows());
  const UInt N = matrix.num_columns();
  EigOutput output;
  output.eigen_values = std::vector<Complex>(N);
  output.eigen_vectors = std::vector<std::vector<Complex> >(N);
  
  // The following constructor triggers compute()
  Eigen::EigenSolver<Eigen::MatrixXd> es(ConvertToEigen(matrix));
  
  for (UInt value_i=0; value_i<N; ++value_i) {
    output.eigen_values[value_i] = es.eigenvalues()[value_i];
    std::vector<Complex> eigen_vector(N);
    for (UInt i=0; i<N; ++i) {
      eigen_vector[i] = es.eigenvectors()(i, value_i);
    }
    output.eigen_vectors[value_i] = eigen_vector;
  }
  return output;
}
#endif
  
} // namespace mcl

