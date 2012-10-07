/*
 matrixop.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
 */

// This file contains definitions of matrix operations and classes

#ifndef MCL_MATRIXOP_H
#define MCL_MATRIXOP_H

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "mcltypes.h"
#include "vectorop.h"
#include "equalityop.h"

namespace mcl {

template<class T>
class Matrix {
public:
  Matrix(UInt num_rows, UInt num_columns) : num_rows_(num_rows), 
          num_columns_(num_columns) {
    for (UInt i=0; i<num_rows; ++i) {
      data_.push_back(Zeros<T>(num_columns));
    }
  }
  
  void set_element(UInt index_row, UInt index_column, T element) {
    assert(index_row>=0 & index_row<num_rows_);
    assert(index_column>=0 & index_column<num_columns_);
    data_[index_row][index_column] = element;
  }
  
  T element(UInt index_row, UInt index_column) {
    assert(index_row>=0 & index_row<num_rows_);
    assert(index_column>=0 & index_column<num_columns_);
    return data_[index_row][index_column];
  }
  
  UInt num_rows() { return num_rows_; }
  
  UInt num_columns() { return num_columns_; }
  
  void Save(const char* file_name) {
    std::ofstream output_file;
    output_file.open(file_name);
    for (UInt i=0; i<num_rows_; ++i) {
      for (UInt j=0; j<num_columns_; ++j) { output_file<<data_[i][j]<<" "; }
      output_file<<std::endl;
    }
    output_file.close();
  }
  
  
  // Reads a matrix. Elements have to be separated by tabs (\t) and there 
  // must be no ending empty line (e.g. 5 lines == 5 rows).
  static Matrix Load(const char* file_name) {
    std::string line;
    std::ifstream in_file (file_name);
    assert(in_file.is_open());
      
    // First: lets count the number of rows
    UInt number_of_rows = 0;
    UInt number_of_columns = 0;
    while (std::getline(in_file, line)) {
      std::vector<std::string> elements = Split(line, '\t');
      if (number_of_columns == 0) { number_of_columns = (int) elements.size(); }
      else { assert(number_of_columns == elements.size()); }
      
      ++number_of_rows; 
    }
    
    // Reset pointer
    in_file.clear();
    in_file.seekg(0,std::ios::beg);
    
    // Create new matrix
    // TODO: recognize complex matrix
    Matrix<Real> matrix(number_of_rows, number_of_columns);
    for(UInt row=0; row<number_of_rows; ++row) {
      std::getline(in_file, line);
      std::vector<std::string> elements = Split(line, '\t');
      for (UInt column=0; column<elements.size(); ++column) {
        matrix.set_element(row, column, StringToDouble(elements[column]));
      }
    }
    
    in_file.close();
    return matrix;
  }
  
  
  static bool Test() {
    
    Matrix<Real> matrix_a(3,2);
    matrix_a.set_element(0, 1, 1.0);
    matrix_a.set_element(2, 1, 1.5);
    assert(IsEqual(matrix_a.element(0,1), 1.0));
    assert(IsEqual(matrix_a.element(0,0), 0.0));
    assert(IsEqual(matrix_a.element(2,1), 1.5));
    
    matrix_a.set_element(2, 1, 0.5);
    assert(IsEqual(matrix_a.element(2,1), 0.5));
    
    return true;
  }

  
private:
  
  std::vector<std::vector<T> > data_;
  UInt num_columns_;
  UInt num_rows_;
};

} // namespace mcl

#endif
