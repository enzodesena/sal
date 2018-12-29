/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "saltypes.hpp"
#include "salutilities.hpp"
#include "point.hpp"
#include "vector.hpp"
#include "salconstants.hpp"

namespace sal
{
class Source
{
public:
  Source(
    const Point&) noexcept;

  Point position() const noexcept;

  void SetPosition(
    const Point& position) noexcept;

private:
  Triplet position_;
};

Source::Source(
  const Point& position) noexcept
  : position_(position)
{
}


Point Source::position() const noexcept
{
  return position_;
}


void Source::SetPosition(
  const Point& position) noexcept
{
  position_ = position;
}


} // namespace sal
