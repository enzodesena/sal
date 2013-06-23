/*
 vectorop.h
 Matlab Cpp Library (MCL)
 
 This contains vector operations. This only include simple operations on vectors
 such as: creating a vector, zero padding, getting a subset, flipping etc.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_VECTOROP_H
#define MCL_VECTOROP_H


#include <cassert>
#include "mcltypes.h"
#include "elementaryop.h"
#include "basicop.h"
#include "matrixop.h"
#include <vector>
#include <iostream>


namespace mcl {

  
  
/** Equivalent to Matlab's length(input). */
template<class T>
UInt Length(const std::vector<T>& input) {
  return (UInt) input.size();
}


/** Returns a vector of zeros */
template <class T> 
std::vector<T> Zeros(UInt length) {
  //TODO: check if this returns zeros for all types
  return std::vector<T>(length);
}

template <class T> 
std::vector<T> EmptyVector() {
  return std::vector<T>();
}
  
/**
 Adds zero until the output vector has a length of total_length.
 If the length of input is smaller than total_length, than it returns the
 vector with the first total_length elements.
 */
template<class T> 
std::vector<T> ZeroPad(const std::vector<T>& input, UInt total_length) {
  std::vector<T> output = Zeros<T>(total_length);
  UInt M = std::min(input.size(), total_length);
  for (UInt i=0; i<M; ++i) { output[i] = input[i]; }
  return output;
}



/** 
 Returns the point by point multiplication of the vector with the gain.
 Equivalent to Matlab's vector_a.*gain.
 */
template<class T> 
std::vector<T> Multiply(const std::vector<T>& vector,
                        const T& gain) {
  std::vector<T> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) {
    output[i] = vector[i]*gain;
  }
  return output;
}

/** 
 Returns the point by point addition of the two vectors.
 Equivalent to Matlab's vector_a+vector_b.
 */
template<class T>
std::vector<T> Add(const std::vector<T>& vector_a,
                   const T scalar) {
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]+scalar;
  }
  return output;
}

  
/** 
 Returns the subset of elements with indexes from_index and to_index.
 Equivalent to Matlab's vector(from_index:to_index). (Careful about the
 different indexes convention between here and Matlab.
 */
template<class T> 
std::vector<T> Subset(const std::vector<T>& vector, 
                      const UInt from_index, const UInt to_index) {
  assert(from_index>=0 & from_index<vector.size() & 
         to_index>=0 & to_index<vector.size() & from_index<=to_index);
  // Allocate output vector with appropriate length.
  std::vector<T> output(to_index-from_index+1);
  UInt k = 0; // running index into new vector;
  for (UInt i=from_index; i<=to_index; ++i) {
    output[k++] = vector[i];
  }
  return output;
}


/** 
 Returns the concatenation of vector_a and vector_b. Equivalent to Matlab's
 [vector_a; vector_b].
 */
template<class T>
std::vector<T> Concatenate(std::vector<T> vector_a,
                           const std::vector<T>& vector_b) {
  std::vector<T> output = Zeros<T>(vector_a.size()+vector_b.size());
  vector_a.insert(vector_a.end(), vector_b.begin(), vector_b.end());
  return vector_a;
}


/** Returns a vector with only one element. */
template<class T> 
std::vector<T> UnaryVector(const T& element) {
  std::vector<T> output(1, element);
  return output;
}
  
/** Returns a vector with two elements. */
template<class T>
std::vector<T> BinaryVector(const T& element_a, const T& element_b) {
  std::vector<T> output(2);
  output[0] = element_a;
  output[1] = element_b;
  return output;
}

/**
 Flips the vector. Equivalent to matlab's flipud or fliplr (which for vectors
 are equivalent).
 */
template<class T>
std::vector<T> Flip(std::vector<T> vector) {
  if (vector.size() <= 1) { return vector; }
  UInt N(Length(vector));
  for (UInt i=0; i<=((UInt) (floor(N/2)-1)); ++i) {
    T temp_value = vector[i];
    vector[i] = vector[N-i-1];
    vector[N-i-1] = temp_value;
  }
  return vector;
}

/** 
 Equivalent to Matlab's circshift(vector, num_positions). A positive
 num_positions corresponds to a forward shift.
 */
template<class T>
std::vector<T> CircShift(const std::vector<T>& vector, Int num_positions) {
  UInt N = vector.size();
  std::vector<T> output(N);
  for (UInt i=0; i<N; ++i) {
    UInt index = (UInt) Mod(((Int) i) - num_positions, (Int) N);
    output[i] = vector[index];
  }
  
  return output;
}
  
/** Equivalent to Matlab's conv(vector_a, vector_b). */
template<class T>
std::vector<T> Conv(const std::vector<T>& vector_a, 
                    const std::vector<T>& vector_b) {
  UInt N_a = vector_a.size();
  UInt N_b = vector_b.size();
  UInt out_length = N_a+N_b-1;
  
  std::vector<T> moving_vector_temp = Concatenate(Zeros<T>(N_b-1), 
                                                  Flip(vector_a));
  std::vector<T> moving_vector_a = Concatenate(moving_vector_temp, 
                                               Zeros<T>(N_b-1));
  
  std::vector<T> output = Zeros<T>(out_length);
  for (UInt n=0; n<out_length; ++n) {
    for (UInt m=0; m<N_b; ++m) {
      output[out_length-n-1] += moving_vector_a[n+m]*vector_b[m];
    }
  }
  return output;
}
  
  
/** 
 Adds all the vectors and zero-pads short vectors if they have different
 lengths.
 */
