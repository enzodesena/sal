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
  Source(const Point);
  
  Source(const Point, const Signal signal);
  
  Point position() const;
  void set_position(const Point);
  
  MonoStream* stream() { return &stream_; }
  
  static Source WavRead(Point point, const std::string file_name) {
    std::vector<Signal> audio = WavHandler::Read(file_name);
    Source output(point);
    output.stream()->Push(audio[0]);
    return output;
  }
  
private:
  
  MonoStream stream_;
  Point position_;
};

} // namespace sal

#endif
