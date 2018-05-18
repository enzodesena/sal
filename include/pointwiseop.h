/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_POINTWISE_H
#define MCL_POINTWISE_H

#include <cassert>
#include "mcltypes.h"
#include <vector>
#include <limits>

namespace mcl {

/**
 Returns the point by point addition of the two vectors.
 Equivalent to Matlab's vector_a+vector_b.
 */
template<class T> 
std::vector<T> Add(const std::vector<T>& vector_a,
                           const std::vector<T>& vector_b) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size()) { ASSERT(false); }
  
  std::vector<T> output((Int)vector_a.size());
  for (Int i=0; i<(Int)vector_a.size(); ++i) {
    output[i] = vector_a[i]+vector_b[i];
  }
  return output;
}
  
void Add(const Real* input_data_a,
                 const Real* input_data_b,
                 Int num_samples,
                 Real* output_data) noexcept;


template<>
inline std::vector<Real> Add(const std::vector<Real>& vector_a,
                                     const std::vector<Real>& vector_b) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size()) { ASSERT(false); }
  
  std::vector<Real> output((Int)vector_a.size());
  Add(vector_a.data(), vector_b.data(), (Int)vector_a.size(),
      output.data());
  return output;
}
  
/** Returns the opposite vector.Equivalent to Matlab's -vector. */
template<class T> 
std::vector<T> Opposite(const std::vector<T>& vector) noexcept {
  // Checking we are not dealing with unsigned types.
  // The assert below responds false to complex. TODO: fix this
  //ASSERT(std::numeric_limits<T>::is_signed);
  
  std::vector<T> output(vector.size());
  for (Int i=0; i<(Int)vector.size(); ++i) { output[i] = -vector[i]; }
  return output;
}
  
  
/** Returns the inverse vector.Equivalent to Matlab's 1./vector. */
std::vector<Real> Inverse(const std::vector<Real>& vector) noexcept;

/** 
 Returns the point by point subtraction of the two vectors.
 Equivalent to Matlab's vector_a-vector_b.
 */
template<class T> 
std::vector<T> Subtract(const std::vector<T>& vector_a,
                                const std::vector<T>& vector_b) noexcept {
  return Add(vector_a, Opposite(vector_b));
}


/** 
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template<class T> 
std::vector<T> Multiply(const std::vector<T>& vector_a,
                                const std::vector<T>& vector_b) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size()) { ASSERT(false); }
  
  std::vector<T> output((Int)vector_a.size());
  for (Int i=0; i<(Int)vector_a.size(); ++i) {
    output[i] = vector_a[i]*vector_b[i];
  }
  return output;
}

void Multiply(const Real* input_data_a,
                      const Real* input_data_b,
                      Int num_samples,
                      Real* output_data) noexcept;
  
  
template<>
inline std::vector<Real> Multiply(const std::vector<Real>& vector_a,
                                          const std::vector<Real>& vector_b) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size()) { ASSERT(false); }
  
  std::vector<Real> output((Int)vector_a.size());
  Multiply(vector_a.data(), vector_b.data(), (Int)vector_a.size(),
           output.data());
  return output;
}
  
/** 
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template<class T>
std::vector<T> Divide(const std::vector<T>& vector_a,
                              const std::vector<T>& vector_b) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size()) { ASSERT(false); }
  std::vector<T> output((Int)vector_a.size());
  for (Int i=0; i<(Int)vector_a.size(); ++i) {
    output[i] = vector_a[i]/vector_b[i];
  }
  return output;
}


/** Equivalent to Matlab's exp(vector). */
template<class T>
std::vector<T> Exp(const std::vector<T>& vector) noexcept {
  Int n(vector.size());
  std::vector<T> output(vector.size());
  for (Int i=0; i<n; ++i) { output[i] = exp(vector[i]); }
  return output;
}
  
  
  
/** 
 Returns the vector with conjugate entries.
 Equivalent to Matlab's conj(vector).
 */
std::vector<Complex> Conj(const std::vector<Complex>& vector) noexcept;

/** Transform real vector into complex vector with null imaginary part */
std::vector<Complex>
ComplexVector(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's real(input). */
std::vector<Real> RealPart(const std::vector<Complex>& input) noexcept;

/** Equivalent to Matlab's imag(input). */
std::vector<Real> Imag(const std::vector<Complex>& input) noexcept;


/** 
 Returns the point-wise poser to exponent.
 Equivalent to Matlab's vector.^exponent
 */
std::vector<Real> Pow(const std::vector<Real>& vector,
                              Real exponent) noexcept;


/** Equivalent to Matlab's abs(vector) */
std::vector<Real> Abs(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's abs(vector) */
std::vector<Real> Abs(const std::vector<Complex>& input) noexcept;

/** Equivalent to Matlab's vector.*(vector>0) */
std::vector<Real> HalfWave(const std::vector<Real>& vector) noexcept;

/** Equivalent to Matlab's cos(vector) */
std::vector<Real> Cos(const std::vector<Real>& vector) noexcept;

/** Equivalent to Matlab's sin(vector) */
std::vector<Real> Sin(const std::vector<Real>& vector) noexcept;
  
/** 
 Returns the natural logarithm of the elements of vector.
 Equivalent to Matlab's log(vector).
 */
std::vector<Real> Log(const std::vector<Real>& vector) noexcept;
  
/**
 Returns the 10-base logarithm of the elements of vector.
 Equivalent to Matlab's log10(vector).
 */
std::vector<Real> Log10(const std::vector<Real>& vector) noexcept;
  
std::vector<Int> ConvertToInt(const std::vector<UInt>& vector) noexcept;
  
bool PointWiseOpTest();
  
} /**< namespace mcl */

#endif
