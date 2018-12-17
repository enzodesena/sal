/*
 saltypes.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 This file contains several types, macros and supporting classes definitions.
 
 */

#ifndef SAL_TYPES_H
#define SAL_TYPES_H

#include "vector.hpp"
#include "point.hpp"
#include "quaternion.hpp"
#include "mcltypes.hpp"

namespace sal
{
typedef mcl::Real Sample;
typedef mcl::Real Time;
typedef mcl::Real Speed;
typedef mcl::Real Length;
typedef mcl::Real Angle;
typedef mcl::Int UInt;
typedef mcl::Int Int;

template<class T>
using Signal = mcl::Vector<T>;


template<class T>
struct StereoSignal
{
  Signal<T> left;
  Signal<T> right;
};

template<typename T, std::size_t N>
class Array
{
public:
  T elems[N];

  T& operator[](
    std::size_t i) noexcept
  {
    ASSERT(i < N);
    return elems[i];
  }
  
  const T& operator[](
    std::size_t i) const noexcept
  {
    ASSERT(i < N);
    return elems[i];
  }
};

typedef mcl::Point<Length> Point;
typedef mcl::Quaternion<Length> Quaternion;


enum class InterpolationType
{
  kRounding,
  /** Rounds the latency to the nearest integer. */
  kLinear,
  /** Applies fractional delays with linear interpolation.
             It reduces audible clicks, but can cause low-pass
             effect. */
};
} // namespace sal

#endif
