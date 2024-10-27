/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_VECTOROP_H
#define MCL_VECTOROP_H

#include <iostream>
#include <vector>

#include "basicop.h"
#include "digitalfilter.h"
#include "elementaryop.h"
#include "matrixop.h"
#include "mcltypes.h"

namespace sal {

namespace dsp {

/** Equivalent to Matlab's length(input). */
template <class T>
size_t Length(const std::vector<T>& input) noexcept {
  return input.size();
}

/** Returns a vector of zeros */
template <class T>
std::vector<T> Zeros(Int length) noexcept {
  // TODO: check if this returns zeros for all types
  return std::vector<T>(length);
}

template <class T>
std::vector<T> EmptyVector() noexcept {
  return std::vector<T>();
}

/**
 Adds zero until the output vector has a length of total_length.
 If the length of input is smaller than total_length, than it returns the
 vector with the first total_length elements.
 */
template <class T>
std::vector<T> ZeroPad(const std::vector<T>& input,
                       const Int total_length) noexcept {
  std::vector<T> output = Zeros<T>(total_length);
  Int M = ((Int)input.size() < total_length) ? input.size() : total_length;
  for (Int i = 0; i < M; ++i) {
    output[i] = input[i];
  }
  return output;
}

/**
 Returns the point by point multiplication of the vector with the gain.
 Equivalent to Matlab's vector_a.*gain.
 */
template <class T>
std::vector<T> Multiply(const std::vector<T>& vector, const T gain) noexcept {
  std::vector<T> output(vector.size());
  for (Int i = 0; i < (Int)vector.size(); ++i) {
    output[i] = vector[i] * gain;
  }
  return output;
}

void Multiply(std::span<const Real> input_data, const Real gain,
              std::span<Real> output_data) noexcept;

inline std::vector<Real> Multiply(std::span<const Real> input,
                                  const Real gain) noexcept {
  std::vector<Real> output(input.size());
  Multiply(input, gain, output);
  return output;
}

/** This calculates the multiplication of a vector (`input_data_mult`)
 by a constant (`gain`), and then adds the resulting vector to
 another vector (`input_data_add'). */
void MultiplyAdd(std::span<const Real> input_data_mult, const Real gain,
                 std::span<const Real> input_data_add,
                 std::span<Real> output_data) noexcept;

/**
 Returns the point by point addition of the two vectors.
 Equivalent to Matlab's vector_a+vector_b.
 */
template <class T>
std::vector<T> Add(const std::vector<T>& vector_a, const T scalar) noexcept {
  std::vector<T> output((Int)vector_a.size());
  for (Int i = 0; i < (Int)vector_a.size(); ++i) {
    output[i] = vector_a[i] + scalar;
  }
  return output;
}

Real Add(const Real* input_data, const Int num_samples) noexcept;

/**
 Returns the subset of elements with indexes from_index and to_index.
 Equivalent to Matlab's vector(from_index:to_index). (Careful about the
 different indexes convention between here and Matlab.
 */
template <class T>
std::vector<T> Subset(const std::vector<T>& vector, const Int from_index,
                      const Int to_index) noexcept {
  ASSERT(from_index < (Int)vector.size());
  ASSERT(to_index < (Int)vector.size());
  ASSERT(from_index <= to_index);

  return std::vector<T>(vector.begin() + from_index,
                        vector.begin() + to_index + 1);
}

/**
 Returns the concatenation of vector_a and vector_b. Equivalent to Matlab's
 [vector_a; vector_b].
 */
template <class T>
std::vector<T> Concatenate(std::vector<T> vector_a,
                           const std::vector<T>& vector_b) noexcept {
  std::vector<T> output = Zeros<T>((Int)vector_a.size() + (Int)vector_b.size());
  vector_a.insert(vector_a.end(), vector_b.begin(), vector_b.end());
  return vector_a;
}

/** Returns a vector with only one element. */
template <class T>
std::vector<T> UnaryVector(const T& element) noexcept {
  std::vector<T> output(1, element);
  return output;
}

/** Returns a vector with two elements. */
template <class T>
std::vector<T> BinaryVector(const T& element_a, const T& element_b) noexcept {
  std::vector<T> output(2);
  output[0] = element_a;
  output[1] = element_b;
  return output;
}

/**
 Flips the vector. Equivalent to matlab's flipud or fliplr (which for vectors
 are equivalent).
 */
template <class T>
std::vector<T> Flip(std::vector<T> vector) noexcept {
  if (vector.size() <= 1) {
    return vector;
  }
  Int N(Length(vector));
  for (Int i = 0; i <= ((Int)(floor(N / 2) - 1)); ++i) {
    T temp_value = vector[i];
    vector[i] = vector[N - i - 1];
    vector[N - i - 1] = temp_value;
  }
  return vector;
}

/**
 Equivalent to Matlab's circshift(vector, num_positions). A positive
 num_positions corresponds to a forward shift.
 */
template <class T>
std::vector<T> CircShift(const std::vector<T>& vector,
                         Int num_positions) noexcept {
  Int N = vector.size();
  std::vector<T> output(N);
  for (Int i = 0; i < N; ++i) {
    Int index = (UInt)Mod(((Int)i) - num_positions, (Int)N);
    output[i] = vector[index];
  }

  return output;
}

/** Equivalent to Matlab's conv(vector_a, vector_b). */
template <class T>
std::vector<T> Conv(const std::vector<T>& vector_a,
                    const std::vector<T>& vector_b) noexcept {
  Int N_a = (Int)vector_a.size();
  Int N_b = (Int)vector_b.size();
  Int out_length = N_a + N_b - 1;

  std::vector<T> moving_vector_temp =
      Concatenate(Zeros<T>(N_b - 1), Flip(vector_a));
  std::vector<T> moving_vector_a =
      Concatenate(moving_vector_temp, Zeros<T>(N_b - 1));

  std::vector<T> output = Zeros<T>(out_length);
  for (Int n = 0; n < out_length; ++n) {
    for (Int m = 0; m < N_b; ++m) {
      output[out_length - n - 1] += moving_vector_a[n + m] * vector_b[m];
    }
  }
  return output;
}

/**
 Adds all the vectors and zero-pads short vectors if they have different
 lengths.
 */
template <class T>
std::vector<T> AddVectors(
    const std::vector<std::vector<T> >& vectors) noexcept {
  // Get maximum length
  std::vector<Int> vector_lengths(vectors.size());
  for (Int i = 0; i < (Int)vectors.size(); ++i) {
    vector_lengths[i] = (Int)vectors[i].size();
  }
  Int max_length(Max(vector_lengths));

  std::vector<T> output = Zeros<T>(max_length);
  for (Int i = 0; i < (Int)vectors.size(); ++i) {
    output = Add(output, ZeroPad(vectors[i], max_length));
  }

  return output;
}

/**
 Adds two vectors and zero-pads the shorter one if they have different
 lengths.
 */
template <class T>
std::vector<T> AddVectors(const std::vector<T>& vector_a,
                          const std::vector<T>& vector_b) noexcept {
  // Get maximum length
  Int max_length(Max((Int)vector_a.size(), (Int)vector_b.size()));

  std::vector<T> output = Zeros<T>(max_length);
  output = Add(output, ZeroPad(vector_a, max_length));
  output = Add(output, ZeroPad(vector_b, max_length));

  return output;
}

/** Interleaves two vectors, with the first element of `vector_a` going
 first.*/
template <class T>
std::vector<T> Interleave(const std::vector<T>& vector_a,
                          const std::vector<T>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());

