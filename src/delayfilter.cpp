/*
 delayfilter.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "delayfilter.h"
#include "exception.h"
#include <cassert>

using sal::Sample;
using sal::UInt;

namespace sal {

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
  latency_ = copy.latency_;
  
  start_ = new Sample[max_latency_+1];
  end_ = start_+max_latency_;
  
  write_index_ = start_+(copy.write_index_-copy.start_);
  read_index_ = start_+(copy.read_index_-copy.start_);
  
  for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = copy.start_[i]; }
}

DelayFilter& DelayFilter::operator= (const DelayFilter& other) {
  if (this != &other) {
    delete[] start_;
    
    max_latency_ = other.max_latency_;
    latency_ = other.latency_;
    
    start_ = new Sample[max_latency_+1];
    end_ = start_+max_latency_;
    
    write_index_ = start_+(other.write_index_-other.start_);
    read_index_ = start_+(other.read_index_-other.start_);
    
    for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = other.start_[i]; }
  }
  return *this;
}

void DelayFilter::set_latency(const UInt latency) {
  latency_ = latency;
  if (latency > max_latency_) { throw_line(""); }
  
  read_index_ = write_index_ - latency;
  
  if (read_index_ < start_) { read_index_ += max_latency_ + 1; }
  
  assert((read_index_ >= start_) & (read_index_ <= end_));
}

UInt DelayFilter::latency() const { return latency_; }

UInt DelayFilter::max_latency() const { return max_latency_; }

Sample DelayFilter::Read(const UInt& delay_tap) const {
  assert(delay_tap < max_latency_);
  assert(write_index_>=start_);
  assert(write_index_<=end_);
  return (write_index_ - delay_tap >= start_) ?
  *(write_index_ - delay_tap) :
  *(write_index_ - delay_tap + max_latency_ + 1);
}

Sample DelayFilter::FractionalRead(const Time fractional_delay_tap) const {
  UInt x_a = (UInt) floor(fractional_delay_tap);
  UInt x_b = x_a+1;
  Sample f_x_a = Read(x_a);
  Sample f_x_b = Read(x_b);
  return (f_x_b-f_x_a)/(x_b-x_a)*(fractional_delay_tap-x_a)+f_x_a;
}
  
void DelayFilter::Reset() {
  for (UInt i=0; i<(max_latency_+1); ++i) { start_[i] = 0.0; }
}
  
mcl::Real DelayFilter::Filter(const mcl::Real input) {
  Write(input);
  mcl::Real output = Read();
  Tick();
  return output;
}
  
} // namespace sal
