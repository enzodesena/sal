/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_POINTWISE_H
#define MCL_POINTWISE_H

#include <cassert>
#include <limits>
#include <span>
#include <vector>

#include "mcltypes.h"

namespace mcl {

/**
 Returns the point by point addition of the two vectors.
 Equivalent to Matlab's vector_a+vector_b.
 */
template <class T>
std::vector<T> Add(const std::vector<T>& vector_a,
                   const std::vector<T>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());

  std::vector<T> output((Int)vector_a.size());
  for (Int i = 0; i < (Int)vector_a.size(); ++i) {
    output[i] = vector_a[i] + vector_b[i];
  }
  return output;
}

void Add(std::span<const Real> input_a, std::span<const Real> input_b,
         std::span<Real> output) noexcept;

template <>
inline std::vector<Real> Add(const std::vector<Real>& vector_a,
                             const std::vector<Real>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());

  std::vector<Real> output((Int)vector_a.size());
  Add(vector_a, vector_b, output);
  return output;
}

/** Returns the opposite vector.Equivalent to Matlab's -vector. */
template <class T>
std::vector<T> Opposite(const std::vector<T>& vector) noexcept {
  // Checking we are not dealing with unsigned types.
  // The assert below responds false to complex. TODO: fix this
  // ASSERT(std::numeric_limits<T>::is_signed);

  std::vector<T> output(vector.size());
  for (Int i = 0; i < (Int)vector.size(); ++i) {
    output[i] = -vector[i];
  }
  return output;
}

/** Returns the inverse vector.Equivalent to Matlab's 1./vector. */
std::vector<Real> Inverse(const std::vector<Real>& vector) noexcept;

/**
 Returns the point by point subtraction of the two vectors.
 Equivalent to Matlab's vector_a-vector_b.
 */
template <class T>
std::vector<T> Subtract(const std::vector<T>& vector_a,
                        const std::vector<T>& vector_b) noexcept {
  return Add(vector_a, Opposite(vector_b));
}

/**
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template <class T>
std::vector<T> Multiply(const std::vector<T>& vector_a,
                        const std::vector<T>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());

  std::vector<T> output((Int)vector_a.size());
  for (Int i = 0; i < (Int)vector_a.size(); ++i) {
    output[i] = vector_a[i] * vector_b[i];
  }
  return output;
}

void Multiply(std::span<const Real> input_a, std::span<const Real> input_b,
              std::span<Real> output) noexcept;

template <>
inline std::vector<Real> Multiply(const std::vector<Real>& vector_a,
                                  const std::vector<Real>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());

  std::vector<Real> output((Int)vector_a.size());
  Multiply(vector_a, vector_b, output);
  return output;
}

/**
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template <class T>
std::vector<T> Divide(const std::vector<T>& vector_a,
                      const std::vector<T>& vector_b) noexcept {
  ASSERT(vector_a.size() == vector_b.size());
  std::vector<T> output((Int)vector_a.size());
  for (Int i = 0; i < (Int)vector_a.size(); ++i) {
    output[i] = vector_a[i] / vector_b[i];
  }
  return output;
}

/** Equivalent to Matlab's exp(vector). */
template <class T>
std::vector<T> Exp(const std::vector<T>& vector) noexcept {
  Int n(vector.size());
  std::vector<T> output(vector.size());
  for (Int i = 0; i < n; ++i) {
    output[i] = exp(vector[i]);
  }
  return output;
}

/**
 Returns the vector with conjugate entries.
 Equivalent to Matlab's conj(vector).
 */
std::vector<Complex> Conj(const std::vector<Complex>& vector) noexcept;

/** Transform real vector into complex vector with null imaginary part */
std::vector<Complex> ComplexVector(const std::vector<Real>& input) noexcept;

/** Equivalent to Matlab's real(input). */
std::vector<Real> RealPart(const std::vector<Complex>& input) noexcept;

/** Equivalent to Matlab's imag(input). */
std::vector<Real> Imag(const std::vector<Complex>& input) noexcept;

/**
 Returns the point-wise poser to exponent.
 Equivalent to Matlab's vector.^exponent
 */
std::vector<Real> Pow(const std::vector<Real>& vector,
                      const Real exponent) noexcept;

std::vector<Real> Pow(const Real exponent,
                      const std::vector<Real>& vector) noexcept;

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

}  // namespace mcl

#endif
