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
enum class Channel
{
  kMono = 0,
  kLeft = 0,
  kRight = 1
};


template<typename T>
class Buffer
{
private:
  mcl::Vector<mcl::Vector<T>> data_;
  size_t num_channels_;
  size_t num_samples_;

  mcl::Vector<T> temporary_vector_; // Support vector for filter operation

  Buffer& buffer_reference_ = *this;
  bool owns_data_;


  void AllocateMemory()
  {
    data_ = mcl::Vector<mcl::Vector<T>>(num_channels_);
    for (size_t chan_id = 0; chan_id < num_channels_; ++chan_id)
    {
      data_[chan_id] = mcl::Vector<T>(num_samples_);
    }
  }


public:
  /** Constructs a multichannel buffer. */
  Buffer(
    const size_t num_channels,
    const size_t num_samples)
    : num_channels_(num_channels)
    , num_samples_(num_samples)
    , temporary_vector_(mcl::Vector<T>(num_samples, 0.0))
  {
    ASSERT(num_channels >= 0 && num_samples >= 0);
    AllocateMemory();
  }


  Buffer()
    : Buffer(0, 0)
  {
  }


  /** Constructs a multichannel buffer as a reference to another data
   structure. If constructed in this way, this object will not own the data.
   
   @param[in] data_referenced the data structure which we are referencing to.
   @param[in] num_channels the number of channels for the data structure
   we are referencing to.
   @param[in] num_samples the number of samples for the data structure
   we are referencing to.
   */
  //  Buffer(
  //    Buffer& buffer_referenced,
  //    const size_t num_channels,
  //    const size_t num_samples) noexcept
  //    : num_channels_(num_channels)
  //    , num_samples_(num_samples)
  //    , owns_data_(false)
  //    , temporary_vector_(mcl::Vector <T>(num_samples, 0.0))
  //  {
  //    data_ = data_referenced;
  //  }

  virtual size_t num_channels() const noexcept
  {
    return num_channels_;
  }


  virtual size_t num_samples() const noexcept
  {
    return num_samples_;
  }


  T GetSample(
    size_t channel_id,
    size_t sample_id) const noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());
    return data_[channel_id][sample_id];
  }


  bool IsDataOwner() const noexcept
  {
    return owns_data_;
  }


  void SetSample(
    const size_t channel_id,
    const size_t sample_id,
    const T sample) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());
    data_[channel_id][sample_id] = sample;
  }


  /** Reassigns the values of a set of contigous samples in the buffer.
   
   @param[in] channel_id The ID of the channel.
   @param[in] from_sample_id The index of the first sample we want to modify.
   @param[in] num_samples The number of samples we want to modify.
   @param[in] samples The new samples.
   */
  void SetSamples(
    const size_t channel_id,
    const size_t from_sample_id,
    const mcl::Vector<T>& samples) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT((from_sample_id + samples.size()) <= num_samples());

    for (size_t sample_id = from_sample_id;
         sample_id < (from_sample_id + samples.size());
         ++sample_id)
    {
      data_[channel_id][sample_id] = samples[sample_id - from_sample_id];
    }
  }


  void SetSamples(
    const Buffer& other) noexcept
  {
    ASSERT(num_samples_ == other.num_samples_);
    ASSERT(num_channels_ == other.num_channels_);
    for (size_t chan_id = 0; chan_id < num_channels_; ++chan_id)
    {
      for (size_t sample_id = 0; sample_id < num_samples_; ++sample_id)
      {
        data_[chan_id][sample_id] = other.data_[chan_id][sample_id];
      }
    }
  }


  /** Adds samples to current sample values in the buffer.
   
   @param[in] channel_id The ID of the channel.
   @param[in] from_sample_id The index of the first sample we want to modify.
   @param[in] num_samples The number of samples we want to modify.
   @param[in] samples The new samples.
   */
  void AddSamples(
    const size_t channel_id,
    const size_t from_sample_id,
    const mcl::Vector<T>& samples) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT((from_sample_id + samples.size()) <= num_samples());

    mcl::Add
    (
      samples,
      &(data_[channel_id][from_sample_id]),
      &(data_[channel_id][from_sample_id]));
  }


  /** This method first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(
    const size_t channel_id,
    const size_t from_sample_id,
    const mcl::Vector<T>& samples,
    const T constant) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT((from_sample_id + samples.size()) <= num_samples_);
    mcl::MultiplyAdd
    (
      samples,
      constant,
      Vector(data_[channel_id], from_sample_id, samples.size()),
      Vector(data_[channel_id], from_sample_id, samples.size()));
  }


  void FilterAddSamples(
    const size_t channel_id,
    const size_t from_sample_id,
    const mcl::Vector<T>& samples,
    mcl::DigitalFilter<T>& filter) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT((from_sample_id + samples.size()) <= num_samples_);
    filter.Filter(samples, temporary_vector_);
    mcl::Add
    (
      temporary_vector_,
      Vector(data_[channel_id], from_sample_id, samples.size()),
      Vector(data_[channel_id], from_sample_id, samples.size()));
  }


  const mcl::Vector<T>& GetReadReference(
    const size_t channel_id) const noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }


  mcl::Vector<T>& GetWriteReference(
    const size_t channel_id) noexcept
  {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }


  /** Adds all the samples from another buffer. The buffer has to be of the
   same type and have the same number of channels
   and samples (checked only through debugging asserts).
   */
  virtual void AddSamples(
    const Buffer& other_buffer) noexcept
  {
    ASSERT(num_channels() == other_buffer.num_channels());
    ASSERT(num_samples() == other_buffer.num_samples());

    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id)
    {
      mcl::Add
      (
        GetReadReference(chan_id),
        other_buffer.GetReadReference(chan_id),
        GetWriteReference(chan_id));
    }
  }


  void PrintData()
  {
    for (int chan_id = 0; chan_id < num_channels_; ++chan_id)
    {
      for (int sample_id = 0; sample_id < num_samples_; ++sample_id)
      {
        std::cout << data_[chan_id][sample_id] << " ";
      }
      std::cout << std::endl;
    }
  }


  /** Resets all the values to zero. */
  virtual void Reset() noexcept
  {
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id)
    {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id)
      {
        data_[chan_id][sample_id] = 0.0;
      }
    }
  }


  Buffer(
    const Buffer& other)
    : num_channels_(other.num_channels_)
    , num_samples_(other.num_samples_)
    , temporary_vector_(mcl::Vector<T>(other.num_samples(), 0.0))
    , owns_data_(other.owns_data_)
  {
    if (owns_data_)
    {
      AllocateMemory();
      SetSamples(other);
    }
    else
    {
      data_ = other.data_;
    }
  }


  /** Copy assignment operator. If you are trying to assign the object onto
   itself, this operator has no effect. Also, there is no effect if you try
   to assign a buffer that is referencing itself. For
   instance, if A is a buffer that owns the data, and B is a buffer that
   refers to A's data, then the assignment A = B has no effect. */
  Buffer& operator=(
    const Buffer& other)
  {
    if (this != &other)
    {
      if (owns_data_ && &(other.data_[0]) == &(data_[0]))
      {
        return *this;
      }

      num_channels_ = other.num_channels_;
      num_samples_ = other.num_samples_;
      owns_data_ = other.owns_data_;
      temporary_vector_ = mcl::Vector<T>(other.num_samples(), 0.0);

      if (owns_data_)
      {
        AllocateMemory();
        SetSamples(other);
      }
      else
      {
        data_ = other.data_;
      }
    }
    return *this;
  }


  static bool Test();
};


