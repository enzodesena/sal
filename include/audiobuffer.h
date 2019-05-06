/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_AUDIOBUFFER_H
#define SAL_AUDIOBUFFER_H


#include "saltypes.h"
#include "pointwiseop.h"
#include "vectorop.h"
#include "digitalfilter.h"
#include <iostream>

namespace sal {
  
using mcl::Int;
  
  
class MonoBuffer;
  
class Buffer {
public:
  /** Constructs a multichannel buffer. */
  Buffer(const Int num_channels, const Int num_samples) :
      num_channels_(num_channels), num_samples_(num_samples),
      owns_data_(true), temporary_vector_(std::vector<Sample>(num_samples, 0.0)) {
    ASSERT(num_channels >= 0 && num_samples >= 0);
    AllocateMemory();
  }
  
  Buffer() : Buffer(0,0) {}
  
  /** Constructs a multichannel buffer as a reference to another data
   structure. If constructed in this way, this object will not own the data.
   
   @param[in] data_referenced the data structure which we are referencing to.
   @param[in] num_channels the number of channels for the data structure
   we are referencing to.
   @param[in] num_samples the number of samples for the data structure
   we are referencing to.
   */
  Buffer(Sample** data_referenced,
         const Int num_channels, const Int num_samples) noexcept :
      num_channels_(num_channels), num_samples_(num_samples), owns_data_(false),
      temporary_vector_(std::vector<Sample>(num_samples, 0.0)) {
    data_ = data_referenced;
  }


  virtual Int num_channels() const noexcept { return num_channels_; }
  
  virtual Int num_samples() const noexcept { return num_samples_; }
  
  inline Sample GetSample(Int channel_id, Int sample_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());
    
    return data_[channel_id][sample_id];
  }
  
  bool IsDataOwner() const noexcept { return owns_data_; }
  
  inline void SetSample(const Int channel_id, const Int sample_id,
                        const Sample sample) noexcept {
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
  void SetSamples(const Int channel_id, const Int from_sample_id,
                  const Int num_samples, const Sample* samples) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    
    ASSERT(from_sample_id >= 0);
    ASSERT(num_samples >= 0);
    ASSERT((from_sample_id+num_samples) <= this->num_samples());

    for (Int sample_id=from_sample_id;
         sample_id<(from_sample_id+num_samples);
         ++sample_id) {
      data_[channel_id][sample_id] = samples[sample_id-from_sample_id];
    }
  }
  
