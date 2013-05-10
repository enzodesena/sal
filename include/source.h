/*
 source.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 153 $
 Last changed date:  $Date: 2012-10-06 21:39:09 +0100 (Sat, 06 Oct 2012) $
 */

#ifndef SAT_SOURCE_H
#define SAT_SOURCE_H


#include "sattypes.h"
#include "point.h"
#include "stream.h"
#include "wavhandler.h"
#include <vector>

namespace sat {

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

} // namespace sat

#endif
