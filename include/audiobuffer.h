/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_AUDIOBUFFER_H
#define SAL_AUDIOBUFFER_H

#include <iostream>
#include <span>

#include "digitalfilter.h"
#include "pointwiseop.h"
#include "saltypes.h"
#include "vectorop.h"

namespace sal {

using sal::dsp::Int;

class MonoBuffer;

class Buffer {
 public:
  /** Constructs a multichannel buffer. */
  Buffer(const size_t num_channels, const size_t num_samples)
      : temporary_vector_(std::vector<Sample>(num_samples, 0.0)) {
    ASSERT(num_channels >= 0 && num_samples >= 0);
    data_.reserve(num_channels);
    views_.reserve(num_channels);
    for (size_t chan_id = 0; chan_id < num_channels; ++chan_id) {
      data_.emplace_back(std::vector<Sample>(num_samples));
      views_.emplace_back(data_[chan_id]);
    }
  }

  Buffer() : Buffer(0, 0) {}

  // Copy constructor
  Buffer(const Buffer& other)
      : data_(other.data_), views_(other.views_), temporary_vector_(other.temporary_vector_) {
    if (other.OwnsData()) {
      for (size_t chan_id = 0; chan_id < data_.size(); ++chan_id) {
        views_[chan_id] = std::span<Sample>(data_[chan_id]);
      }
    }
  }
  
  /** Copy assignment operator. If you are trying to assign the object onto
   itself, this operator has no effect. Also, there is no effect if you try
   to assign a buffer that is referencing itself. For
   instance, if A is a buffer that owns the data, and B is a buffer that
   refers to A's data, then the assignment A = B has no effect. */
  Buffer& operator=(const Buffer& other) {
    if (this != &other) {
      if (OwnsData() &&
          num_channels() > 0 &&
          other.num_channels() > 0 &&
          other.views_[0].begin() == views_[0].begin()) { return *this; }
      
      temporary_vector_ = other.temporary_vector_;
      views_.clear();
      views_.reserve(other.views_.size());
      if (other.OwnsData()) {
        data_ = other.data_;
        for (size_t chan_id = 0; chan_id < data_.size(); ++chan_id) {
          views_.emplace_back(data_[chan_id]);
        }
      } else {
        data_ = {};
        views_ = other.views_;
      }
    }
    return *this;
  }
  
  // Move constructor
  Buffer(Buffer&& other) noexcept
      : data_(std::move(other.data_)),
        views_(std::move(other.views_)),
        temporary_vector_(std::move(other.temporary_vector_)) {
    if (OwnsData()) {
      // Re-establish views_ to reference the moved data_
      for (size_t chan_id = 0; chan_id < data_.size(); ++chan_id) {
        views_[chan_id] = std::span<Sample>(data_[chan_id]);
      }
    }
    // If other doesn't own data, views_ is already correctly set to refer to
    // external data.
  }

  // Move assignment operator
  Buffer& operator=(Buffer&& other) noexcept {
    if (this != &other) {
      if (OwnsData() && num_channels() > 0 && other.num_channels() > 0 &&
          other.views_[0].begin() == views_[0].begin()) {
        return *this;
      }

      temporary_vector_ = std::move(other.temporary_vector_);
      views_.clear();

      if (other.OwnsData()) {
        // Move data and set up views to point to our moved data_
        data_ = std::move(other.data_);
        other.views_ = {};
        views_.reserve(data_.size());
        for (size_t chan_id = 0; chan_id < data_.size(); ++chan_id) {
          views_.emplace_back(data_[chan_id]); // Still copy again in case std::move(other.data_) didn't do its job properly
        }
      } else {
        // Just move views_ if we're a non-owning view, and clear data_
        data_ = {};
        views_ = std::move(other.views_);
      }
    }
    return *this;
  }

  
  
  // Creates a buffer that views external data
  Buffer(std::vector<std::span<Sample>> external_data)
      : views_(std::move(external_data)) {
    ASSERT(data_.empty()); // Ensures that data_ is left empty to signify no ownership
#ifndef NDEBUG
    for (const auto& span : views_) {
      ASSERT(!views_.empty() && span.size() == views_[0].size());
    }
#endif
  }
  
  // Creates a mono buffer that views external data
  Buffer(std::span<Sample> external_data)
      : views_(std::vector<std::span<Sample>>(1, external_data)) {
    ASSERT(data_.empty()); // Ensures that data_ is left empty to signify no ownership
  }
  
  virtual size_t num_channels() const noexcept { return views_.size(); }

  virtual size_t num_samples() const noexcept {
    return (num_channels() > 0) ? views_[0].size() : 0;
  }

  inline Sample GetSample(size_t channel_id, size_t sample_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());

    return views_[channel_id][sample_id];
  }

