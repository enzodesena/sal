/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "comparisonop.h"
#include "mcltypes.h"
#include "vectorop.h"
#include "pointwiseop.h"
#include "quaternion.h"
#include <vector>


#if defined(__APPLE__)
#if (__GNUC__ >= 4)
#include <cmath>
#define isnan(x) std::isnan(x)
#else
#include <math.h>
#define isnan(x) __isnand((double)x)
#endif
#endif

namespace mcl {
  

bool IsEqual(Real num_a, Real num_b, Real precision) {
  if (isnan(num_a) || isnan(num_b)) { return false; }
  return ((Real) fabs(((double) num_a) - ((double) num_b))) < precision;
}
  

bool IsNan(Real num) {
  return isnan(num);
}

std::vector<bool> IsNan(std::vector<Real> input) {
  std::vector<bool> output;
  for (Int i=0; i<(Int)input.size(); ++i) {
    output.push_back(IsNan(input[i]));
  }
  return output;
}

bool IsInf(Real num) {
  return isinf(num);
}

std::vector<bool> IsInf(std::vector<Real> input) {
  std::vector<bool> output;
  for (Int i=0; i<(Int)input.size(); ++i) {
    output.push_back(IsInf(input[i]));
  }
  return output;
}

std::vector<bool> Not(std::vector<bool> input) {
  std::vector<bool> output;
  for (Int i=0; i<(Int)input.size(); ++i) {
    output.push_back(!input[i]);
  }
  return output;
}

bool All(std::vector<bool> input) {
  for (Int i=0; i<(Int)input.size(); ++i) {
    if (input[i] == false) {
      return false;
    }
  }
  return true;
}

bool None(std::vector<bool> input) {
  return All(Not(input));
}

bool Any(std::vector<bool> input) {
  for (Int i=0; i<(Int)input.size(); ++i) {
    if (input[i] == true) {
      return true;
    }
  }
  return false;
}

bool IsSmallerOrEqual(const Real num_a, const Real num_b, const Real precision) {
  return num_a <= (num_b + precision);
}

bool IsLargerOrEqual(const Real num_a, const Real num_b, const Real precision) {
  return num_a >= (num_b - precision);
}

bool AreAllSmallerOrEqual(const std::vector<Real>& vector_a,
                          const std::vector<Real>& vector_b) {
  if ((Int)vector_a.size() != (Int)vector_b.size())
    return false;
  
  for (Int i=0; i<(Int)(Int)vector_a.size(); ++i) {
    if (! IsSmallerOrEqual(vector_a[i], vector_b[i])) { return false; }
  }
  
  return true;
}

bool IsEqual(Complex num_a, Complex num_b, Real precision) {
  return (fabs(num_a.real() - num_b.real()) < precision) &
  (fabs(num_a.imag() - num_b.imag()) < precision);
}

bool IsEqual(const Quaternion& q_a, const Quaternion& q_b) {
  return IsEqual(q_a.w(), q_b.w()) & IsEqual(q_a.x(), q_b.x()) &
  IsEqual(q_a.y(), q_b.y()) & IsEqual(q_a.z(), q_b.z());
}



bool IsEqual(const std::vector<Int>& vector_a,
             const std::vector<Int>& vector_b) {
  if ((Int)vector_a.size() != (Int)vector_b.size())
    return false;
  
  for (Int i=0; i<(Int)vector_a.size(); ++i) {
    if (vector_a[i] != vector_b[i]) { return false; }
  }
  return true;
}

bool IsReal(const std::vector<Complex>& input) {
  const Int size = input.size();
  for (Int i=0; i<size; ++i) {
    if (! IsEqual(input[i].imag(), 0.0)) { return false; }
  }
  return true;
}



bool IsEqual(std::vector<Point> points_a, std::vector<Point> points_b) {
  const Int num_points = (Int)points_a.size();
  if (num_points != (Int)points_b.size()) { return false; }
  for (Int i=0; i<num_points; ++i) {
    if (! IsEqual(points_a[i], points_b[i])) { return false; }
  }
  return true;
}


bool IsEqual(const Point& point_a, const Point& point_b,
             const Real precision) {
  return mcl::IsEqual(point_a.x(), point_b.x(), precision) &&
  mcl::IsEqual(point_a.y(), point_b.y(), precision) &&
  mcl::IsEqual(point_a.z(), point_b.z(), precision);
}

  
bool IsEqual(const Real* input_data_a,
             const Real* input_data_b,
             const Int num_samples,
             Real precision) {
  for (Int i=0; i<num_samples; ++i) {
    if (! mcl::IsEqual(input_data_a[i], input_data_b[i], precision)) {
      return false;
    }
  }
  return true;
}
  

bool IsEqual(const Real* input_data_a,
             const std::vector<Real> input_data_b,
             Real precision) {
  return IsEqual(input_data_a, input_data_b.data(), input_data_b.size(),
                 precision);
}

  
bool IsEqual(const std::vector<Real> input_data_b,
             const Real* input_data_a,
             Real precision) {
  return IsEqual(input_data_a, input_data_b.data(), input_data_b.size(),
                 precision);
}
  
} // namespace mcl
