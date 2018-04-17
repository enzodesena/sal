/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

// This file contains definitions of matrix operations and classes

#ifndef MCL_MATRIXOP_H
#define MCL_MATRIXOP_H

#include <fstream>
#include <iostream>
#include <iomanip>
#include "mcltypes.h"
#include "comparisonop.h"
#include "basicop.h"
#include "elementaryop.h"

#if MCL_LOAD_EIGEN
  #include <Eigen/Dense>
#endif

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {

// Forward declaration
MCL_API std::vector<std::string> Split(const std::string& string,
                                       char delim) noexcept;
  
/** Matrix class */
template<class T>
class MCL_API Matrix {
public:
  /** Default constructor with empty (0x0) matrix */
  Matrix() noexcept : num_rows_(0), num_columns_(0) {}
  
  /** 
   Constructs a matrix with default entries
   */
  Matrix(Int num_rows, Int num_columns) noexcept : num_rows_(num_rows), 
          num_columns_(num_columns) {
    for (Int i=0; i<num_rows; ++i) {
      data_.push_back(std::vector<T>(num_columns));
    }
  }
  
  /**
   Constructs a matrix from a vector of vectors (outer vector represents rows)
   */
  Matrix(const std::vector<std::vector<T> > vectors) noexcept {
    num_rows_ = vectors.size();
    if (num_rows_ > 0) {
      num_columns_ = vectors[0].size();
      for (Int i=1; i<num_rows_; ++i) {
        // Check that all rows have the same number of columns
        if (vectors[i].size() != num_columns_) {
          ASSERT_WITH_MESSAGE(false, "One or more rows do not have the same number of columns");
        }
      }
      data_ = vectors;
    }
    else {
      num_columns_ = 0;
    }
  }
  
  /** Sets element in given row and column */
  void set_element(const Int& index_row, const Int& index_column,
                   const T& element) noexcept {
    if (index_row>=num_rows_ || index_column>=num_columns_) {
      ASSERT_WITH_MESSAGE(false, "Out-of-bounds index in setting matrix element");}
    data_[index_row][index_column] = element;
  }
  
  /** Sets an entire column */
  void set_column(Int index_column, std::vector<T> column) noexcept {
    if ((Int)column.size() != num_rows_) { ASSERT(false); }
    if (index_column >= num_columns_) { ASSERT(false); }
    for (Int i=0; i<num_rows_; ++i) {
      set_element(i, index_column, column[i]);
    }
  }
  
  /** Sets an entire row */
  void set_row(Int index_row, std::vector<T> row) noexcept {
    if ((Int)row.size() != num_columns_) { ASSERT(false); }
    if (index_row >= num_rows_) { ASSERT(false); }
    data_[index_row] = row;
  }
  
  /** Accesses an element in given row and column */
  T element(const Int& index_row, const Int& index_column) const noexcept {
    if (index_row >= num_rows_) { ASSERT(false); }
    if (index_column >= num_columns_) { ASSERT(false); }
    return data_[index_row][index_column];
  }
  
  /** Accesses an entire row */
  std::vector<T> row(Int index_row) const noexcept {
    if (index_row >= num_rows_) { ASSERT(false); }
    return data_[index_row];
  }
  
  /** Accesses an entire column */
  std::vector<T> column(Int index_column) const noexcept {
    if (index_column >= num_columns_) { ASSERT(false); }
    std::vector<T> output(num_rows_);
    for (Int i=0; i<num_rows_; ++i) { output[i] = data_[i][index_column]; }
    return output;
  }
  
  /** Returns the serialised matrix. Equivalent to Matlab's matrix(:) */
  std::vector<T> Serial() const noexcept {
    std::vector<T> serial(num_columns()*num_rows());
    
    Int k=0;
    for (Int j=0; j<num_columns(); ++j) {
      for (Int i=0; i<num_rows(); ++i) {
        serial[k++] = element(i, j);
      }
    }
    return serial;
  }
  
  /** Returns the number of rows */
  Int num_rows() const noexcept { return num_rows_; }
  
  /** Returns the number of columns */
  Int num_columns() const noexcept { return num_columns_; }
  
  /** Writes the matrix to a file. The optional parameter `precision` sets
   the number of decimal positions in the output file*/
  void Save(std::string file_name, mcl::Int precision = 5) {
    std::ofstream output_file;
    output_file.open(file_name.c_str());
    output_file<<std::fixed;
    output_file<<std::setprecision((int)precision);
    for (Int i=0; i<num_rows_; ++i) {
      for (Int j=0; j<num_columns_; ++j) {
        output_file<<data_.at(i).at(j)<<" ";
      }
      output_file<<std::endl;
    }
    output_file.close();
  }
  
  /** Returns the raw data */
  std::vector<std::vector<T> > data() noexcept { return data_; }
  
  /**
   Reads a matrix. Elements have to be separated by tabs and there
   must be no ending empty line (e.g. 5 lines == 5 rows).
   */
  static Matrix Load(std::string file_name) {
    std::string line;
    std::ifstream in_file (file_name.c_str());
    if (! in_file.is_open()) { ASSERT(false); }
    
    // First: lets count the number of rows
    Int number_of_rows = 0;
    Int number_of_columns = 0;
    while (std::getline(in_file, line)) {
      std::vector<std::string> elements = Split(line, '\t');
      if (number_of_columns == 0) { number_of_columns = elements.size(); }
      else {
        if (number_of_columns != elements.size()) { ASSERT(false); }
      }
      
      ++number_of_rows; 
    }
    
    // Reset pointer
    in_file.clear();
    in_file.seekg(0,std::ios::beg);
    
    // Create new matrix
    // TODO: recognize complex matrix
    Matrix<T> matrix(number_of_rows, number_of_columns);
    for(Int row=0; row<number_of_rows; ++row) {
      std::getline(in_file, line);
      std::vector<std::string> elements = Split(line, '\t');
      for (Int column=0; column<elements.size(); ++column) {
        matrix.set_element(row, column, (T) StringToDouble(elements[column]));
      }
    }
    
    in_file.close();
    return matrix;
  }
  
  /**
   Constructs a matrix of all ones. Equivalent to Matlab's ones(N,M).
   */
  static Matrix Ones(Int number_of_rows, Int number_of_columns) noexcept {
    Matrix<T> matrix(number_of_rows, number_of_columns);
    for(Int row=0; row<number_of_rows; ++row) {
      for (Int column=0; column<number_of_columns; ++column) {
        matrix.set_element(row, column, (T) 1.0);
      }
    }
    return matrix;
  }

  /**
   Constructs a matrix of all ones. Equivalent to Matlab's ones(N).
   */
  static Matrix Ones(Int matrix_dimension) noexcept {
    return Matrix<T>::Ones(matrix_dimension, matrix_dimension);
  }
  
private:
  // Outer is rows, inner is columns. Hence, data_[0] is the first column.
  std::vector<std::vector<T> > data_;
  Int num_rows_;
  Int num_columns_;
};
  
template<class T>
MCL_API void Print(const Matrix<T>& matrix) noexcept {
  for (Int i=0; i<matrix.num_rows(); ++i) {
    for (Int j=0; j<matrix.num_columns(); ++j) {
      std::cout<<matrix.element(i,j)<<"\t";
    }
    std::cout<<std::endl;
  }
}

/** Transposes the matrix. Equivalent to Matlab's matrix' */
template<class T>
MCL_API Matrix<T> Transpose(const Matrix<T>& matrix) noexcept {
  Matrix<T> output(matrix.num_columns(), matrix.num_rows());
  
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
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
MCL_API Matrix<T> Multiply(const Matrix<T>& matrix, T value) noexcept {
  Matrix<T> output(matrix.num_rows(), matrix.num_columns());
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
      output.set_element(i, j, matrix.element(i, j)*value);
    }
  }
  return output;
}
 
  
/** Matrix multiplication. Equivalent to Matlabs' matrix_a*matrix_b */
template<class T>
MCL_API Matrix<T> Multiply(const Matrix<T>& matrix_a,
                           const Matrix<T>& matrix_b) noexcept {
  if (matrix_a.num_columns() != matrix_b.num_rows()) { ASSERT(false); }
  
  Matrix<T> output(matrix_a.num_rows(), matrix_b.num_columns());
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
      T output_value = (T) 0.0;
      for (Int k=0; k<matrix_a.num_columns(); ++k) {
        output_value += matrix_a.element(i, k) * matrix_b.element(k, j);
      }
      output.set_element(i, j, output_value);
    }
  }
  return output;
}
  
