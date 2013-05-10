/*
 slowdelayfilter.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 132 $
 Last changed date:  $Date: 2012-09-24 11:00:27 +0100 (Mon, 24 Sep 2012) $
 */

#include "slowdelayfilter.h"
#include "sattypes.h"
#include <cassert>
#include <iostream>

using sat::Sample;
using sat::UInt;

namespace sat {

SlowDelayFilter::SlowDelayFilter(UInt latency, UInt max_latency) :
        latency_(latency),
        chasing_latency_index_(0),
        target_latency_(latency) {
  UInt integer_latency = latency;
  if (max_latency < integer_latency) { max_latency = integer_latency; }
  max_latency_ = max_latency;
  start_ = new Sample[max_latency+1];
  end_ = start_+max_latency;
  
  write_index_ = start_;
  this->set_latency(latency);
  for (UInt i=0; i<(max_latency+1); ++i) { start_[i] = 0.0; }
}

SlowDelayFilter::SlowDelayFilter(const SlowDelayFilter& copy) {
  max_latency_ = copy.max_latency_;
  start_ = new Sample[max_latency_+1];
  end_ = copy.end_;
  write_index_ = copy.write_index_;
  read_index_ = copy.read_index_;
  latency_ = copy.latency_;
  target_latency_ = copy.target_latency_;
  chasing_latency_index_ = copy.chasing_latency_index_;
  for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = copy.start_[i]; }
}
  
SlowDelayFilter& SlowDelayFilter::operator= (const SlowDelayFilter& other) {
  if (this != &other) {
    delete[] start_;
    max_latency_ = other.max_latency_;
    start_ = new Sample[max_latency_+1];
    end_ = other.end_;
    write_index_ = other.write_index_;
    read_index_ = other.read_index_;
    latency_ = other.latency_;
    target_latency_ = other.target_latency_;
    chasing_latency_index_ = other.chasing_latency_index_;
    for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = other.start_[i]; }
  }
  return *this;
}


void SlowDelayFilter::set_latency(const UInt latency) {
  using sat::Int;
  if (abs(((int) latency)-((int)latency_)) <= MAX_LATENCY_JUMP) {
    UpdateLatency(latency);
    target_latency_ = latency;
    chasing_latency_index_ = 0;
  } else {
    target_latency_ = latency;
    chasing_latency_index_ = 0;
  }
}
  
void SlowDelayFilter::UpdateLatency(const UInt latency) {
  latency_ = latency;
  assert(latency >= 0);
  assert(latency <= max_latency_);
  
  read_index_ = write_index_ - latency;
  
  if (read_index_ < start_) { read_index_ += max_latency_ + 1; }
  
  assert(read_index_ >= start_ & read_index_ <= end_);
}

void SlowDelayFilter::Tick() {
  using sat::Int;
  using sat::UInt;
  
  if (target_latency_ != latency_) {
    if (++chasing_latency_index_ >= UPDATE_STEP) {
      const Int latency_step =
      ((((Int) target_latency_)-((Int) latency_)) > 0) ? 1 : -1;
      const Int new_latency = ((Int) latency_) + latency_step;
      
      UpdateLatency((UInt) new_latency);
      chasing_latency_index_ = 0;
    }
    
  }
  
  write_index_ = (write_index_ != end_) ? (write_index_+1) : start_;
  read_index_ = (read_index_ != end_) ? (read_index_+1) : start_;
}




} // namespace sat
