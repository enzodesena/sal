/*
 delayfilter.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 155 $
 Last changed date:  $Date: 2012-10-07 13:43:12 +0100 (Sun, 07 Oct 2012) $
 */

#include "delayfilter.h"
#include <cassert>

using sat::Sample;
using sat::UInt;

namespace sat {

DelayFilter::DelayFilter(UInt latency, UInt max_latency) :
        latency_(latency) {
  UInt integer_latency = latency;
  if (max_latency < integer_latency) { max_latency = integer_latency; }
  max_latency_ = max_latency;
  start_ = new Sample[max_latency+1];
  end_ = start_+max_latency;
  
  write_index_ = start_;
  this->set_latency(latency);
  for (UInt i=0; i<(max_latency+1); ++i) { start_[i] = 0.0; }
}

DelayFilter::DelayFilter(const DelayFilter& copy) {
  max_latency_ = copy.max_latency_;
  start_ = new Sample[max_latency_+1];
  end_ = copy.end_;
  write_index_ = copy.write_index_;
  read_index_ = copy.read_index_;
  latency_ = copy.latency_;
  for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = copy.start_[i]; }
}

DelayFilter& DelayFilter::operator= (const DelayFilter& other) {
  if (this != &other) {
    delete[] start_;
    max_latency_ = other.max_latency_;
    start_ = new Sample[max_latency_+1];
    end_ = other.end_;
    write_index_ = other.write_index_;
    read_index_ = other.read_index_;
    latency_ = other.latency_;
    for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = other.start_[i]; }
  }
  return *this;
}

void DelayFilter::set_latency(const UInt latency) {
  latency_ = latency;
  assert(latency >= 0);
  assert(latency <= max_latency_);
  
  read_index_ = write_index_ - latency;
  
  if (read_index_ < start_) { read_index_ += max_latency_ + 1; }
  
  assert(read_index_ >= start_ & read_index_ <= end_);
}

UInt DelayFilter::latency() const { return latency_; }

UInt DelayFilter::max_latency() const { return max_latency_; }



} // namespace sat
