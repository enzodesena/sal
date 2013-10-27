/*
 matrixop.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

// This file contains definitions of matrix operations and classes

#ifndef MCL_MATRIXOP_H
#define MCL_MATRIXOP_H

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mcltypes.h"
#include "comparisonop.h"
#include "elementaryop.h"

#if LOAD_EIGEN
#include <Eigen/Dense>
#endif

namespace mcl {

// Forward declaration
std::vector<std::string> Split(const std::string& string, char delim);
template<class T>
T Dot(const std::vector<T>& vector_a, const std::vector<T>& vector_b);
  
/** Matrix class */
template<class T>
class Matrix {
public:
  /** 
   Constructs a matrix with default entries
   */
  Matrix(UInt num_rows, UInt num_columns) : num_rows_(num_rows), 
          num_columns_(num_columns) {
    for (UInt i=0; i<num_rows; ++i) {
      data_.push_back(std::vector<T>(num_columns));
    }
  }
  
  /** Sets element in given row and column */
  void set_element(UInt index_row, UInt index_column, T element) {
    assert(index_row>=0 & index_row<num_rows_);
    assert(index_column>=0 & index_column<num_columns_);
    data_[index_row][index_column] = element;
  }
  
  /** Sets an entire column */
  void set_column(UInt index_column, std::vector<T> column) {
    assert(column.size() == num_rows_);
    assert(index_column>=0 & index_column<num_columns_);
    for (UInt i=0; i<num_rows_; ++i) {
      set_element(i, index_column, column[i]);
    }
  }
  
  /** Sets an entire row */
  void set_row(UInt index_row, std::vector<T> row) {
    assert(row.size() == num_columns_);
    assert(index_row>=0 & index_row<num_rows_);
    data_[index_row] = row;
  }
  
  /** Accesses an element in given row and column */
  T element(UInt index_row, UInt index_column) const {
    assert(index_row>=0 & index_row<num_rows_);
    assert(index_column>=0 & index_column<num_columns_);
    return data_[index_row][index_column];
  }
  
  /** Accesses an entire row */
  std::vector<T> row(UInt index_row) const {
    assert(index_row>=0 & index_row<num_rows_);
    return data_[index_row];
  }
  
  /** Accesses an entire column */
  std::vector<T> column(UInt index_column) const {
    assert(index_column>=0 & index_column<num_columns_);
    std::vector<T> output(num_rows_);
    for (UInt i=0; i<num_rows_; ++i) { output[i] = data_[i][index_column]; }
    return output;
  }
  
  /** Returns the number of rows */
  UInt num_rows() const { return num_rows_; }
  
  /** Returns the number of columns */
  UInt num_columns() const { return num_columns_; }
  
  /** Writes the matrix to a file */
  void Save(std::string file_name) {
    std::ofstream output_file;
    output_file.open(file_name.c_str());
    for (UInt i=0; i<num_rows_; ++i) {
      for (UInt j=0; j<num_columns_; ++j) { output_file<<data_.at(i).at(j)<<" "; }
      output_file<<std::endl;
    }
    output_file.close();
  }
  
  
  /**
   Reads a matrix. Elements have to be separated by tabs and there
   must be no ending empty line (e.g. 5 lines == 5 rows).
   */
  static Matrix Load(std::string file_name) {
    std::string line;
    std::ifstream in_file (file_name.c_str());
    assert(in_file.is_open());
    
    // First: lets count the number of rows
    UInt number_of_rows = 0;
    UInt number_of_columns = 0;
    while (std::getline(in_file, line)) {
      std::vector<std::string> elements = Split(line, '\t');
      if (number_of_columns == 0) { number_of_columns = elements.size(); }
      else { assert(number_of_columns == elements.size()); }
      
      ++number_of_rows; 
    }
    
    // Reset pointer
    in_file.clear();
    in_file.seekg(0,std::ios::beg);
    
    // Create new matrix
    // TODO: recognize complex matrix
    Matrix<T> matrix(number_of_rows, number_of_columns);
    for(UInt row=0; row<number_of_rows; ++row) {
      std::getline(in_file, line);
      std::vector<std::string> elements = Split(line, '\t');
      for (UInt column=0; column<elements.size(); ++column) {
        matrix.set_element(row, column, (T) StringToDouble(elements[column]));
      }
    }
    
    in_file.close();
    return matrix;
  }

  
private:
  // Outer is rows, inner is columns. Hence, data_[0] is the first column.
  std::vector<std::vector<T> > data_;
  UInt num_columns_;
  UInt num_rows_;
};
  
