/*
 delayfilter.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#define DEFAULT_MAX_LATENCY 3276800

#include "saltypes.hpp"
#include "salconstants.hpp"
#include <algorithm>

namespace sal
{
template<typename T>
class DelayFilter
{
public:
  /**
   Constructs a delay filter object with intial latency given by latency. A maximum
   latency has to be given to allocate the maximum amount of memory of the circular
   memory.
   */
  DelayFilter(
    const size_t latency,
    const size_t max_latency) noexcept;


  /**
   This writes the next sample into the filter. If this method is called 2 times
   before the Tick() operation, the former value will be overwritten.
   */
  void Write(
    const T sample) noexcept;


  void Write(
    const mcl::Vector<T>& input) noexcept;

  /** Resets the state of the filter */
  void ResetState() noexcept;


  /**
   Returns the current sample from the filter. Between two Tick() operation it will
   give always the same output.
   */
  inline T Read() const noexcept;


  /** This allows to read at a different location from the read pointer. */
  T ReadAt(
    const size_t delay_tap) const noexcept;

  T FractionalReadAt(
    const Time fractional_delay_tap) const noexcept;


  /** Read a certain number of samples.
   @param[out] output is the output vector.  */
  void Read(
    mcl::Vector<T>& output) const noexcept;


  /** This causes time to tick by one sample. */
  void Tick() noexcept;


  /** This causes time to tick by more than one sample (use only if you
   understand what you are doing!). */
  void Tick(
    size_t num_samples) noexcept;

  /**
   Resets the latency of the filter. This can introduce artifacts if the
   latency is updated too fast.
   */
  void SetLatency(
    size_t latency) noexcept;

  /** Returns the current latency of the delay filter */
  size_t latency() const noexcept;

  /** Returns the maximum latency of the delay filter */
  size_t max_latency() const noexcept;

  void Filter(
    const mcl::Vector<T>& input,
    mcl::Vector<T>& output) noexcept;
protected:
  mcl::Vector<T> data_;
  size_t write_index_;
  size_t read_index_;
  size_t latency_;
};






} // namespace sal


#include "delayfilter.ipp"