template<class T>
MCL_API std::vector<T> Multiply(const Matrix<T>& matrix_a,
                                const std::vector<T>& vector) noexcept {
  if (matrix_a.num_columns() != (Int)vector.size()) { ASSERT(false); }
  Matrix<T> temp_input(vector.size(), 1);
  temp_input.set_column(0, vector);
  Matrix<T> temp_output = Multiply(matrix_a, temp_input);
  if (temp_output.num_columns() != 1) { ASSERT(false); }
  if (temp_output.num_rows() != (Int)vector.size()) { ASSERT(false); }
  
  return temp_output.column(0);
}
  
  
/** 
 Extract the maximum value of the matrix. Equivalent to Matlab's
 max(max(matrix)) 
 */
template<class T>
MCL_API T Max(const Matrix<T>& matrix) noexcept {
  return Max<T>(matrix.Serial());
}
  
  
/** Contains eigenvalues and eigenvectors */
struct EigOutput {
  std::vector<Complex> eigen_values; /**< Eigenvalues */
  std::vector<std::vector<Complex> > eigen_vectors; /**< Eigenvectors */
};
  
MCL_API EigOutput Eig(const Matrix<Real>& matrix) noexcept;

MCL_API Matrix<Real> RealPart(const Matrix<Complex>& input) noexcept;
  
#if MCL_LOAD_EIGEN
MCL_API Eigen::MatrixXd ConvertToEigen(const Matrix<Real>& input);
#endif
  
template<class T>
MCL_API bool IsEqual(const Matrix<T>& matrix_a, const Matrix<T>& matrix_b) noexcept {
  if (matrix_a.num_rows() != matrix_b.num_rows() |
      matrix_a.num_columns() != matrix_b.num_columns()) { return false; }
  
  for (Int i=0; i<matrix_a.num_rows(); ++i) {
    for (Int j=0; j<matrix_a.num_columns(); ++j) {
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
