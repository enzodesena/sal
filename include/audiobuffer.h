/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SAL_AUDIOBUFFER_H
#define SAL_AUDIOBUFFER_H


#include "saltypes.h"
#include "pointwiseop.h"
#include <iostream>

namespace sal {
  
using mcl::Int;
  
  
class Buffer {
public:
  virtual Int num_samples() const noexcept = 0;
  
  virtual void AddSamples(const Buffer& buffer) noexcept = 0;
  
  virtual void Reset() noexcept = 0;
  
  virtual ~Buffer() {}
  
  static bool Test();
};
  
  
class MonoBuffer;
  
class MultichannelBuffer : public Buffer {
public:
  
  /** Constructs a multichannel buffer. */
  MultichannelBuffer(const Int num_channels, const Int num_samples) :
      num_channels_(num_channels), num_samples_(num_samples),
      owns_data_(true) {
    assert(num_channels >= 0 & num_samples >= 0);
    AllocateMemory();
  }
  
  /** Constructs a multichannel buffer as a reference to another data
   structure. If constructed in this way, this object will not own the data.
   
   @param[in] data_referenced the data structure which we are referencing to.
   @param[in] num_channels the number of channels for the data structure
   we are referencing to.
   @param[in] num_samples the number of samples for the data structure
   we are referencing to.
   */
  MultichannelBuffer(Sample** data_referenced,
                     const Int num_channels, const Int num_samples) noexcept :
  num_channels_(num_channels), num_samples_(num_samples), owns_data_(false) {
    data_ = data_referenced;
  }

//  MultichannelBuffer(const std::vector<MonoBuffer>& input_buffers) noexcept :
//      MultichannelBuffer(input_buffers.size(), input_buffers.at(0).num_samples()) {
//    for (Int chan_id = 0; chan_id<input_buffers.size(); ++chan_id) {
//      assert(input_buffers[0].num_samples() == input_buffers[chan_id].num_samples());
//      for (Int sample_id = 0;
//    }
//  }

  virtual Int num_channels() const noexcept { return num_channels_; }
  
  virtual Int num_samples() const noexcept { return num_samples_; }
  
  inline Sample GetSample(Int channel_id, Int sample_id) const noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    assert(sample_id >= 0 && sample_id < num_samples());
    
    return data_[channel_id][sample_id];
  }
  
  bool IsDataOwner() const noexcept { return owns_data_; }
  
  inline void SetSample(const Int channel_id, const Int sample_id,
                        const Sample sample) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    assert(sample_id >= 0 && sample_id < num_samples());
    
    data_[channel_id][sample_id] = sample;
  }
  
  /** Reassigns the values of a set of contigous samples in the buffer.
   
   @param[in] channel_id The ID of the channel.
   @param[in] from_sample_id The index of the first sample we want to modify.
   @param[in] num_samples The number of samples we want to modify.
   @param[in] samples The new samples.
   */
  void SetSamples(const Int channel_id, const Int from_sample_id,
                  const Int num_samples, const Sample* samples) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    
    assert(from_sample_id >= 0);
    assert(num_samples >= 0);
    assert((from_sample_id+num_samples) <= this->num_samples());

    for (Int sample_id=from_sample_id;
         sample_id<(from_sample_id+num_samples);
         ++sample_id) {
      data_[channel_id][sample_id] = samples[sample_id-from_sample_id];
    }
  }
  
  /** Adds samples to current sample values in the buffer.
   
   @param[in] channel_id The ID of the channel.
   @param[in] from_sample_id The index of the first sample we want to modify.
   @param[in] num_samples The number of samples we want to modify.
   @param[in] samples The new samples.
   */
  void AddSamples(const Int channel_id,
                  const Int from_sample_id,
                  const Int num_samples,
                  const Sample* samples) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    assert(from_sample_id >= 0);
    assert(num_samples >= 0);
    assert((from_sample_id+num_samples) <= num_samples_);
    
    for (Int sample_id=from_sample_id; sample_id<(from_sample_id+num_samples);
         ++sample_id) {
      data_[channel_id][sample_id] = samples[sample_id-from_sample_id];
    }
  }
  
  const Sample* GetReadPointer(const Int channel_id) const noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }
  
  Sample* GetWritePointer(const Int channel_id) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }
  
  Sample** GetWritePointers() noexcept { return data_; }
  
  /** Adds all the samples from another buffer. The buffer has to be of the
   same type (checked by dynamic_cast) and have the same number of channels
   and samples (checked only through debugging asserts).
   */
  virtual void AddSamples(const Buffer& buffer) noexcept {
    const MultichannelBuffer& multi_buffer =
          dynamic_cast<const MultichannelBuffer&>(buffer);
    
    assert(num_channels() == multi_buffer.num_channels());
    assert(num_samples() == buffer.num_samples());
    
    for (Int chan_id = 0; chan_id<num_channels(); ++chan_id) {
      mcl::Add(GetReadPointer(chan_id),
               multi_buffer.GetReadPointer(chan_id),
               num_samples(),
               GetWritePointer(chan_id));
    }
  }
  
  
  void PrintData() {
    for (int chan_id=0; chan_id<num_channels_; ++chan_id) {
      for (int sample_id=0; sample_id<num_samples_; ++sample_id) {
        std::cout<<data_[chan_id][sample_id]<<" ";
      }
      std::cout<<std::endl;
    }
  }
  
  /** Resets all the values to zero. */
  virtual void Reset() noexcept {
    for (Int chan_id = 0; chan_id<num_channels(); ++chan_id) {
      for (Int sample_id = 0; sample_id<num_samples(); ++sample_id) {
        data_[chan_id][sample_id] = 0.0;
      }
    }
  }
  
  enum ChannelLabels {
    mono_channel = 0,
    left_channel = 0,
    right_channel = 1
  };
  
    
  MultichannelBuffer(const MultichannelBuffer& other) :
  num_channels_(other.num_channels_), num_samples_(other.num_samples_),
  owns_data_(other.owns_data_) {
    if (owns_data_) {
      AllocateMemory();
      CopyDataFrom(other);
    } else {
      data_ = other.data_;
    }
  }
  
  /** Copy assignment operator. If you are trying to assign the object onto
   itself, this operator has no effect. Also, there is no effect if you try
   to assign a buffer that is referencing itself. For
   instance, if A is a buffer that owns the data, and B is a buffer that
   refers to A's data, then the assignment A = B has no effect. */
  MultichannelBuffer& operator=(const MultichannelBuffer& other) {
    if (this != &other) {
      if (owns_data_ && other.data_ == data_) {
        return *this;
      }
      
      if (owns_data_) { DeallocateMemory(); }
      
      num_channels_ = other.num_channels_;
      num_samples_ = other.num_samples_;
      owns_data_ = other.owns_data_;
      
      if (owns_data_) {
        AllocateMemory();
        CopyDataFrom(other);
      } else {
        data_ = other.data_;
      }
    }
    return *this;
  }
  
  virtual ~MultichannelBuffer() {
    if (owns_data_) { DeallocateMemory(); }
  }
    
  static bool Test();
