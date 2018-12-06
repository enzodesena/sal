/*
 array.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 This file contains several types, macros and supporting classes definitions.
 
 */

#ifndef SAL_ARRAY_H
#define SAL_ARRAY_H

#include "salconstants.hpp"

namespace sal
{
template<typename T, std::size_t N>
class Array
{
public:
  T elems[N];


  T& operator[](
    std::size_t i)
  {
    ASSERT(i < N);
    return elems[i];
  }
};
} // namespace sal

#endif
