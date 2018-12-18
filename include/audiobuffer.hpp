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
private:
  mcl::Vector<mcl::Vector<T>> data_;
  size_t num_channels_;
  size_t num_samples_;

  mcl::Vector<T> temporary_vector_; // Support vector for filter operation

  Buffer& buffer_reference_ = *this;
  bool owns_data_;


  void AllocateMemory();


public:
  /** Constructs a multichannel buffer. */
  Buffer(
    const size_t num_channels,
    const size_t num_samples);

  Buffer();
  
  virtual size_t num_channels() const noexcept;

  virtual size_t num_samples() const noexcept;

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
  virtual void AddSamples(
    const Buffer& other_buffer) noexcept;

  void PrintData();

  /** Resets all the values to zero. */
  virtual void SetSamplesToZero() noexcept;

  Buffer(
    const Buffer& other);

  /** Copy assignment operator. If you are trying to assign the object onto
   itself, this operator has no effect. Also, there is no effect if you try
   to assign a buffer that is referencing itself. For
   instance, if A is a buffer that owns the data, and B is a buffer that
   refers to A's data, then the assignment A = B has no effect. */
  Buffer& operator=(
    const Buffer& other);
};


template<typename T>
class MonoBuffer : public Buffer<T>
{
public:
  using Channel::kMono;

  explicit MonoBuffer(
    const size_t num_samples) noexcept;

  void SetSample(
    const size_t sample_id,
    const T sample_value) noexcept;

  using Buffer<T>::SetSamples;

  void SetSamples(
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples) noexcept;

  T GetSample(
    const size_t sample_id) const noexcept;

  static MonoBuffer Unary(
    const T sample) noexcept;

  using Buffer<T>::AddSamples;

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
    const size_t num_samples) noexcept;

  void SetLeftSample(
    const size_t sample_id,
    const T sample_value) noexcept;
  
  void SetRightSample(
    const size_t sample_id,
    const T sample_value) noexcept;

  T GetLeftSample(
    const size_t sample_id) const noexcept;

  T GetRightSample(
    const size_t sample_id) const noexcept;
};


template<typename T>
class BFormatBuffer : public Buffer<T>
{
public:
  BFormatBuffer(
    const size_t max_degree,
    const size_t num_samples);

  void SetSample(
    const Int degree,
    const size_t order,
    const size_t sample_id,
    const T& sample_value) noexcept;

  using Buffer<T>::AddSamples;

  void AddSamples(
    const Int degree,
    const size_t order,
    const size_t from_sample_id,
    const size_t num_samples,
    const T* samples);

  T GetSample(
    const Int degree,
    const size_t order,
    const size_t sample_id) const noexcept;
  
  static size_t GetChannelId(
    const Int degree,
    const size_t order);

  static size_t GetNumChannels(
    const size_t max_degree) noexcept;
};
} // End namespace

#include "audiobuffer_impl.hpp"
