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

namespace sal {
  
using mcl::Int;
  
  
class Buffer {
public:
  virtual Int num_samples() const noexcept = 0;
  
  virtual void AddSamples(const Buffer& buffer) noexcept = 0;
  
  virtual void Reset() noexcept = 0;
  
  static bool Test();
};
  
  
class MultichannelBuffer : public Buffer {
public:
  MultichannelBuffer(const Int num_channels, const Int num_samples) noexcept {
    assert(num_channels >= 0 & num_samples >= 0);
    data_ = std::vector<std::vector<sal::Sample> >(num_channels);
    for (Int i = 0; i<num_channels; ++i) {
      data_[i] = std::vector<sal::Sample>(num_samples, 0.0);
    }
  }

  virtual Int num_channels() const noexcept { return data_.size(); }
  
  virtual Int num_samples() const noexcept {
    return (num_channels() >= 1) ? data_[0].size() : 0;
  }
  
  inline Sample sample(Int channel_id, Int sample_id) const noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    assert(sample_id >= 0 && sample_id < num_samples());
    
    return data_[channel_id][sample_id];
  }
  
  inline void set_sample(const Int channel_id, const Int sample_id,
                         const Sample sample) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    assert(sample_id >= 0 && sample_id < num_samples());
    
    data_[channel_id][sample_id] = sample;
  }
  
  /** Adds samples to current sample values in the buffer. */
  void AddSamples(const Int channel_id,
                  const Sample* samples,
                  const Int start_from_sample_id,
                  const Int num_samples_to_add) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    
    assert(start_from_sample_id >= 0);
    assert(num_samples_to_add >= 0);
    assert((start_from_sample_id+num_samples_to_add) <= num_samples());
    
    for (Int sample_id=start_from_sample_id;
         sample_id<(start_from_sample_id+num_samples_to_add);
         ++sample_id) {
      data_[channel_id][sample_id] = samples[sample_id-start_from_sample_id];
    }
  }
  
  const Sample* GetReadPointer(const Int channel_id) const noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id].data();
  }
  
  Sample* GetWritePointer(const Int channel_id) noexcept {
    assert(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id].data();
  }
  
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
  
  static bool Test();
private:
  std::vector<std::vector<sal::Sample> > data_;
};
  
  
class MonoBuffer : public MultichannelBuffer {
public:
  explicit MonoBuffer(const Int num_samples) noexcept :
        MultichannelBuffer(1, num_samples) {}
  
  inline void set_sample(const Int sample_id,
                         const Sample sample_value) noexcept {
    MultichannelBuffer::set_sample(mono_channel, sample_id, sample_value);
  }
  
  inline Sample sample(const Int sample_id) const noexcept {
    return MultichannelBuffer::sample(mono_channel, sample_id);
  }
  
  const Sample* GetReadPointer() const noexcept {
    return MultichannelBuffer::GetReadPointer(mono_channel);
  }
  
  Sample* GetWritePointer() noexcept {
    return MultichannelBuffer::GetWritePointer(mono_channel);
  }
  
  static MonoBuffer Unary(const Sample sample) noexcept  {
    MonoBuffer output(1);
    output.set_sample(0, sample);
    return output;
  }
};
  
class StereoBuffer : public MultichannelBuffer {
public:
  StereoBuffer(const Int num_samples) noexcept :
        MultichannelBuffer(2, num_samples) {}
  
  inline void set_left_sample(const Int sample_id,
                              const Sample sample_value) noexcept {
    MultichannelBuffer::set_sample(left_channel, sample_id, sample_value);
  }
  
  inline void set_right_sample(const Int sample_id,
                               const Sample sample_value) noexcept {
    MultichannelBuffer::set_sample(right_channel, sample_id, sample_value);
  }
  
  inline Sample left_sample(const Int sample_id) const noexcept {
    return MultichannelBuffer::sample(left_channel, sample_id);
  }
  
  inline Sample right_sample(const Int sample_id) const noexcept {
    return MultichannelBuffer::sample(right_channel, sample_id);
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
                      const Int start_from_sample_id,
                      const Int num_samples_to_add) noexcept {
    MultichannelBuffer::AddSamples(left_channel, samples,
                                   start_from_sample_id, num_samples_to_add);
  }
  
  void AddSamplesRight(const Sample* samples,
                       const Int start_from_sample_id,
                       const Int num_samples_to_add) noexcept {
    MultichannelBuffer::AddSamples(right_channel, samples,
                                   start_from_sample_id, num_samples_to_add);
  }
};
  
} // End namespace

#endif