template<class T>
void Print(const Matrix<T>& matrix) {
  for (UInt i=0; i<matrix.num_rows(); ++i) {
    for (UInt j=0; j<matrix.num_columns(); ++j) {
      std::cout<<matrix.element(i,j)<<"\t";
    }
    std::cout<<std::endl;
  }
}

/** Transposes the matrix. Equivalent to Matlab's matrix' */
template<class T>
Matrix<T> Transpose(const Matrix<T>& matrix) {
  Matrix<T> output(matrix.num_columns(), matrix.num_rows());
  
  for (UInt i=0; i<output.num_rows(); ++i) {
    for (UInt j=0; j<output.num_columns(); ++j) {
      output.set_element(i, j, matrix.element(j, i));
    }
  }
  return output;
}
  
/** 
 Multiplies all the elements of `matrix` by `value`. Equivalent
 to Matlabs' matrix.*value
 */
template<class T>
Matrix<T> Multiply(const Matrix<T>& matrix, T value) {
  Matrix<T> output(matrix.num_rows(), matrix.num_columns());
  for (UInt i=0; i<output.num_rows(); ++i) {
    for (UInt j=0; j<output.num_columns(); ++j) {
      output.set_element(i, j, matrix.element(i, j)*value);
    }
  }
  return output;
}
 
  
/** Matrix multiplication. Equivalent to Matlabs' matrix_a*matrix_b */
template<class T>
Matrix<T> Multiply(const Matrix<T>& matrix_a, const Matrix<T>& matrix_b) {
  assert(matrix_a.num_columns() == matrix_b.num_rows());
  
  Matrix<T> output(matrix_a.num_rows(), matrix_b.num_columns());
  for (UInt i=0; i<output.num_rows(); ++i) {
    for (UInt j=0; j<output.num_columns(); ++j) {
      output.set_element(i, j, Dot(matrix_a.row(i), matrix_b.column(j)));
    }
  }
  return output;
}
  
template<class T>
std::vector<T>
Multiply(const Matrix<T>& matrix_a, const std::vector<T>& vector) {
  assert(matrix_a.num_columns() == vector.size());
  Matrix<T> temp_input(vector.size(), 1);
  temp_input.set_column(0, vector);
  Matrix<T> temp_output = Multiply(matrix_a, temp_input);
  assert(temp_output.num_columns() == 1);
  assert(temp_output.num_rows() == vector.size());
  
  return temp_output.column(0);
}
  
/** Contains eigenvalues and eigenvectors */
struct EigOutput {
  std::vector<Complex> eigen_values; /**< Eigenvalues */
  std::vector<std::vector<Complex> > eigen_vectors; /**< Eigenvectors */
};
  
EigOutput Eig(const Matrix<Real>& matrix);

Matrix<Real> RealPart(const Matrix<Complex>& input);
  
#if LOAD_EIGEN
Eigen::MatrixXd ConvertToEigen(const Matrix<Real>& input);
#endif
  
template<class T>
bool IsEqual(const Matrix<T>& matrix_a, const Matrix<T>& matrix_b) {
  if (matrix_a.num_rows() != matrix_b.num_rows() |
      matrix_a.num_columns() != matrix_b.num_columns()) { return false; }
  
  for (UInt i=0; i<matrix_a.num_rows(); ++i) {
    for (UInt j=0; j<matrix_a.num_columns(); ++j) {
      if (!IsEqual(matrix_a.element(i, j), matrix_b.element(i, j))) {
        return  false;
      }
    }
  }
  return true;
}

bool MatrixOpTest();
  
  
} // namespace mcl

#endif
