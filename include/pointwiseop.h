/*
 pointwise.h
 MCL
 
 This contains vector point-wise operations
 
 Authors: Enzo De Sena, enzodesena@me.com
 
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
                   const std::vector<T>& vector_b) {
  assert(vector_a.size() == vector_b.size());
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]+vector_b[i];
  }
  return output;
}


/** Returns the opposite vector.Equivalent to Matlab's -vector. */
template<class T> 
std::vector<T> Opposite(const std::vector<T>& vector) {
  // Checking we are not dealing with unsigned types.
  // The assert below responds false to complex. TODO: fix this
  //assert(std::numeric_limits<T>::is_signed);
  
  std::vector<T> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) { output[i] = -vector[i]; }
  return output;
}
  
  
/** Returns the inverse vector.Equivalent to Matlab's 1./vector. */
std::vector<Real> Inverse(const std::vector<Real>& vector);

/** 
 Returns the point by point subtraction of the two vectors.
 Equivalent to Matlab's vector_a-vector_b.
 */
template<class T> 
std::vector<T> Subtract(const std::vector<T>& vector_a,
                        const std::vector<T>& vector_b) {
  return Add(vector_a, Opposite(vector_b));
}


/** 
 Returns the point by point multiplication of the two vectors.
 Equivalent to Matlab's vector_a.*vector_b.
 */
template<class T> 
std::vector<T> Multiply(const std::vector<T>& vector_a,
                        const std::vector<T>& vector_b) {
  assert(vector_a.size() == vector_b.size());
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
std::vector<T> Divide(const std::vector<T>& vector_a,
                      const std::vector<T>& vector_b) {
  assert(vector_a.size() == vector_b.size());
  std::vector<T> output(vector_a.size());
  for (UInt i=0; i<vector_a.size(); ++i) {
    output[i] = vector_a[i]/vector_b[i];
  }
  return output;
}


/** Equivalent to Matlab's exp(vector). */
template<class T>
std::vector<T> Exp(const std::vector<T>& vector) {
  UInt n(vector.size());
  std::vector<T> output(vector.size());
  for (UInt i=0; i<n; ++i) { output[i] = exp(vector[i]); }
  return output;
}
  
  
  
/** 
 Returns the vector with conjugate entries.
 Equivalent to Matlab's conj(vector).
 */
std::vector<Complex> Conj(const std::vector<Complex>& vector);

/** Transform real vector into complex vector with null imaginary part */
std::vector<Complex> ComplexVector(const std::vector<Real>& input);

/** Equivalent to Matlab's real(input). */
std::vector<Real> RealPart(const std::vector<Complex>& input);

/** Equivalent to Matlab's imag(input). */
std::vector<Real> Imag(const std::vector<Complex>& input);


/** 
 Returns the point-wise poser to exponent.
 Equivalent to Matlab's vector.^exponent
 */
std::vector<Real> Pow(const std::vector<Real>& vector, Real exponent);


/** Equivalent to Matlab's abs(vector) */
std::vector<Real> Abs(const std::vector<Real>& input);

/** Equivalent to Matlab's abs(vector) */
std::vector<Real> Abs(const std::vector<Complex>& input);

/** Equivalent to Matlab's vector.*(vector>0) */
std::vector<Real> HalfWave(const std::vector<Real>& vector);

/** Equivalent to Matlab's cos(vector) */
std::vector<Real> Cos(const std::vector<Real>& vector);

/** Equivalent to Matlab's sin(vector) */
std::vector<Real> Sin(const std::vector<Real>& vector);
  
/** 
 Returns the natural logarithm of the elements of vector.
 Equivalent to Matlab's log(vector).
 */
std::vector<Real> Log(const std::vector<Real>& vector);
  
/**
 Returns the 10-base logarithm of the elements of vector.
 Equivalent to Matlab's log10(vector).
 */
std::vector<Real> Log10(const std::vector<Real>& vector);
  
std::vector<Int> ConvertToInt(const std::vector<UInt>& vector);
  
bool PointWiseOpTest();
  
} /**< namespace mcl */

#endif
