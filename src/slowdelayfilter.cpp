/*
 slowdelayfilter.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "slowdelayfilter.h"
#include "saltypes.h"
#include <cassert>
#include <iostream>
#include <tgmath.h>

using sal::Sample;
using sal::UInt;

namespace sal {

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
  
Sample SlowDelayFilter::FractionalRead(const Time fractional_delay_tap) const {
  sal::Time x_a = floor(fractional_delay_tap);
  sal::Time x_b = x_a+1.0;
  Sample f_x_a = Read(x_a);
  Sample f_x_b = Read(x_b);
  return (f_x_b-f_x_a)/(x_b-x_a)*(fractional_delay_tap-x_a)+f_x_a;
}
  
Sample SlowDelayFilter::Read(const UInt& delay_tap) const {
  assert(delay_tap < max_latency_);
  assert(write_index_>=start_);
  assert(write_index_<=end_);
  return (write_index_ - delay_tap >= start_) ?
  *(write_index_ - delay_tap) :
  *(write_index_ - delay_tap + max_latency_ + 1);
}

void SlowDelayFilter::set_latency(const UInt latency,
                                  const UInt update_step) {
  update_step_ = update_step;
  target_latency_ = latency;
  chasing_latency_index_ = 0;
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
  using sal::Int;
  using sal::UInt;
  
  if (target_latency_ != latency_) {
    if (++chasing_latency_index_ >= update_step_) {
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




} // namespace sal
