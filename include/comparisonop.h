/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */


#ifndef MCL_EQUALITYOP_H
#define MCL_EQUALITYOP_H

#ifndef VERY_SMALL
  #define VERY_SMALL (0.0001f)
#endif

#include "mcltypes.h"
#include "quaternion.h"
#include <vector>

namespace mcl {


bool IsEqual(Real num_a, Real num_b, Real precision = VERY_SMALL);

bool IsEqual(Complex num_a, Complex num_b, Real precision = VERY_SMALL);

bool IsSmallerOrEqual(const Real num_a, const Real num_b,
                      const Real precision = VERY_SMALL);

bool IsLargerOrEqual(const Real num_a, const Real num_b,
                     const Real precision = VERY_SMALL);

bool AreAllSmallerOrEqual(const std::vector<Real>& vector_a,
                          const std::vector<Real>& vector_b);

template<class T>
bool IsEqual(const std::vector<T>& vector_a, const std::vector<T>& vector_b,
             Real precision = VERY_SMALL) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size())
    return false;
  
  for (Int i=0; i<(Int)(Int)vector_a.size(); ++i) {
    if (! IsEqual(vector_a[i], vector_b[i], precision))
      return false;
  }
  return true;
}


bool IsEqual(const std::vector<Int>& vector_a, const std::vector<Int>& vector_b);

bool IsEqual(const Quaternion& quaternion_a, const Quaternion& quaternion_b);



bool IsEqual(const Point& point_a, const Point& point_b,
             const Real precision = VERY_SMALL);

bool IsEqual(std::vector<Point> points_a, std::vector<Point> points_b);

bool IsEqual(const Real* input_data_a, const Real* input_data_b,
             const Int num_samples, Real precision = VERY_SMALL);

bool IsEqual(const Real* input_data_a, const std::vector<Real> input_data_b,
             Real precision = VERY_SMALL);

bool IsEqual(const std::vector<Real> input_data_b, const Real* input_data_a,
             Real precision = VERY_SMALL);
  
/** Returns true if num is nan */
bool IsNan(Real num);

/** Returns true if num is nan */
std::vector<bool> IsNan(std::vector<Real> input);

/** Returns opposite bool as input */
std::vector<bool> Not(std::vector<bool> input);

/** Returns true if all bools are true */
bool All(std::vector<bool> input);

/** Returns true if any one of the bools is true */
bool Any(std::vector<bool> input);

/** Opposite of All: returns true if none of the inputs are true */
bool None(std::vector<bool> input);

/** Returns true if num is +inf or -inf */
bool IsInf(Real num);

/** Returns true if num is +inf or -inf */
std::vector<bool> IsInf(std::vector<Real> input);

bool ComparisonOpTest();
  
  
  
} // namespace mcl



#endif
