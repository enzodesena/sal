/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_SOURCE_H
#define SAL_SOURCE_H

#include <vector>

#include "point.h"
#include "salconstants.h"
#include "saltypes.h"
#include "salutilities.h"

namespace sal {

class Source {
 public:
  Source(const mcl::Point&) noexcept;

  mcl::Point position() const noexcept;

  void SetPosition(const mcl::Point& position) noexcept;

 private:
  Triplet position_;
};

}  // namespace sal

#endif