  inline void SetSample(const size_t channel_id, const size_t sample_id,
                        const Sample sample) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());

    views_[channel_id][sample_id] = sample;
  }

  void SetSamples(const Buffer& other) noexcept {
    ASSERT(num_samples() == other.num_samples());
    ASSERT(num_channels() == other.num_channels());
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        views_[chan_id][sample_id] = other.views_[chan_id][sample_id];
      }
    }
  }

  std::span<const Sample> GetReadView(const size_t channel_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return views_[channel_id];
  }

  std::span<Sample> GetWriteView(const size_t channel_id) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return views_[channel_id];
  }
  
  std::vector<std::span<Sample>> GetWriteViews() noexcept {
    return views_;
  }

  void PrintData() {
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        std::cout << views_[chan_id][sample_id] << " ";
      }
      std::cout << std::endl;
    }
  }

  /** Resets all the values to zero. */
  virtual void Reset() noexcept {
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        views_[chan_id][sample_id] = 0.0;
      }
    }
  }
  
  bool OwnsData() const noexcept {
    return views_.size() == data_.size();
  }

  enum ChannelLabels { kMonoChannel = 0, kLeftChannel = 0, kRightChannel = 1 };

  static bool Test();

  static void AddBuffers(const Buffer& input_buffer_a,
                         const Buffer& input_buffer_b,
                         Buffer& output_buffer) noexcept {
    ASSERT(input_buffer_a.num_channels() == input_buffer_b.num_channels());
    ASSERT(input_buffer_a.num_channels() == output_buffer.num_channels());
    ASSERT(input_buffer_a.num_samples() == input_buffer_b.num_samples());
    ASSERT(input_buffer_a.num_samples() == output_buffer.num_samples());
    for (size_t chan_id = 0; chan_id < input_buffer_a.num_channels();
         ++chan_id) {
      dsp::Add(input_buffer_a.GetReadView(chan_id),
               input_buffer_b.GetReadView(chan_id),
               output_buffer.GetWriteView(chan_id));
    }
  }

 private:
  std::vector<std::span<Sample>> views_; // If the buffer owns the data this points to data_
  std::vector<std::vector<Sample>> data_; // If the buffer does not own the data, this is empty
  std::vector<Sample> temporary_vector_;  // Support vector for filter operation
};


class MonoBuffer : public Buffer {
 public:
  explicit MonoBuffer(const Int num_samples) noexcept
      : Buffer(1, num_samples) {}

  MonoBuffer(std::span<Sample> external_data) noexcept
      : Buffer(external_data) {}
  
  inline void SetSample(const Int sample_id,
                        const Sample sample_value) noexcept {
    Buffer::SetSample(kMonoChannel, sample_id, sample_value);
  }

  inline Sample GetSample(const Int sample_id) const noexcept {
    return Buffer::GetSample(kMonoChannel, sample_id);
  }

  std::span<const Sample> GetReadView() const noexcept {
    return Buffer::GetReadView(kMonoChannel);
  }

  std::span<Sample> GetWriteView() noexcept {
    return Buffer::GetWriteView(kMonoChannel);
  }

  static MonoBuffer Unary(const Sample sample) noexcept {
    MonoBuffer output(1);
    output.SetSample(0, sample);
    return output;
  }
};

class StereoBuffer : public Buffer {
 public:
  StereoBuffer(const Int num_samples) noexcept : Buffer(2, num_samples) {}

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

  std::span<const Sample> GetLeftReadView() const noexcept {
    return Buffer::GetReadView(kLeftChannel);
  }

  std::span<const Sample> GetRightReadView() const noexcept {
    return Buffer::GetReadView(kRightChannel);
  }

  std::span<Sample> GetLeftWriteView() noexcept {
    return Buffer::GetWriteView(kLeftChannel);
  }

  std::span<Sample> GetRightWriteView() noexcept {
    return Buffer::GetWriteView(kRightChannel);
  }

};

enum class HoaOrdering { Fuma, Acn };

class HoaBuffer : public Buffer {
 private:
  HoaOrdering ordering_;

 public:
  HoaBuffer(const Int max_degree, const Int num_samples,
            const HoaOrdering ordering = HoaOrdering::Acn)
      : Buffer(GetNumChannels(max_degree), num_samples), ordering_(ordering) {}

  inline void SetSample(const Int order, const Int degree, const Int sample_id,
                        const Sample& sample_value) noexcept {
    Buffer::SetSample(GetChannelId(order, degree, ordering_), sample_id,
                      sample_value);
  }


  inline Sample GetSample(const Int order, const Int degree,
                          const Int sample_id) const noexcept {
    return Buffer::GetSample(GetChannelId(order, degree, ordering_), sample_id);
  }

  static Int GetChannelId(const Int order, const Int degree,
                          const HoaOrdering ordering) {
    ASSERT(order >= 0);
    ASSERT(degree <= std::abs(order));

    switch (ordering) {
      case HoaOrdering::Acn: {
        return order * order + order + degree;
      }
      case HoaOrdering::Fuma: {
        // see https://en.wikipedia.org/wiki/Ambisonic_data_exchange_format
        if (order == 0) {
          return 0;
        } else if (order == 1) {
          if (degree == 1) {
            return 1;
          } else if (degree == -1) {
            return 2;
          } else if (degree == 0) {
            return 3;
          }
        } else {
          const Int center_index = order * order;
          Int channel_id = center_index;
          Int this_degree = 0;
          while (this_degree != degree) {
            this_degree =
                (this_degree > 0) ? -this_degree : abs(this_degree) + 1;
            channel_id++;
          }
          return channel_id;
        }

        break;
      }
      default: {
        ASSERT(false);
        return 0;
      }
    }
    ASSERT(false);
    return 0;
  }

  static Int GetNumChannels(const Int max_order) {
    ASSERT(max_order > 0);
    return (max_order + 1) * (max_order + 1);  // (N+1)^2
  }
};

}  // namespace sal

#endif