  std::vector<T> output;
  for (Int i = 0; i < (Int)vector_a.size(); ++i) {
    output.push_back(vector_a[i]);
    output.push_back(vector_b[i]);
  }
  return output;
}

/** Decreases the sampling frequency of the input vector by keeping
 the first sample and then every `downsampling_factor`-th sample after the
 first. */
template <class T>
std::vector<T> Downsample(const std::vector<T>& vector,
                          const Int downsampling_factor) noexcept {
  ASSERT(downsampling_factor >= 1);
  std::vector<T> output;
  for (Int i = 0; i < (Int)vector.size(); i += downsampling_factor) {
    output.push_back(vector[i]);
  }
  return output;
}

/**
 This is equivalent to Matlab's from:to. E.g. 3:5 outputs a vector [3,4,5].
 TODO: Implement fractional input.
 */
template <class T>
std::vector<T> ColonOperator(const Int from, const Int to) noexcept {
  if ((to - from) < 0) {
    return EmptyVector<T>();
  }
  const Int vector_length = (UInt)(to - from + 1);
  std::vector<T> output(vector_length);
  for (Int i = 0; i < vector_length; ++i) {
    output[i] = ((T)i) + ((T)from);
  }
  return output;
}

/**
 This is equivalent to Matlab's from:step:to. E.g. 3:2:6 outputs a vector
 [3,4,6].
 */
// TODO: implement negative step and fractional input.
std::vector<Real> ColonOperator(const Real from, const Real step,
                                const Real to) noexcept;

/**
 Returns elements of vector `vector` from from_id to to_id
 (including extremes).
 */
template <class T>
std::vector<T> Elements(const std::vector<T>& vector, const Int from_id,
                        const Int to_id) noexcept {
  return std::vector<T>(vector.begin() + ((Int)from_id),
                        vector.begin() + ((Int)to_id) + 1);
}

template <class T>
std::vector<T> GetSegment(const std::vector<T>& vector, const Int subset_id,
                          const Int subset_length,
                          bool zeropad_if_shorter = false) noexcept {
  const Int size = vector.size();

  const Int from_sample = subset_id * subset_length;
  if (from_sample >= size) {
    if (zeropad_if_shorter) {
      return Zeros<T>(subset_length);
    } else {
      return std::vector<T>();  // Return empty vector
    }
  }

  const Int to_sample = Min(from_sample + subset_length - 1, size - 1);

  const Int actual_length = to_sample - from_sample + 1;
  if (zeropad_if_shorter && actual_length < subset_length) {
    return ZeroPad(Elements(vector, from_sample, to_sample), subset_length);
  } else {
    return Elements(vector, from_sample, to_sample);
  }
}

/**
 Multiplies all the elements in the vector. Equivalent to Matlab's
 prod(vector).
 */
template <class T>
T Prod(const std::vector<T>& vector) noexcept {
  const Int num_elements = vector.size();
  T output = (T)1.0;
  for (Int i = 0; i < num_elements; ++i) {
    output *= vector[i];
  }
  return output;
}

/** Dot product between two vectors. Equivalent to Matlab's dot(a,b) */
template <class T>
T Dot(const std::vector<T>& vector_a, const std::vector<T>& vector_b) noexcept {
  const Int num_elements = (Int)vector_a.size();
  ASSERT(num_elements == (Int)vector_b.size());

  T output = (T)0.0;
  for (Int i = 0; i < num_elements; ++i) {
    output += vector_a[i] * vector_b[i];
  }
  return output;
}

Real Norm(const std::vector<Real>& vector, Real l_norm = 2.0) noexcept;

template <class T>
void Print(const T* vector, const Int num_elements) noexcept {
  std::cout << "\n------------\n";
  for (Int i = 0; i < num_elements; ++i) {
    std::cout << vector[i] << std::endl;
  }
  std::cout << "------------\n";
}

template <class T>
void Print(const std::vector<T>& vector) noexcept {
  Print(vector.data(), vector.size());
}

/** Returns a real vector of `length` ones. */
std::vector<Real> Ones(Int length) noexcept;

std::vector<Real> Hann(const Int length) noexcept;

/** Returns a Hamming window of length `length' */
std::vector<Real> Hamming(const Int length) noexcept;

std::vector<Real> TukeyWin(const Int length, const Real ratio) noexcept;

/** Equivalent to Matlab's linspace(min, max, num_elements); */
std::vector<Real> LinSpace(Real min, Real max, Int num_elements) noexcept;

Real Sum(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's mean(input) */
Real Mean(const std::vector<Real>& input) noexcept;

/**
 Returns the geometric mean of the input vector. Equivalent
 to Matlab's geomean(input)
 **/
Real Geomean(const std::vector<Real>& input) noexcept;

/**
 Weighted mean. Not implemented in Matlab (but should be). The weights are
 normalised inside the function. Hence Mean(input, ones(N)) gives the same
 result as Mean(input, ones(N)/N).
 */
Real Mean(const std::vector<Real>& input,
          const std::vector<Real>& weigths) noexcept;

/**
 Returns the standard deviation of the `input` vector. Equivalent to Matlab's
 std(input). This includes the correction for having an unbiased estimator.
 */
Real Std(const std::vector<Real>& input) noexcept;

/** Var (unbiased estimator) */
Real Var(const std::vector<Real>& input) noexcept;

/** Weighted var (biased estimator) */
Real Var(const std::vector<Real>& input,
         const std::vector<Real>& weights) noexcept;

/** Splits a string using a delimiter. */
std::vector<std::string> Split(const std::string& string, char delim) noexcept;

/** Converts roots to polynomial. Equivalent to Matlab's poly(roots) */
std::vector<Complex> Poly(const std::vector<Complex> roots) noexcept;
std::vector<Complex> Poly(const std::vector<Real> roots) noexcept;

/** Returns true if all elements are non negative */
bool IsNonNegative(const std::vector<Real>& input) noexcept;

/** Test function for the functions in this file */
bool VectorOpTest();

Matrix<Real> Cov(const std::vector<Real>& x,
                 const std::vector<Real>& y) noexcept;

Matrix<Real> Cov(const std::vector<std::vector<Real> >& input) noexcept;

Real CovElement(const std::vector<Real>& x,
                const std::vector<Real>& y) noexcept;

/**
 Returns a vector containing the cumulative sum of
 the elements of X. Equivalent to Matlab's cumsum(input)
 */
std::vector<Real> CumSum(const std::vector<Real>& input) noexcept;

/** Splits signal up into (overlapping) frames */
std::vector<std::vector<Real> > Enframe(const std::vector<Real>& input,
                                        const std::vector<Real>& window,
                                        const Int frame_increment) noexcept;

std::vector<Real> OverlapAdd(const std::vector<std::vector<Real> >& frames,
                             const std::vector<Real>& window,
                             const Int frame_increment) noexcept;

std::vector<Complex> ConvertToComplex(std::vector<Real> input) noexcept;

///** Filters all signals in a vector of signals through a given filter. Prior
///to every signal, the filter state is reset, and it is also resetted at the
///end. */
void ProcessAll(Filter& filter,
                const std::vector<std::vector<Real> >& input_signals,
                std::vector<std::vector<Real> >& output_signals);
//
///** Filters all signals in a vector of vectors signals through a given filter.
///Prior to every signal, the filter state is reset, and it is also resetted at
///the end. */
void FilterAll(
    Filter& filter,
    std::vector<std::vector<std::vector<Real> > >& matrix_of_input_signals,
    std::vector<std::vector<std::vector<Real> > >& matrix_of_output_signals);

} // namespace dsp

} // namespace sal

#endif
