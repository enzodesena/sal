/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "saltypes.hpp"
#include "pointwiseop.hpp"
#include "vectorop.hpp"
#include "mclintrinsics.hpp"
#include "digitalfilter.hpp"
#include <iostream>

namespace sal
{

template<typename T>
class Buffer
{
public:
  /** Constructs a multichannel buffer. */
  Buffer(
    const size_t num_channels,
    const size_t num_samples);

  Buffer() noexcept;
  
  size_t num_channels() const noexcept;

  size_t num_samples() const noexcept;

  mcl::Vector<T>& operator[](
    const size_t index) noexcept;

  const mcl::Vector<T>& operator[](
    const size_t index) const noexcept;
  
  T GetSample(
    size_t channel_id,
    size_t sample_id) const noexcept;

  bool OwnsData() const noexcept;
  
  void SetSample(
    const size_t channel_id,
    const size_t sample_id,
    const T sample) noexcept;

  /** Reassigns the values of a set of contigous samples in the buffer.
   
   @param[in] channel_id The ID of the channel.
   @param[in] from_sample_id The index of the first sample we want to modify.
   @param[in] num_samples The number of samples we want to modify.
   @param[in] samples The new samples.
   */
  void SetSamples(
    const size_t channel_id,
    const size_t from_sample_id,
    const mcl::Vector<T>& samples) noexcept;

  void SetSamples(
    const Buffer& other) noexcept;

  typename mcl::Vector<T>::ConstIterator begin(
    const size_t channel_id) const noexcept;

  typename mcl::Vector<T>::Iterator begin(
    const size_t channel_id) noexcept;

  typename mcl::Vector<T>::ConstIterator end(
    const size_t channel_id) const noexcept;

  typename mcl::Vector<T>::Iterator end(
    const size_t channel_id) noexcept;
  
  mcl::Vector<T> GetChannelReference(
    const size_t channel_id) noexcept;
  
  mcl::Vector<T> GetChannelConstReference(
    const size_t channel_id) const noexcept;

  /** Adds all the samples from another buffer. The buffer has to be of the
   same type and have the same number of channels
   and samples (checked only through debugging asserts).
   */
  void AddSamples(
    const Buffer& other_buffer) noexcept;

  void PrintData() noexcept;

  /** Resets all the values to zero. */
  void ResetSamples() noexcept;
  
  
  /** Constructs a multichannel buffer as a reference to another multichannel
   buffer. If constructed in this way, this object will not own the data.
   
   @param[in] data_referenced the data structure which we are referencing to.
   @param[in] first_element_index index of the first sample
   @param[in] size the number of samples for the data structure
   we are referencing to.
   */
  friend Buffer MakeBufferReference(
    Buffer& buffer,
    const size_t first_sample_index = 0,
    size_t num_samples = std::numeric_limits<size_t>::max(),
    const size_t first_channel_index = 0,
    size_t num_channels = std::numeric_limits<size_t>::max()) noexcept
  {
    num_samples =
      (num_samples < std::numeric_limits<size_t>::max())
      ? num_samples
      : buffer.num_samples();
    num_channels =
      (num_channels < std::numeric_limits<size_t>::max())
      ? num_channels
      : buffer.num_channels();
    ASSERT(num_samples <= buffer.num_samples());
    ASSERT(num_channels <= buffer.num_channels());
    return std::move(
      Buffer
      (
        buffer,
        first_sample_index,
        num_samples,
        first_channel_index,
        num_channels));
  }
  
  /** Constructs a multichannel buffer as a reference to a set of vectors.
   
   @param[in] data the set of vectors. The
    size of each vector has to be the same.
   */
  static Buffer MakeBufferReference(
    mcl::Vector<mcl::Vector<T>>& data) noexcept
  {
    return std::move(Buffer(data));
  }
  
private:
/** This implements the corresponding `MakeBufferReference` method.
  The reason why it was preferred to make this constructor private is to
  force clients to use the more explicit MakeBufferReference to make sure
  the client understand it is dealing with a reference.
 */
  Buffer(
    Buffer& buffer_referenced,
    const size_t first_sample_index,
    const size_t num_samples,
    const size_t first_channel_index,
    const size_t num_channels);

/** This implements the corresponding `MakeBufferReference` method.
  The reason why it was preferred to make this constructor private is to
  force clients to use the more explicit MakeBufferReference to make sure
  the client understand it is dealing with a reference.
 */
  Buffer(
    mcl::Vector<mcl::Vector<T>>& data);
  
  // Member variables
  mcl::Vector<mcl::Vector<T>> data_;
};


} // End namespace

#include "audiobuffer.ipp"