private:
  Sample** data_;
  Int num_channels_;
  Int num_samples_;
  bool owns_data_;
  
  void CopyDataFrom(const MultichannelBuffer& other) {
    assert(num_samples_ == other.num_samples_);
    assert(num_channels_ == other.num_channels_);
    for (int chan_id=0; chan_id<num_channels_; ++chan_id) {
      for (int sample_id=0; sample_id<num_samples_; ++sample_id) {
        data_[chan_id][sample_id] = other.data_[chan_id][sample_id];
      }
    }
  }
  
  void AllocateMemory() {
    data_ = new double*[num_channels_];
    for (Int chan_id=0; chan_id<num_channels_; ++chan_id) {
      data_[chan_id] = new double[num_samples_]();
    }
  }
  
  void DeallocateMemory() {
    for (int chan_id=0; chan_id<num_channels_; ++chan_id) {
      delete[] data_[chan_id];
    }
    delete[] data_;
    data_ = nullptr;
  }
};
  
  
class MonoBuffer : public MultichannelBuffer {
public:
  explicit MonoBuffer(const Int num_samples) noexcept :
        MultichannelBuffer(1, num_samples) {}
  
  inline void SetSample(const Int sample_id,
                         const Sample sample_value) noexcept {
    MultichannelBuffer::SetSample(mono_channel, sample_id, sample_value);
  }
  
  inline Sample GetSample(const Int sample_id) const noexcept {
    return MultichannelBuffer::GetSample(mono_channel, sample_id);
  }
  
  const Sample* GetReadPointer() const noexcept {
    return MultichannelBuffer::GetReadPointer(mono_channel);
  }
  
  Sample* GetWritePointer() noexcept {
    return MultichannelBuffer::GetWritePointer(mono_channel);
  }
  
  static MonoBuffer Unary(const Sample sample) noexcept  {
    MonoBuffer output(1);
    output.SetSample(0, sample);
    return output;
  }
  
  virtual ~MonoBuffer() {}
};
  
class StereoBuffer : public MultichannelBuffer {
public:
  StereoBuffer(const Int num_samples) noexcept :
        MultichannelBuffer(2, num_samples) {}
  
  inline void SetLeftSample(const Int sample_id,
                              const Sample sample_value) noexcept {
    MultichannelBuffer::SetSample(left_channel, sample_id, sample_value);
  }
  
  inline void SetRightSample(const Int sample_id,
                               const Sample sample_value) noexcept {
    MultichannelBuffer::SetSample(right_channel, sample_id, sample_value);
  }
  
  inline Sample GetLeftSample(const Int sample_id) const noexcept {
    return MultichannelBuffer::GetSample(left_channel, sample_id);
  }
  
  inline Sample GetRightSample(const Int sample_id) const noexcept {
    return MultichannelBuffer::GetSample(right_channel, sample_id);
  }
  
  const Sample* GetLeftReadPointer() const noexcept {
    return MultichannelBuffer::GetReadPointer(left_channel);
  }
  
  const Sample* GetRightReadPointer() const noexcept {
    return MultichannelBuffer::GetReadPointer(right_channel);
  }
  
  Sample* GetLeftWritePointer() noexcept {
    return MultichannelBuffer::GetWritePointer(left_channel);
  }
  
  Sample* GetRightWritePointer() noexcept {
    return MultichannelBuffer::GetWritePointer(right_channel);
  }
  
  void AddSamplesLeft(const Sample* samples,
                      const Int from_sample_id,
                      const Int num_samples_to_add) noexcept {
    MultichannelBuffer::AddSamples(left_channel, from_sample_id,
                                   num_samples_to_add, samples);
  }
  
  void AddSamplesRight(const Sample* samples,
                       const Int from_sample_id,
                       const Int num_samples_to_add) noexcept {
    MultichannelBuffer::AddSamples(right_channel, from_sample_id,
                                   num_samples_to_add, samples);
  }
  
  virtual ~StereoBuffer() {}
};
  
} // End namespace

#endif
