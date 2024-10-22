/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
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

namespace mcl {

// Forward declaration
std::vector<std::string> Split(const std::string& string,
                                       char delim) noexcept;
  
/** Matrix class */
template<class T>
class Matrix {
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
        if ((Int)vectors[i].size() != num_columns_) {
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
  void SetElement(const Int& index_row, const Int& index_column,
                   const T& element) noexcept {
    ASSERT_WITH_MESSAGE(index_row < num_rows_ && index_column < num_columns_,
                        "Out-of-bounds index in setting matrix element");
    data_[index_row][index_column] = element;
  }
  
  /** Sets an entire column */
  void SetColumn(Int index_column, std::vector<T> column) noexcept {
    ASSERT((Int)column.size() == num_rows_);
    ASSERT(index_column < num_columns_);
    for (Int i=0; i<num_rows_; ++i) {
      SetElement(i, index_column, column[i]);
    }
  }
  
  /** Sets an entire row */
  void SetRow(Int index_row, std::vector<T> row) noexcept {
    ASSERT((Int)row.size() == num_columns_);
    ASSERT(index_row < num_rows_);
    data_[index_row] = row;
  }
  
  /** Accesses an element in given row and column */
  T GetElement(const Int& index_row, const Int& index_column) const noexcept {
    ASSERT(index_row < num_rows_);
    ASSERT(index_column < num_columns_);
    return data_[index_row][index_column];
  }
  
  /** Accesses an entire row */
  std::vector<T> GetRow(Int index_row) const noexcept {
    ASSERT(index_row < num_rows_);
    return data_[index_row];
  }
  
  /** Accesses an entire column */
  std::vector<T> GetColumn(Int index_column) const noexcept {
    ASSERT(index_column < num_columns_);
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
        serial[k++] = GetElement(i, j);
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
  
  /** Same as Save(...), but before saving converts decimal numbers (assumed to be in
   the range -1, 1) to integers using the formula: scaledx=int(x^(`bit_precision`-1)-1)).
   If `bit_precision`=14, this is essentially writing out in a number representation equivalent
   to 24 bit WAV file. */
  void SaveIntegerFormat(std::string file_name, mcl::Int bit_precision = 24) {
    std::ofstream output_file;
    output_file.open(file_name.c_str());
    output_file<<std::fixed;
    output_file<<std::setprecision(0);
    for (Int i=0; i<num_rows_; ++i) {
      for (Int j=0; j<num_columns_; ++j) {
        output_file<<static_cast<int>(data_.at(i).at(j) * ((1 << (bit_precision-1)) - 1))<<" ";
      }
      output_file<<std::endl;
    }
    output_file.close();
  }
  
  /** Returns the raw data */
  std::vector<std::vector<T> > data() const noexcept { return data_; }
  
  /**
   Reads a matrix. Elements have to be separated by tabs and there
   must be no ending empty line (e.g. 5 lines == 5 rows).
   */
  static Matrix Load(std::string file_name) {
    std::string line;
    std::ifstream in_file (file_name.c_str());
    ASSERT(in_file.is_open());
    
    // First: lets count the number of rows
    Int number_of_rows = 0;
    Int number_of_columns = 0;
    while (std::getline(in_file, line)) {
      std::vector<std::string> elements = Split(line, '\t');
      if (number_of_columns == 0) { number_of_columns = (Int) elements.size(); }
      else {
        ASSERT(number_of_columns == (Int)elements.size());
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
      for (Int column=0; column<(Int)elements.size(); ++column) {
        matrix.SetElement(row, column, (T) StringToDouble(elements[column]));
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
        matrix.SetElement(row, column, (T) 1.0);
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
void Print(const Matrix<T>& matrix) noexcept {
  for (Int i=0; i<matrix.num_rows(); ++i) {
    for (Int j=0; j<matrix.num_columns(); ++j) {
      std::cout<<matrix.GetElement(i,j)<<"\t";
    }
    std::cout<<std::endl;
  }
}

/** Transposes the matrix. Equivalent to Matlab's matrix' */
template<class T>
Matrix<T> Transpose(const Matrix<T>& matrix) noexcept {
  Matrix<T> output(matrix.num_columns(), matrix.num_rows());
  
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
      output.SetElement(i, j, matrix.GetElement(j, i));
    }
  }
  return output;
}
  
/** 
 Multiplies all the elements of `matrix` by `value`. Equivalent
 to Matlabs' matrix.*value
 */
template<class T>
Matrix<T> Multiply(const Matrix<T>& matrix, T value) noexcept {
  Matrix<T> output(matrix.num_rows(), matrix.num_columns());
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
      output.SetElement(i, j, matrix.GetElement(i, j)*value);
    }
  }
  return output;
}
 
  
/** Matrix multiplication. Equivalent to Matlabs' matrix_a*matrix_b */
template<class T>
Matrix<T> Multiply(const Matrix<T>& matrix_a,
                           const Matrix<T>& matrix_b) noexcept {
  ASSERT(matrix_a.num_columns() == matrix_b.num_rows());
  
  Matrix<T> output(matrix_a.num_rows(), matrix_b.num_columns());
  for (Int i=0; i<output.num_rows(); ++i) {
    for (Int j=0; j<output.num_columns(); ++j) {
      T output_value = (T) 0.0;
      for (Int k=0; k<matrix_a.num_columns(); ++k) {
        output_value += matrix_a.GetElement(i, k) * matrix_b.GetElement(k, j);
      }
      output.SetElement(i, j, output_value);
    }
  }
  return output;
}
  

/** Carries out the mulitplication between  matrix_a * vector, where vector is a column vector.
 The output is also a column vector. */
template<class T>
std::vector<T> Multiply(const Matrix<T>& matrix_a,
                        const std::vector<T>& vector) noexcept {
  ASSERT(matrix_a.num_columns() == (Int)vector.size());
  Matrix<T> temp_input((Int) vector.size(), 1);
  temp_input.SetColumn(0, vector);
  Matrix<T> temp_output = Multiply(matrix_a, temp_input);
  ASSERT(temp_output.num_columns() == 1);
  ASSERT(temp_output.num_rows() == (Int)vector.size());
  
  return temp_output.GetColumn(0);
}
  

/** Carries out the mulitplication between vector * matrix_a, where vector is a row vector.
 The output is also a row vector. */
template<class T>
std::vector<T> Multiply(const std::vector<T>& vector,
                        const Matrix<T>& matrix_a) noexcept {
ASSERT(matrix_a.num_rows() == (Int)vector.size());
Matrix<T> temp_input(1, (Int) vector.size());
temp_input.SetRow(0, vector);
Matrix<T> temp_output = Multiply(temp_input, matrix_a);
ASSERT(temp_output.num_rows() == 1);
ASSERT(temp_output.num_columns() == (Int)vector.size());

return temp_output.GetRow(0);
}

template<class T>
Matrix<T> Inverse(const Matrix<T>& matrix_a) {
  size_t rows = matrix_a.num_rows();
  size_t cols = matrix_a.num_columns();
  ASSERT(rows == cols);
  std::vector<std::vector<T> > data = matrix_a.data();
  
  // Create the augmented matrix [A|I]
  for (size_t i = 0; i < rows; i++) {
    data[i].resize(2 * cols, 0.0);
    data[i][cols + i] = 1.0;
  }

  // Perform row operations
  for (size_t i = 0; i < rows; i++) {
    // Divide row by pivot
    Real pivot = data[i][i];
    for (size_t j = 0; j < 2 * cols; j++) {
      data[i][j] /= pivot;
    }
    
    // Eliminate other rows
    for (size_t j = 0; j < rows; j++)
    {
      if (j != i) {
        Real factor = data[j][i];
        for (size_t k = 0; k < 2 * cols; k++) {
          data[j][k] -= factor * data[i][k];
        }
      }
    }
  }

  // Remove the identity matrix
  for (size_t i = 0; i < rows; i++) {
    data[i].erase(data[i].begin(), data[i].begin() + cols);
    data[i].shrink_to_fit();
  }
  return mcl::Matrix(data);
}

  
/** 
 Extract the maximum value of the matrix. Equivalent to Matlab's
 max(max(matrix)) 
 */
template<class T>
T Max(const Matrix<T>& matrix) noexcept {
  return Max<T>(matrix.Serial());
}
  
  
/** Contains eigenvalues and eigenvectors */
struct EigOutput {
  std::vector<Complex> eigen_values; /**< Eigenvalues */
  std::vector<std::vector<Complex> > eigen_vectors; /**< Eigenvectors */
};
  
EigOutput Eig(const Matrix<Real>& matrix) noexcept;

Matrix<Real> RealPart(const Matrix<Complex>& input) noexcept;
  
#if MCL_LOAD_EIGEN
Eigen::MatrixXd ConvertToEigen(const Matrix<Real>& input);
#endif
  
template<class T>
bool IsEqual(const Matrix<T>& matrix_a, const Matrix<T>& matrix_b) noexcept {
  if (matrix_a.num_rows() != matrix_b.num_rows() |
      matrix_a.num_columns() != matrix_b.num_columns()) { return false; }
  
  for (Int i=0; i<matrix_a.num_rows(); ++i) {
    for (Int j=0; j<matrix_a.num_columns(); ++j) {
      if (!IsEqual(matrix_a.GetElement(i, j), matrix_b.GetElement(i, j))) {
        return  false;
      }
    }
  }
  return true;
}

bool MatrixOpTest();
  
  
} // namespace mcl

#endif
