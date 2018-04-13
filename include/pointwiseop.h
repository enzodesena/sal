/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef MCL_POINTWISE_H
#define MCL_POINTWISE_H

#include <cassert>
#include "mcltypes.h"
#include <vector>
#include <limits>

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {

/**
 Returns the point by point addition of the two vectors.
 Equivalent to Matlab's vector_a+vector_b.
 */
template<class T> 
MCL_API std::vector<T> Add(const std::vector<T>& vector_a,
                           const std::vector<T>& vector_b) noexcept {
  if (vector_a.size() != vector_b.size()) { assert(false); }
  
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]+vector_b[i];
  }
  return output;
}


/** Returns the opposite vector.Equivalent to Matlab's -vector. */
template<class T> 
MCL_API std::vector<T> Opposite(const std::vector<T>& vector) noexcept {
  // Checking we are not dealing with unsigned types.
  // The assert below responds false to complex. TODO: fix this
  //assert(std::numeric_limits<T>::is_signed);
  
  std::vector<T> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) { output[i] = -vector[i]; }
  return output;
}
  
  
/** Returns the inverse vector.Equivalent to Matlab's 1./vector. */
MCL_API std::vector<Real> Inverse(const std::vector<Real>& vector) noexcept;

/** 
 Returns the point by point subtraction of the two vectors.
 Equivalent to Matlab's vector_a-vector_b.
 */
template<class T> 
MCL_API std::vector<T> Subtract(const std::vector<T>& vector_a,
                                const std::vector<T>& vector_b) noexcept {
  return Add(vector_a, Opposite(vector_b));
}


/** 
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template<class T> 
MCL_API std::vector<T> Multiply(const std::vector<T>& vector_a,
                                const std::vector<T>& vector_b) noexcept {
  if (vector_a.size() != vector_b.size()) { assert(false); }
  
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]*vector_b[i];
  }
  return output;
}
  
/** 
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template<class T>
MCL_API std::vector<T> Divide(const std::vector<T>& vector_a,
                              const std::vector<T>& vector_b) noexcept {
  if (vector_a.size() != vector_b.size()) { assert(false); }
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]/vector_b[i];
  }
  return output;
}


/** Equivalent to Matlab's exp(vector). */
template<class T>
MCL_API std::vector<T> Exp(const std::vector<T>& vector) noexcept {
  UInt n(vector.size());
  std::vector<T> output(vector.size());
  for (UInt i=0; i<n; ++i) { output[i] = exp(vector[i]); }
  return output;
}
  
  
  
/** 
 Returns the vector with conjugate entries.
 Equivalent to Matlab's conj(vector).
 */
MCL_API std::vector<Complex> Conj(const std::vector<Complex>& vector) noexcept;

/** Transform real vector into complex vector with null imaginary part */
MCL_API std::vector<Complex>
ComplexVector(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's real(input). */
MCL_API std::vector<Real> RealPart(const std::vector<Complex>& input) noexcept;

/** Equivalent to Matlab's imag(input). */
MCL_API std::vector<Real> Imag(const std::vector<Complex>& input) noexcept;


/** 
 Returns the point-wise poser to exponent.
 Equivalent to Matlab's vector.^exponent
 */
MCL_API std::vector<Real> Pow(const std::vector<Real>& vector,
                              Real exponent) noexcept;


/** Equivalent to Matlab's abs(vector) */
MCL_API std::vector<Real> Abs(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's abs(vector) */
MCL_API std::vector<Real> Abs(const std::vector<Complex>& input) noexcept;

/** Equivalent to Matlab's vector.*(vector>0) */
MCL_API std::vector<Real> HalfWave(const std::vector<Real>& vector) noexcept;

/** Equivalent to Matlab's cos(vector) */
MCL_API std::vector<Real> Cos(const std::vector<Real>& vector) noexcept;

/** Equivalent to Matlab's sin(vector) */
MCL_API std::vector<Real> Sin(const std::vector<Real>& vector) noexcept;
  
/** 
 Returns the natural logarithm of the elements of vector.
 Equivalent to Matlab's log(vector).
 */
MCL_API std::vector<Real> Log(const std::vector<Real>& vector) noexcept;
  
/**
 Returns the 10-base logarithm of the elements of vector.
 Equivalent to Matlab's log10(vector).
 */
MCL_API std::vector<Real> Log10(const std::vector<Real>& vector) noexcept;
  
MCL_API std::vector<Int> ConvertToInt(const std::vector<UInt>& vector) noexcept;
  
bool PointWiseOpTest();
  
} /**< namespace mcl */

#endif
