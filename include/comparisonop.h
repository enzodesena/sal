/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#ifndef MCL_EQUALITYOP_H
#define MCL_EQUALITYOP_H

#ifndef VERY_SMALL
  #define VERY_SMALL (1.0E-4)
#endif

#include "mcltypes.h"
#include "quaternion.h"
#include <vector>

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {


MCL_API bool IsEqual(Real num_a, Real num_b, Real precision = VERY_SMALL);

MCL_API bool IsEqual(Complex num_a, Complex num_b,
                     Real precision = VERY_SMALL);

MCL_API bool IsSmallerOrEqual(const Real num_a, const Real num_b,
                              const Real precision = VERY_SMALL);

MCL_API bool IsLargerOrEqual(const Real num_a, const Real num_b,
                             const Real precision = VERY_SMALL);

MCL_API bool AreAllSmallerOrEqual(const std::vector<Real>& vector_a,
                          const std::vector<Real>& vector_b);

template<class T>
MCL_API bool IsEqual(const std::vector<T>& vector_a,
                     const std::vector<T>& vector_b,
                     Real precision = VERY_SMALL) noexcept {
  if ((Int)vector_a.size() != (Int)vector_b.size())
    return false;
  
  for (Int i=0; i<(Int)(Int)vector_a.size(); ++i) {
    if (! IsEqual(vector_a[i], vector_b[i], precision))
      return false;
  }
  return true;
}


MCL_API bool IsEqual(const std::vector<Int>& vector_a,
                     const std::vector<Int>& vector_b);

MCL_API bool IsEqual(const Quaternion& quaternion_a,
                     const Quaternion& quaternion_b);



MCL_API bool IsEqual(const Point& point_a, const Point& point_b,
             MCL_API const Real precision = VERY_SMALL);

MCL_API bool IsEqual(std::vector<Point> points_a, std::vector<Point> points_b);

bool IsEqual(const Real* input_data_a,
             const Real* input_data_b,
             const Int num_samples,
             Real precision = VERY_SMALL);

bool IsEqual(const Real* input_data_a,
             const std::vector<Real> input_data_b,
             Real precision = VERY_SMALL);

bool IsEqual(const std::vector<Real> input_data_b,
             const Real* input_data_a,
             Real precision = VERY_SMALL);
  
/** Returns true if num is nan */
MCL_API bool IsNan(Real num);

/** Returns true if num is nan */
MCL_API std::vector<bool> IsNan(std::vector<Real> input);

/** Returns opposite bool as input */
MCL_API std::vector<bool> Not(std::vector<bool> input);

/** Returns true if all bools are true */
MCL_API bool All(std::vector<bool> input);

/** Returns true if any one of the bools is true */
MCL_API bool Any(std::vector<bool> input);

/** Opposite of All: returns true if none of the inputs are true */
MCL_API bool None(std::vector<bool> input);

/** Returns true if num is +inf or -inf */
MCL_API bool IsInf(Real num);

/** Returns true if num is +inf or -inf */
MCL_API std::vector<bool> IsInf(std::vector<Real> input);

MCL_API bool ComparisonOpTest();
  
  
  
} // namespace mcl



#endif
