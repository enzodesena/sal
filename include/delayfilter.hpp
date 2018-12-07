/*
 delayfilter.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_DELAYFILTER_H
#define SAL_DELAYFILTER_H

#define DEFAULT_MAX_LATENCY 3276800

#include "saltypes.hpp"
#include "digitalfilter.hpp"
#include "salconstants.hpp"
#include <algorithm>

namespace sal
{
template<typename T>
class DelayFilter : public mcl::DigitalFilterInterface<T>
{
public:
  /**
   Constructs a delay filter object with intial latency given by latency. A maximum
   latency has to be given to allocate the maximum amount of memory of the circular
   memory.
   */
  DelayFilter(
    const Int latency,
    const Int max_latency) noexcept
    : latency_(std::numeric_limits<size_t>::max())
    , data_(max_latency+1, T(0.0))
    , write_index_(0)
  {
    ASSERT_WITH_MESSAGE(latency >= 0, "The latency cannot be nagative.");
    ASSERT_WITH_MESSAGE(
      max_latency >= 0,
      "The maximum latency cannot be nagative.");

    this->SetLatency(latency);
  }


  /**
   This writes the next sample into the filter. If this method is called 2 times
   before the Tick() operation, the former value will be overwritten.
   */
  void Write(
    const T sample) noexcept
  {
    data_[write_index_] = sample;
  }


  void Write(
    const T* samples,
    size_t num_samples) noexcept
  {
    ASSERT(num_samples >= 0);
    if (num_samples > (max_latency() - latency_ + 1))
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Writing more samples (%d) than max_latency-latency+1 (%d)."
          "This operation will go ahead, but some samples will be "
          "overwritten. ",
          num_samples,
          max_latency() - latency_ + 1);
    }

    size_t write_index = write_index_;
    for (size_t i = 0; i < num_samples; ++i)
    {
      ASSERT(write_index >= 0 && write_index <= max_latency());
      data_[write_index++] = samples[i];
      if (write_index > max_latency())
      {
        write_index = 0;
      }
    }
  }

  /** Resets the state of the filter */
  void Reset() noexcept
  {
    mcl::SetToZero(data_);
  }


  /**
   Returns the current sample from the filter. Between two Tick() operation it will
   give always the same output.
   */
  inline T Read() const noexcept
  {
    return data_[read_index_];
  }


  /** This allows to read at a different location from the read pointer. */
  const T& ReadAt(
    const size_t delay_tap) const noexcept
  {
#ifndef NOLOGGING
    if (delay_tap > max_latency())
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Trying to read at a delay tap (%d) larger than the maximum latency "
          "of the delay line (%d). Giving back the value at the maximum "
          "latency instead. ",
          delay_tap,
          max_latency());
    }
#endif

    ASSERT(write_index_ >= 0 && write_index_ <= max_latency());
    size_t sanitised_delay_tap = std::min(delay_tap, max_latency());
    return (write_index_ >= sanitised_delay_tap) ?
             data_[write_index_ - sanitised_delay_tap] :
             data_[write_index_ - sanitised_delay_tap + max_latency() + 1];
  }


  /** Read the next `num_samples` samples.
   @param[in] num_samples the number of samples to be read.
   @param[out] output_data the array where to write these samples. */
  void Read(
    size_t num_samples,
    T* output_data) const noexcept
  {
    if (num_samples > max_latency())
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Trying to read a number of samples (%d) larger than the latency "
          "of the delay line (%d). This operation will go ahead, but it "
          "means you will be reading samples that haven't been written yet.",
          num_samples,
          latency_);
    }

    size_t read_index = read_index_;
    for (size_t i = 0; i < num_samples; ++i)
    {
      ASSERT(read_index >= 0 && read_index <= max_latency());
      output_data[i] = data_[read_index++];
      if (read_index > max_latency())
      {
        read_index = 0;
      }
    }
  }


  T FractionalReadAt(
    const Time fractional_delay_tap) const noexcept
  {
#ifndef NOLOGGING
    if (fractional_delay_tap >= (Time)max_latency())
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Trying to read at a delay tap (%f) larger than the maximum latency "
          "of the delay line (%d). Giving back the value at the maximum "
          "latency instead. ",
          fractional_delay_tap,
          max_latency());
    }
#endif

    Time sanitised_delay_tap = std::min(
      fractional_delay_tap, (Time)max_latency());
    Int x_a = (Int)sanitised_delay_tap; // Cast to int is equivalent to floor
    Int x_b = x_a + 1;
    T f_x_a = ReadAt(x_a);
    T f_x_b = ReadAt(x_b);
    return (f_x_b - f_x_a) / (x_b - x_a) * (sanitised_delay_tap - x_a) + f_x_a;
  }


  /** This causes time to tick by one sample. */
  void Tick() noexcept
  {
    write_index_ = (write_index_ != max_latency()) ? (write_index_ + 1) : 0;
    read_index_ = (read_index_ != max_latency()) ? (read_index_ + 1) : 0;
  }


  /** This causes time to tick by more than one sample (use only if you
   understand what you are doing!). */
  void Tick(
    Int num_samples) noexcept
  {
    ASSERT(num_samples >= 0);
    if (num_samples > max_latency())
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Ticking by more samples (%d) than the max latency of the delay "
          "line (%d). The operation will go ahead, but this implies that "
          "some samples may never be read.",
          num_samples,
          latency_);
    }

    Int wrapped_num_samples = num_samples % max_latency();
    ASSERT(wrapped_num_samples >= 0 && wrapped_num_samples < max_latency());
    if (write_index_ + wrapped_num_samples <= max_latency())
    {
      write_index_ += wrapped_num_samples;
    }
    else
    {
      write_index_ = 0 + (wrapped_num_samples - (max_latency() - write_index_) - 1);
    }
    if (read_index_ + wrapped_num_samples <= max_latency())
    {
      read_index_ += wrapped_num_samples;
    }
    else
    {
      read_index_ = 0 + (wrapped_num_samples - (max_latency() - read_index_) - 1);
    }
    ASSERT(write_index_ >= 0 && write_index_ <= max_latency());
    ASSERT(read_index_ >= 0 && read_index_ <= max_latency());
  }

  /**
   Resets the latency of the filter. This can introduce artifacts if the
   latency is updated too fast.
   */
  void SetLatency(
    size_t latency) noexcept
  {
    if (latency_ == latency)
    {
      return;
    }
    if (latency > max_latency())
    {
      mcl::Logger::GetInstance().
        LogError
        (
          "Trying to set a delay filter latency (%d) larger than "
          "the maximum latency (%d). The latency will be set to the "
          "the maximum latency instead. ",
          latency,
          max_latency());
    }
    latency_ = std::min(latency, max_latency());
    read_index_ = (write_index_ >= latency_)
      ? write_index_ - latency_
      : write_index_ - latency_ + max_latency() + 1;
    ASSERT((read_index_ >= 0) & (read_index_ <= max_latency()));
  }

  /** Returns the current latency of the delay filter */
  size_t latency() const noexcept
  {
    return latency_;
  }

  /** Returns the maximum latency of the delay filter */
  size_t max_latency() const noexcept
  {
    return data_.size()-1;
  }

  void Filter(
    const mcl::Vector<T>& input,
    mcl::Vector<T>& output) noexcept
  {
    ASSERT(input.size() == output.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
      Write(input[i]);
      output[i] = Read();
      Tick();
    }
  }
protected:
  mcl::Vector<T> data_;
  size_t write_index_;
  size_t read_index_;
  size_t latency_;
};
} // namespace sal

#endif