  void SetSamples(const Buffer& other) noexcept {
    ASSERT(num_samples_ == other.num_samples_);
    ASSERT(num_channels_ == other.num_channels_);
    for (int chan_id=0; chan_id<num_channels_; ++chan_id) {
      for (int sample_id=0; sample_id<num_samples_; ++sample_id) {
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
  void AddSamples(const Int channel_id,
                  const Int from_sample_id,
                  const Int num_samples,
                  const Sample* samples) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT(num_samples >= 0);
    ASSERT((from_sample_id+num_samples) <= num_samples_);
    
    mcl::Add(samples,
             &(data_[channel_id][from_sample_id]), num_samples,
             &(data_[channel_id][from_sample_id]));
  }
  
  /** This method first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(const Int channel_id,
                          const Int from_sample_id,
                          const Int num_samples,
                          const Sample* samples,
                          const Sample constant) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT(num_samples >= 0);
    ASSERT((from_sample_id+num_samples) <= num_samples_);
    mcl::MultiplyAdd(samples, constant, &(data_[channel_id][from_sample_id]),
                     num_samples, &(data_[channel_id][from_sample_id]));
  }
  
  void FilterAddSamples(const Int channel_id,
                        const Int from_sample_id,
                        const Int num_samples,
                        const Sample* samples,
                        mcl::DigitalFilter& filter) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(from_sample_id >= 0);
    ASSERT(num_samples >= 0);
    ASSERT((from_sample_id+num_samples) <= num_samples_);
    filter.Filter(samples, num_samples, temporary_vector_.data());
    mcl::Add(temporary_vector_.data(),
             &(data_[channel_id][from_sample_id]), num_samples,
             &(data_[channel_id][from_sample_id]));
  }

  
  const Sample* GetReadPointer(const Int channel_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }
  
  Sample* GetWritePointer(const Int channel_id) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }
  
  Sample** GetWritePointers() noexcept { return data_; }
  
  /** Adds all the samples from another buffer. The buffer has to be of the
   same type and have the same number of channels
   and samples (checked only through debugging asserts).
   */
  virtual void AddSamples(const Buffer& buffer) noexcept {
    ASSERT(num_channels() == buffer.num_channels());
    ASSERT(num_samples() == buffer.num_samples());
    
    for (Int chan_id = 0; chan_id<num_channels(); ++chan_id) {
      mcl::Add(GetReadPointer(chan_id),
               buffer.GetReadPointer(chan_id),
               num_samples(),
               GetWritePointer(chan_id));
    }
  }
  
  void SetFrame(const Int channel_id,
                const Int frame_id,
                const Int frame_length,
                const Signal& signal) {
    for (mcl::Int n=0; n<num_samples(); ++n) {
      mcl::Int index = frame_id*frame_length + n;
      if (index < (mcl::Int) signal.size()) {
        SetSample(channel_id, n, signal[index]);
      } else {
        SetSample(channel_id, n, 0.0);
      }
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
    kMonoChannel = 0,
    kLeftChannel = 0,
    kRightChannel = 1
  };
  
  
  Buffer(const Buffer& other) :
      num_channels_(other.num_channels_), num_samples_(other.num_samples_),
      owns_data_(other.owns_data_),
      temporary_vector_(std::vector<Sample>(other.num_samples(), 0.0)) {
    if (owns_data_) {
      AllocateMemory();
      SetSamples(other);
    } else {
      data_ = other.data_;
    }
  }
  
  /** Copy assignment operator. If you are trying to assign the object onto
   itself, this operator has no effect. Also, there is no effect if you try
   to assign a buffer that is referencing itself. For
   instance, if A is a buffer that owns the data, and B is a buffer that
   refers to A's data, then the assignment A = B has no effect. */
  Buffer& operator=(const Buffer& other) {
    if (this != &other) {
      if (owns_data_ && other.data_ == data_) { return *this; }
      
      if (owns_data_) { DeallocateMemory(); }
      
      num_channels_ = other.num_channels_;
      num_samples_ = other.num_samples_;
      owns_data_ = other.owns_data_;
      temporary_vector_ = std::vector<Sample>(other.num_samples(), 0.0);
      
      if (owns_data_) {
        AllocateMemory();
        SetSamples(other);
      } else {
        data_ = other.data_;
      }
    }
    return *this;
  }
  
  virtual ~Buffer() {
    if (owns_data_) { DeallocateMemory(); }
  }
    
  static bool Test();
  
private:
  Sample** data_;
  Int num_channels_;
  Int num_samples_;
  bool owns_data_;
  std::vector<Sample> temporary_vector_; // Support vector for filter operation
  
  void AllocateMemory() {
    data_ = new Sample*[num_channels_];
    for (Int chan_id=0; chan_id<num_channels_; ++chan_id) {
      data_[chan_id] = new Sample[num_samples_]();
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
  
  
class MonoBuffer : public Buffer {
public:
  explicit MonoBuffer(const Int num_samples) noexcept :
        Buffer(1, num_samples) {}
  
  MonoBuffer(Sample* data_referenced, const Int num_samples) noexcept :
        Buffer(&data_referenced_, 1, num_samples),
        data_referenced_(data_referenced) {}
  
  /** Constructs a mono buffer as a reference to a multichannel buffer.
   If constructed in this way, this object will not own the data.
   
   @param[in] referenced_buffer the buffer structure which we are referencing to.
   @param[in] channel_id the channel id to be referenced.
   */
  MonoBuffer(Buffer& referenced_buffer, const Int channel_id) noexcept :
    Buffer(&(referenced_buffer.GetWritePointers()[channel_id]), 1,
                       referenced_buffer.num_samples()) {}
  
  /** This first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(const Int from_sample_id,
                          const Int num_samples,
                          const Sample* samples,
                          const Sample constant) noexcept {
    Buffer::MultiplyAddSamples(kMonoChannel, from_sample_id, num_samples,
                               samples, constant);
  }
  
  inline void SetSample(const Int sample_id,
                        const Sample sample_value) noexcept {
    Buffer::SetSample(kMonoChannel, sample_id, sample_value);
  }
  
  using Buffer::SetSamples;
  
  void SetSamples(const Int from_sample_id, const Int num_samples,
                  const Sample* samples) noexcept {
    Buffer::SetSamples(kMonoChannel, from_sample_id, num_samples,
                                   samples);
  }
  
  void SetFrame(const Int frame_id,
                const Int frame_length,
                const Signal& signal) {
    Buffer::SetFrame(0, frame_id, frame_length, signal);
  }
  
  inline Sample GetSample(const Int sample_id) const noexcept {
    return Buffer::GetSample(kMonoChannel, sample_id);
  }
  
  const Sample* GetReadPointer() const noexcept {
    return Buffer::GetReadPointer(kMonoChannel);
  }
  
  Sample* GetWritePointer() noexcept {
    return Buffer::GetWritePointer(kMonoChannel);
  }
  
  static MonoBuffer Unary(const Sample sample) noexcept  {
    MonoBuffer output(1);
    output.SetSample(0, sample);
    return output;
  }
  
  using Buffer::AddSamples;
  
  void AddSamples(const Int from_sample_id,
                  const Int num_samples,
                  const Sample* samples) noexcept {
    Buffer::AddSamples(kMonoChannel, from_sample_id, num_samples,
                                   samples);
  }
  
  virtual ~MonoBuffer() {}
private:
  // We use in case of the MonoBuffer(Sample* data_referenced, const Int num_samples)
  // constructor. We need this because taking &data_referenced as the Sample**
  // would be taking the address of a temporary.
  Sample* data_referenced_;
};
  
class StereoBuffer : public Buffer {
public:
  StereoBuffer(const Int num_samples) noexcept :
        Buffer(2, num_samples) {}
  
  inline void SetLeftSample(const Int sample_id,
                              const Sample sample_value) noexcept {
    Buffer::SetSample(kLeftChannel, sample_id, sample_value);
  }
  
  inline void SetRightSample(const Int sample_id,
                               const Sample sample_value) noexcept {
    Buffer::SetSample(kRightChannel, sample_id, sample_value);
  }
  
  inline Sample GetLeftSample(const Int sample_id) const noexcept {
    return Buffer::GetSample(kLeftChannel, sample_id);
  }
  
  inline Sample GetRightSample(const Int sample_id) const noexcept {
    return Buffer::GetSample(kRightChannel, sample_id);
  }
  
  const Sample* GetLeftReadPointer() const noexcept {
    return Buffer::GetReadPointer(kLeftChannel);
  }
  
  const Sample* GetRightReadPointer() const noexcept {
    return Buffer::GetReadPointer(kRightChannel);
  }
  
  Sample* GetLeftWritePointer() noexcept {
    return Buffer::GetWritePointer(kLeftChannel);
  }
  
  Sample* GetRightWritePointer() noexcept {
    return Buffer::GetWritePointer(kRightChannel);
  }
  
  void AddSamplesLeft(const Sample* samples,
                      const Int from_sample_id,
                      const Int num_samples_to_add) noexcept {
    Buffer::AddSamples(kLeftChannel, from_sample_id,
                                   num_samples_to_add, samples);
  }
  
  void FilterAddSamplesLeft(const Int from_sample_id,
                            const Int num_samples,
                            const Sample* samples,
                            mcl::DigitalFilter& filter) noexcept {
    Buffer::FilterAddSamples(kLeftChannel, from_sample_id,
                                         num_samples, samples, filter);
  }
  
  void FilterAddSamplesRight(const Int from_sample_id,
                             const Int num_samples,
                             const Sample* samples,
                             mcl::DigitalFilter& filter) noexcept {
    Buffer::FilterAddSamples(kRightChannel, from_sample_id,
                                         num_samples, samples, filter);
  }
  
  void AddSamplesRight(const Sample* samples,
                       const Int from_sample_id,
                       const Int num_samples_to_add) noexcept {
    Buffer::AddSamples(kRightChannel, from_sample_id,
                                   num_samples_to_add, samples);
  }
  
  virtual ~StereoBuffer() {}
};
  
  
class BFormatBuffer : public Buffer {
public:
  BFormatBuffer(const Int max_degree, const Int num_samples) :
      Buffer(GetNumChannels(max_degree), num_samples) {}
  
  inline void SetSample(const Int degree, const Int order, const Int sample_id,
                        const Sample& sample_value) noexcept {
    Buffer::SetSample(GetChannelId(degree, order),
                                  sample_id, sample_value);
  }
  
  using Buffer::AddSamples;
  
  void AddSamples(const Int degree, const Int order,
                  const Int from_sample_id,
                  const Int num_samples,
                  const Sample* samples) {
    Buffer::AddSamples(GetChannelId(degree, order),
                                   from_sample_id, num_samples, samples);
  }
  
  /** This first multiplies all the input samples by a certain constant
   and then adds the result to the samples in the buffer. */
  void MultiplyAddSamples(const Int degree, const Int order,
                          const Int from_sample_id,
                          const Int num_samples,
                          const Sample* samples,
                          const Sample constant) noexcept {
    Buffer::MultiplyAddSamples(GetChannelId(degree, order),
                               from_sample_id, num_samples,
                               samples, constant);
  }
  
  inline Sample GetSample(const Int degree, const Int order,
                          const Int sample_id) const noexcept {
    return Buffer::GetSample(GetChannelId(degree, order), sample_id);
  }
  
  static Int GetChannelId(const Int degree, const Int order) {
    ASSERT(degree >= 0);
    ASSERT(order <= std::abs(degree));
    Int centre_index = 0;
    for (Int degree_id = 0; degree_id <= degree; ++degree_id) {
      centre_index = centre_index + 2*degree_id;
    }
    // 0 + 2*0 = 0 OK
    // 0 + 2*1 = 2 OK
    // 2 + 2*2 = 6 OK
    // 6 + 2*3 = 12 OK
    ASSERT(centre_index + order >= 0);
    return centre_index + order;
  }
  
  static Int GetNumChannels(const Int max_degree) {
    ASSERT(max_degree > 0);
    return (max_degree+1)*(max_degree+1); // (N+1)^2
  }
};
  
} // End namespace

#endif
