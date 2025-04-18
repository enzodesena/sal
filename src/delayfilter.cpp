/*
 delayfilter.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <cassert>

#include "delayfilter.h"
#include "salutilities.h"

using sal::dsp::Logger;
using sal::Int;
using sal::Sample;

namespace sal {

DelayFilter::DelayFilter(Int latency, Int max_latency) noexcept : latency_(-1) {
  ASSERT_WITH_MESSAGE(latency >= 0, "The latency cannot be nagative.");
  ASSERT_WITH_MESSAGE(max_latency >= 0,
                      "The maximum latency cannot be nagative.");

  max_latency_ = max_latency;
  start_ = new Sample[max_latency + 1];
  end_ = start_ + max_latency;

  write_index_ = start_;
  this->SetLatency(latency);
  for (Int i = 0; i < (max_latency + 1); ++i) {
    start_[i] = 0.0;
  }
}

DelayFilter::DelayFilter(const DelayFilter& copy) {
  max_latency_ = copy.max_latency_;
  latency_ = copy.latency_;

  start_ = new Sample[max_latency_ + 1];
  end_ = start_ + max_latency_;

  write_index_ = start_ + (copy.write_index_ - copy.start_);
  read_index_ = start_ + (copy.read_index_ - copy.start_);

  for (Int i = 0; i < (max_latency_ + 1); ++i) {
    start_[i] = copy.start_[i];
  }
}

DelayFilter& DelayFilter::operator=(const DelayFilter& other) {
  if (this != &other) {
    delete[] start_;

    max_latency_ = other.max_latency_;
    latency_ = other.latency_;

    start_ = new Sample[max_latency_ + 1];
    end_ = start_ + max_latency_;

    write_index_ = start_ + (other.write_index_ - other.start_);
    read_index_ = start_ + (other.read_index_ - other.start_);

    for (Int i = 0; i < (max_latency_ + 1); ++i) {
      start_[i] = other.start_[i];
    }
  }
  return *this;
}

void DelayFilter::Tick(const Int num_samples) noexcept {
  ASSERT(num_samples >= 0);
  if (num_samples > max_latency_) {
    Logger::GetInstance().LogError(
        "Ticking by more samples (%d) than the max latency of the delay "
        "line (%d). The operation will go ahead, but this implies that "
        "some samples may never be read.",
        num_samples, latency_);
  }

  Int wrapped_num_samples = num_samples % max_latency_;
  ASSERT(wrapped_num_samples >= 0 && wrapped_num_samples < max_latency_);
  if (write_index_ + wrapped_num_samples <= end_) {
    write_index_ += wrapped_num_samples;
  } else {
    write_index_ = start_ + (wrapped_num_samples - (end_ - write_index_) - 1);
  }
  if (read_index_ + wrapped_num_samples <= end_) {
    read_index_ += wrapped_num_samples;
  } else {
    read_index_ = start_ + (wrapped_num_samples - (end_ - read_index_) - 1);
  }
  ASSERT(write_index_ >= start_ && write_index_ <= end_);
  ASSERT(read_index_ >= start_ && read_index_ <= end_);
}

void DelayFilter::Write(std::span<const Sample> input_data) noexcept {
  const size_t num_samples = input_data.size();
  if (num_samples > (max_latency_ - latency_ + 1)) {
    Logger::GetInstance().LogError(
        "Writing more samples (%d) than max_latency-latency+1 (%d)."
        "This operation will go ahead, but some samples will be "
        "overwritten. ",
        num_samples, max_latency_ - latency_ + 1);
  }

  Sample* write_index = write_index_;
  for (size_t i = 0; i < num_samples; ++i) {
    ASSERT(write_index >= start_ && write_index <= end_);
    *(write_index++) = input_data[i];
    if (write_index > end_) {
      write_index = start_;
    }
  }
}

void DelayFilter::SetLatency(const Int latency) noexcept {
  if (latency_ == latency) {
    return;
  }

  if (latency > max_latency_) {
    Logger::GetInstance().LogError(
        "Trying to set a delay filter latency (%d) larger than "
        "the maximum latency (%d). The latency will be set to the "
        "the maximum latency instead. ",
        latency, max_latency_);
  }

  latency_ = std::min(latency, max_latency_);

  read_index_ = write_index_ - latency_;

  if (read_index_ < start_) {
    read_index_ += max_latency_ + 1;
  }

  ASSERT((read_index_ >= start_) & (read_index_ <= end_));
}

Int DelayFilter::latency() const noexcept { return latency_; }

Int DelayFilter::max_latency() const noexcept { return max_latency_; }

void DelayFilter::Read(std::span<Sample> output_data) const noexcept {
  if (output_data.size() > (size_t)max_latency_) {
    Logger::GetInstance().LogError(
        "Trying to read a number of samples (%d) larger than the latency "
        "of the delay line (%d). This operation will go ahead, but it "
        "means you will be reading samples that haven't been written yet.",
        output_data.size(), latency_);
  }
  Sample* read_index = read_index_;
  for (size_t i = 0; i < output_data.size(); ++i) {
    ASSERT(read_index >= start_ && read_index <= end_);
    output_data[i] = *(read_index++);
    if (read_index > end_) {
      read_index = start_;
    }
  }
}

void DelayFilter::ResetState() noexcept {
  for (Int i = 0; i < (max_latency_ + 1); ++i) {
    start_[i] = 0.0;
  }
}

dsp::Real DelayFilter::ProcessSample(const dsp::Real input) noexcept {
  Write(input);
  dsp::Real output = Read();
  Tick();
  return output;
}

}  // namespace sal
