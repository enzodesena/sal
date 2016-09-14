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

// This class represents an acoustic source. It plays back a signal and is
// located in a given position.
// TODO: Add source directivity.
class Source {
public:
  Source(const mcl::Point& position) : position_(position),
  stream_(new MonoStream()), internal_stream_(true) {}
  
  Source(const mcl::Point position, const Signal signal) :
    Source(position) {
    stream_->Push(signal);
  }
  
  Source(const mcl::Point& position, MonoStream* const stream) :
  position_(position), stream_(stream), internal_stream_(false) {}
  
  mcl::Point position() const { return position_; }
  void set_position(const mcl::Point& position) { position_ = position; }
  
  MonoStream* stream() { return stream_; }

  static Source WavRead(mcl::Point point, const std::string file_name) {
    std::vector<Signal> audio = WavHandler::Read(file_name);
    Source output(point);
    output.stream()->Push(audio[0]);
    return output;
  }
  
  ~Source() {
    if (internal_stream_) { delete stream_; }
  }
  
private:
  
  MonoStream* stream_;
  bool internal_stream_;
  mcl::Point position_;
};

} // namespace sal

#endif