template<class T>
std::vector<T> AddVectors(const std::vector<std::vector<T> >& vectors) {
  // Get maximum length
  std::vector<UInt> vector_lengths(vectors.size());
  for (UInt i=0; i<vectors.size(); ++i) {
    vector_lengths[i] = vectors[i].size();
  }
  UInt max_length(Max(vector_lengths));
  
  std::vector<T> output = Zeros<T>(max_length);
  for (UInt i=0; i<vectors.size(); ++i) {
    output = Add(output, ZeroPad(vectors[i], max_length));
  }
  
  return output;
}

/**
 This is equivalent to Matlab's from:to. E.g. 3:5 outputs a vector [3,4,5].
 TODO: Implement fractional input.
 */
template<class T>
std::vector<T> ColonOperator(const Int from, const Int to) {
  if ((to-from) < 0) { return EmptyVector<T>(); }
  const UInt vector_length = (UInt) (to-from+1);
  std::vector<T> output(vector_length);
  for (UInt i=0; i<vector_length; ++i) {
    output[i] = ((T) i) + ((T) from);
  }
  return output;
}
  
/** 
 This is equivalent to Matlab's from:step:to. E.g. 3:2:6 outputs a vector
 [3,4,6].
 */
// TODO: implement negative step and fractional input.
std::vector<Real>
ColonOperator(const Real from, const Real step, const Real to);
  

/** 
 Returns elements of vector `vector` from from_id to to_id
 (including extremes).
 */
template<class T>
std::vector<T> Elements(const std::vector<T>& vector,
                        const UInt from_id,
                        const UInt to_id) {
  return std::vector<T>(vector.begin() + ((Int)from_id),
                        vector.begin() + ((Int)to_id)+1);
}

  
template<class T>
std::vector<T> GetFrame(const std::vector<T>& vector,
                        const UInt frame_id,
                        const UInt frame_length) {
  UInt size(vector.size());
  
  UInt from_sample(frame_id * frame_length);
  UInt to_sample = Min(from_sample + frame_length - 1,
                       size - 1);
  if (to_sample > (size - 1)) { to_sample = size - 1; }
  // TODO: modify here (23/6/13: I am not sure what I meant here)
  return Elements(vector, from_sample, to_sample);
}

/** 
 Multiplies all the elements in the vector. Equivalent to Matlab's
 prod(vector).
 */
template<class T>
T Prod(const std::vector<T>& vector) {
  const UInt num_elements = vector.size();
  T output = (T) 1.0;
  for (UInt i=0; i<num_elements; ++i) { output *= vector[i]; }
  return output;
}
  
/** Dot product between two vectors. Equivalent to Matlab's dot(a,b) */
template<class T>
T Dot(const std::vector<T>& vector_a, const std::vector<T>& vector_b) {
  const UInt num_elements = vector_a.size();
  assert(num_elements == vector_b.size());
  T output = (T) 0.0;
  for (UInt i=0; i<num_elements; ++i) {
    output += vector_a[i]*vector_b[i];
  }
  return output;
}

Real Norm(const std::vector<Real>& vector, Real l_norm = 2.0);
  
template<class T>
void Print(const std::vector<T>& vector) {
  const UInt num_elements = vector.size();
  std::cout<<"\n------------\n";
  for (UInt i=0; i<num_elements; ++i) {
    std::cout<<vector[i]<<std::endl;
  }
  std::cout<<"------------\n";
}

/** Returns a real vector of `length` ones. */
std::vector<Real> Ones(UInt length);
  
  
std::vector<Real> Hann(const UInt length);
  
std::vector<Real> TukeyWin(const UInt length, const Real ratio);


/** Equivalent to Matlab's linspace(min, max, num_elements); */
std::vector<Real> LinSpace(Real min, Real max, UInt num_elements);
  
  
Real Sum(const std::vector<Real>& input);

/** Equivalent to Matlab's mean(input) */
Real Mean(const std::vector<Real>& input);
  
/**
 Weighted mean. Not implemented in Matlab (but should be). The weights are
 normalised inside the function. Hence Mean(input, ones(N)) gives the same
 result as Mean(input, ones(N)/N).
 */
Real Mean(const std::vector<Real>& input,
          const std::vector<Real>& weigths);
  
/** 
 Returns the standard deviation of the `input` vector. Equivalent to Matlab's
 std(input). This includes the correction for having an unbiased estimator.
 */
Real Std(const std::vector<Real>& input);

/** Var (unbiased estimator) */
Real Var(const std::vector<Real>& input);
  
/** Weighted var (biased estimator) */
Real Var(const std::vector<Real>& input, const std::vector<Real>& weights);
  
/** Equivalent to Matlab's xcorr(vect_a, vect_b) */
std::vector<Real> XCorr(const std::vector<Real>& vector_a,
                        const std::vector<Real>& vector_b);


/** 
 Writes the vector into a text file at `file_path` with an endline after
 each sample.
 */
void Save(const std::vector<Real>& vector, const char* file_path);  
  
/** Splits a string using a delimiter. */
std::vector<std::string> Split(const std::string& string, char delim);
  
/** Converts roots to polynomial. Equivalent to Matlab's poly(roots) */
std::vector<Complex> Poly(const std::vector<Complex> roots);
std::vector<Complex> Poly(const std::vector<Real> roots);
  
/** Returns true if all elements are non negative */
bool IsNonNegative(const std::vector<Real>& input);
  
/** Test function for the functions in this file */
bool VectorOpTest();
  
Matrix<Real> Cov(const std::vector<Real>& x, const std::vector<Real>& y);
  
Matrix<Real> Cov(const std::vector<std::vector<Real> >& input);
  
Real CovElement(const std::vector<Real>& x, const std::vector<Real>& y);
  
} /**< namespace mcl */

#endif
