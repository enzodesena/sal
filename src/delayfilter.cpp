/*
 delayfilter.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "delayfilter.h"
#include "salutilities.h"
#include <cassert>

using sal::Sample;
using sal::Int;

namespace sal {

DelayFilter::DelayFilter(Int latency, Int max_latency) noexcept : latency_(-1) {
  ASSERT_WITH_MESSAGE(latency >= 0, "The latency cannot be nagative.");
  ASSERT_WITH_MESSAGE(max_latency >= 0,
                      "The maximum latency cannot be nagative.");
  
  max_latency_ = max_latency;
  start_ = new Sample[max_latency+1];
  end_ = start_+max_latency;
  
  write_index_ = start_;
  this->set_latency(latency);
  for (Int i=0; i<(max_latency+1); ++i) { start_[i] = 0.0; }
}

DelayFilter::DelayFilter(const DelayFilter& copy) {
  max_latency_ = copy.max_latency_;
  latency_ = copy.latency_;
  
  start_ = new Sample[max_latency_+1];
  end_ = start_+max_latency_;
  
  write_index_ = start_+(copy.write_index_-copy.start_);
  read_index_ = start_+(copy.read_index_-copy.start_);
  
  for (Int i=0; i<(max_latency_+1); ++i) { start_[i] = copy.start_[i]; }
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
    
    for (Int i=0; i<(max_latency_+1); ++i) { start_[i] = other.start_[i]; }
  }
  return *this;
}
  
void DelayFilter::Tick(const Int num_samples) noexcept {
  ASSERT(num_samples >= 0);
  if (num_samples > latency_) {
    LogError("Ticking by more samples (%d) than the latency of the delay "
             "line (%d). The operation will go ahead, but this implies that "
             "some samples may never be read.",
             num_samples, latency_);
  }
  
  Int wrapped_num_samples = num_samples % max_latency_;
  ASSERT(wrapped_num_samples >= 0 && wrapped_num_samples < max_latency_);
  if (write_index_+wrapped_num_samples <= end_) {
    write_index_ += wrapped_num_samples;
  } else {
    write_index_ = start_ + (wrapped_num_samples-(end_-write_index_)-1);
  }
  if (read_index_+wrapped_num_samples <= end_) {
    read_index_ += wrapped_num_samples;
  } else {
    read_index_ = start_ + (wrapped_num_samples-(end_-read_index_)-1);
  }
  ASSERT(write_index_ >= start_ && write_index_ <= end_);
  ASSERT(read_index_ >= start_ && read_index_ <= end_);
}
  
void DelayFilter::Write(const Sample* samples, const Int num_samples) noexcept {
  ASSERT(num_samples >= 0);
  if (num_samples > (max_latency_-latency_+1)) {
    LogError("Writing more samples (%d) than max_latency-latency+1 (%d)."
             "This operation will go ahead, but some samples will be "
             "overwritten. ",
             num_samples, max_latency_-latency_+1);
  }
  
  Int k = 0;
  for (Int i=0; i<num_samples; ++i) {
    ASSERT((write_index_+k) >= start_ && (write_index_+k) <= end_);
    *(write_index_+k++) = samples[i];
    if (write_index_+k > end_) {
      // Make it such that for the next loop, write_index_+k == start_, i.e.
      // k becomes negative
      k = start_-write_index_;
    }
  }
}
  
  
void DelayFilter::set_latency(const Int latency) noexcept {
  if (latency_ == latency) { return; }
  latency_ = latency;
  ASSERT_WITH_MESSAGE(latency <= max_latency_,
                      "Trying to set a delay filter latency larger than "
                      "the maximum latency.");
  
  read_index_ = write_index_ - latency;
  
  if (read_index_ < start_) { read_index_ += max_latency_ + 1; }
  
  ASSERT((read_index_ >= start_) & (read_index_ <= end_));
}

Int DelayFilter::latency() const noexcept { return latency_; }

Int DelayFilter::max_latency() const noexcept { return max_latency_; }

Sample DelayFilter::ReadAt(const Int delay_tap) const noexcept {
  ASSERT_WITH_MESSAGE(delay_tap < max_latency_,
                      "Tried to access a delay tap larger than delay filter "
                      "length.");
  
  ASSERT(write_index_>=start_);
  ASSERT(write_index_<=end_);
  Sample* read_index = write_index_ - delay_tap;
  return (read_index >= start_) ? *read_index : *(read_index + max_latency_ + 1);
}
  
void DelayFilter::Read(const Int num_samples,
                       Sample* output_data) const noexcept {
  if (num_samples > latency_) {
    LogError("Trying to read a number of samples (%d) larger than the latency "
             "of the delay line (%d). This operation will go ahead, but it "
             "means you will be reading samples that haven't been written yet.",
             num_samples, latency_);
  }
  
  Int k = 0;
  for (Int i=0; i<num_samples; ++i) {
    ASSERT((read_index_+k) >= start_ && (read_index_+k) <= end_);
    output_data[i] = *(read_index_+k++);
    if (read_index_+k > end_) {
      // Make it such that for the next loop, read_index_+k == start_, i.e.
      // k becomes negative
      k = start_-read_index_;
    }
  }
}

Sample DelayFilter::FractionalReadAt(const Time fractional_delay_tap)
      const noexcept {
  ASSERT_WITH_MESSAGE(fractional_delay_tap<max_latency_,
                      "Tried to access a delay tap larger than delay filter"
                      "length.");
  
  Int x_a = (UInt) floor(fractional_delay_tap);
  Int x_b = x_a+1;
  Sample f_x_a = ReadAt(x_a);
  Sample f_x_b = ReadAt(x_b);
  return (f_x_b-f_x_a)/(x_b-x_a)*(fractional_delay_tap-x_a)+f_x_a;
}
  
void DelayFilter::Reset() noexcept {
  for (Int i=0; i<(max_latency_+1); ++i) { start_[i] = 0.0; }
}
  
mcl::Real DelayFilter::Filter(const mcl::Real input) noexcept {
  Write(input);
  mcl::Real output = Read();
  Tick();
  return output;
}
  
} // namespace sal
