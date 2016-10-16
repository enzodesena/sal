/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_SOURCE_H
#define SAL_SOURCE_H


#include "saltypes.h"
#include "point.h"
#include "stream.h"
#include <vector>
#include "salconstants.h"

namespace sal {

class SAL_API Source {
public:
  Source(const mcl::Point);
  
  Source(const mcl::Point, const Signal signal);
  
  mcl::Point position() const;
  void set_position(const mcl::Point);
  
  MonoStream* stream() { return &stream_; }
  
private:
  
  MonoStream stream_;
  mcl::Point position_;
};

} // namespace sal

#endif
