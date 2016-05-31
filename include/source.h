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
#include "wavhandler.h"
#include <vector>

namespace sal {

class Source {
public:
  Source(const mcl::Point);
  
  Source(const mcl::Point, const Signal signal);
  
  mcl::Point position() const;
  void set_position(const mcl::Point);
  
  MonoStream* stream() { return &stream_; }
  
  static Source WavRead(mcl::Point point, const std::string file_name) {
    std::vector<Signal> audio = WavHandler::Read(file_name);
    Source output(point);
    output.stream()->Push(audio[0]);
    return output;
  }
  
private:
  
  MonoStream stream_;
  mcl::Point position_;
};

} // namespace sal

#endif