template<typename T>
class MonoBuffer : public Buffer<T>
{
public:
  explicit MonoBuffer(
    const size_t num_samples) noexcept
    : Buffer<T>(1, num_samples)
  {
  }


  //  MonoBuffer(
  //    Sample* data_referenced,
  //    const size_t num_samples) noexcept
  //    : Buffer(&data_referenced_, 1, num_samples)
  //    , data_referenced_(data_referenced)
  //  {
  //  }

  /** Constructs a mono buffer as a reference to a multichannel buffer.
   If constructed in this way, this object will not own the data.
   
   @param[in] referenced_buffer the buffer structure which we are referencing to.
   @param[in] channel_id the channel id to be referenced.
   */
  //  MonoBuffer(
  //    Buffer& referenced_buffer,
  //    const size_t channel_id) noexcept
  //    : Buffer(&(referenced_buffer.GetWritePointers()[channel_id]), 1, referenced_buffer.num_samples())
  //  {
  //  }

  /** This first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples,
    const T constant) noexcept
  {
    Buffer<T>::MultiplyAddSamples
    (
      Channel::kMono,
      from_sample_id,
      num_samples,
      samples,
      constant);
  }


  void SetSample(
    const size_t sample_id,
    const T sample_value) noexcept
  {
    Buffer<T>::SetSample(Channel::kMono, sample_id, sample_value);
  }


  using Buffer<T>::SetSamples;


  void SetSamples(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples) noexcept
  {
    Buffer<T>::SetSamples
    (
      Channel::kMono,
      from_sample_id,
      num_samples,
      samples);
  }


  T GetSample(
    const size_t sample_id) const noexcept
  {
    return Buffer<T>::GetSample(Channel::kMono, sample_id);
  }


  const T* GetReadPointer() const noexcept
  {
    return Buffer<T>::GetReadPointer(Channel::kMono);
  }


  T* GetWritePointer() noexcept
  {
    return Buffer<T>::GetWritePointer(Channel::kMono);
  }


  static MonoBuffer Unary(
    const T sample) noexcept
  {
    MonoBuffer output(1);
    output.SetSample(0, sample);
    return output;
  }


  using Buffer<T>::AddSamples;


  void AddSamples(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples) noexcept
  {
    Buffer<T>::AddSamples
    (
      Channel::kMono,
      from_sample_id,
      num_samples,
      samples);
  }


  virtual ~MonoBuffer()
  {
  }


private:
  // We use in case of the MonoBuffer(Sample* data_referenced, const size_t num_samples)
  // constructor. We need this because taking &data_referenced as the T**
  // would be taking the address of a temporary.
  T* data_referenced_;
};


template<typename T>
class StereoBuffer : public Buffer<T>
{
public:
  StereoBuffer(
    const size_t num_samples) noexcept
    : Buffer<T>(2, num_samples)
  {
  }


  void SetLeftSample(
    const size_t sample_id,
    const T sample_value) noexcept
  {
    Buffer<T>::SetSample(Channel::kLeft, sample_id, sample_value);
  }


  void SetRightSample(
    const size_t sample_id,
    const T sample_value) noexcept
  {
    Buffer<T>::SetSample(Channel::kRight, sample_id, sample_value);
  }


  T GetLeftSample(
    const size_t sample_id) const noexcept
  {
    return Buffer<T>::GetSample(Channel::kLeft, sample_id);
  }


  T GetRightSample(
    const size_t sample_id) const noexcept
  {
    return Buffer<T>::GetSample(Channel::kRight, sample_id);
  }


  const T* GetLeftReadPointer() const noexcept
  {
    return Buffer<T>::GetReadPointer(Channel::kLeft);
  }


  const T* GetRightReadPointer() const noexcept
  {
    return Buffer<T>::GetReadPointer(Channel::kRight);
  }


  T* GetLeftWritePointer() noexcept
  {
    return Buffer<T>::GetWritePointer(Channel::kLeft);
  }


  T* GetRightWritePointer() noexcept
  {
    return Buffer<T>::GetWritePointer(Channel::kRight);
  }


  void AddSamplesLeft(
    const T* samples,
    const size_t from_sample_id,
    const size_t num_samples_to_add) noexcept
  {
    Buffer<T>::AddSamples
    (
      Channel::kLeft,
      from_sample_id,
      num_samples_to_add,
      samples);
  }


  void FilterAddSamplesLeft(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples,
    mcl::DigitalFilter<T>& filter) noexcept
  {
    Buffer<T>::FilterAddSamples
    (
      Channel::kLeft,
      from_sample_id,
      num_samples,
      samples,
      filter);
  }


  void FilterAddSamplesRight(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples,
    mcl::DigitalFilter<T>& filter) noexcept
  {
    Buffer<T>::FilterAddSamples
    (
      Channel::kRight,
      from_sample_id,
      num_samples,
      samples,
      filter);
  }


  void AddSamplesRight(
    const T* samples,
    const size_t from_sample_id,
    const size_t num_samples_to_add) noexcept
  {
    Buffer<T>::AddSamples
    (
      Channel::kRight,
      from_sample_id,
      num_samples_to_add,
      samples);
  }


  virtual ~StereoBuffer()
  {
  }
};


template<typename T>
class BFormatBuffer : public Buffer<T>
{
public:
  BFormatBuffer(
    const size_t max_degree,
    const size_t num_samples)
    : Buffer<T>(GetNumChannels(max_degree), num_samples)
  {
  }


  void SetSample(
    const Int degree,
    const size_t order,
    const size_t sample_id,
    const T& sample_value) noexcept
  {
    Buffer<T>::SetSample(GetChannelId(degree, order), sample_id, sample_value);
  }


  using Buffer<T>::AddSamples;


  void AddSamples(
    const Int degree,
    const size_t order,
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples)
  {
    Buffer<T>::AddSamples
    (
      GetChannelId(degree, order),
      from_sample_id,
      num_samples,
      samples);
  }


  /** This first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(
    const Int degree,
    const size_t order,
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples,
    const T constant) noexcept
  {
    Buffer<T>::MultiplyAddSamples
    (
      GetChannelId(degree, order),
      from_sample_id,
      num_samples,
      samples,
      constant);
  }


  T GetSample(
    const Int degree,
    const size_t order,
    const size_t sample_id) const noexcept
  {
    return Buffer<T>::GetSample(GetChannelId(degree, order), sample_id);
  }


  static size_t GetChannelId(
    const Int degree,
    const size_t order)
  {
    ASSERT(degree >= 0);
    ASSERT(order <= std::abs(degree));
    size_t centre_index = 0;
    for (Int degree_id = 0; degree_id <= degree; ++degree_id)
    {
      centre_index = centre_index + 2 * degree_id;
    }
    // 0 + 2*0 = 0 OK
    // 0 + 2*1 = 2 OK
    // 2 + 2*2 = 6 OK
    // 6 + 2*3 = 12 OK
    ASSERT(centre_index + order >= 0);
    return centre_index + order;
  }


  static size_t GetNumChannels(
    const size_t max_degree) noexcept
  {
    ASSERT(max_degree > 0);
    return (max_degree + 1) * (max_degree + 1); // (N+1)^2
  }
};
} // End namespace
