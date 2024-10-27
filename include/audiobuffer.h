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
    for (size_t chan_id = 0; chan_id < num_channels; ++chan_id) {
      data_.push_back(std::vector<Sample>(num_samples));
    }
  }

  Buffer() : Buffer(0, 0) {}

  virtual size_t num_channels() const noexcept { return data_.size(); }

  virtual size_t num_samples() const noexcept {
    return (num_channels() > 0) ? data_[0].size() : 0;
  }

  inline Sample GetSample(size_t channel_id, size_t sample_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    ASSERT(sample_id >= 0 && sample_id < num_samples());

    return data_[channel_id][sample_id];
  }

  inline void SetSample(const size_t channel_id, const size_t sample_id,
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
  void SetSamples(const size_t channel_id, const size_t from_sample_id,
                  const size_t num_samples, const Sample* samples) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());

    ASSERT(from_sample_id >= 0);
    ASSERT(num_samples >= 0);
    ASSERT((from_sample_id + num_samples) <= this->num_samples());

    for (size_t sample_id = from_sample_id;
         sample_id < (from_sample_id + num_samples); ++sample_id) {
      data_[channel_id][sample_id] = samples[sample_id - from_sample_id];
    }
  }

  void SetSamples(const Buffer& other) noexcept {
    ASSERT(num_samples() == other.num_samples());
    ASSERT(num_channels() == other.num_channels());
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        data_[chan_id][sample_id] = other.data_[chan_id][sample_id];
      }
    }
  }

  std::span<const Sample> GetReadView(const size_t channel_id) const noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return std::span<const Sample>(data_[channel_id]);
  }

  std::span<Sample> GetWriteView(const size_t channel_id) noexcept {
    ASSERT(channel_id >= 0 && channel_id < num_channels());
    return data_[channel_id];
  }

  void PrintData() {
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        std::cout << data_[chan_id][sample_id] << " ";
      }
      std::cout << std::endl;
    }
  }

  /** Resets all the values to zero. */
  virtual void Reset() noexcept {
    for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id) {
      for (size_t sample_id = 0; sample_id < num_samples(); ++sample_id) {
        data_[chan_id][sample_id] = 0.0;
      }
    }
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
  std::vector<std::vector<Sample> > data_;
  std::vector<Sample> temporary_vector_;  // Support vector for filter operation
};

//
//  /** Adds samples to current sample values in the buffer.
//
//   @param[in] channel_id The ID of the channel.
//   @param[in] from_sample_id The index of the first sample we want to modify.
//   @param[in] num_samples The number of samples we want to modify.
//   @param[in] samples The new samples.
//   */
//  void AddSamples(const Int channel_id,
//                  const Int from_sample_id,
//                  std::span<Sample> samples) noexcept {
//    ASSERT(channel_id >= 0 && channel_id < num_channels());
//    ASSERT(from_sample_id >= 0);
//    ASSERT((from_sample_id+samples.size()) <= (size_t) num_samples_);
//
//    dsp::Add(samples,
//             std::span<Sample>(data_[channel_id].begin() + from_sample_id,
//             samples.size()), std::span<Sample>(data_[channel_id].begin() +
//             from_sample_id, samples.size()));
//  }
//
//  /** This method first multiplies all the input samples by a certain constant
//   and then adds the result to the samples in the buffer. */
//  void MultiplyAddSamples(const Int channel_id,
//                          const Int from_sample_id,
//                          const Int num_samples,
//                          const Sample* samples,
//                          const Sample constant) noexcept {
//    ASSERT(channel_id >= 0 && channel_id < num_channels());
//    ASSERT(from_sample_id >= 0);
//    ASSERT(num_samples >= 0);
//    ASSERT((from_sample_id+num_samples) <= num_samples_);
//    dsp::MultiplyAdd(samples, constant, &(data_[channel_id][from_sample_id]),
//                     num_samples, &(data_[channel_id][from_sample_id]));
//  }
//
//  void FilterAddSamples(const Int channel_id,
//                        const Int from_sample_id,
//                        const Int num_samples,
//                        const Sample* samples,
//                        dsp::Filter& filter) noexcept {
//    ASSERT(channel_id >= 0 && channel_id < num_channels());
//    ASSERT(from_sample_id >= 0);
//    ASSERT(num_samples >= 0);
//    ASSERT((from_sample_id+num_samples) <= num_samples_);
//    filter.ProcessBlock(samples, temporary_vector_.data());
//    dsp::Add(temporary_vector_.data(),
//             &(data_[channel_id][from_sample_id]), num_samples,
//             &(data_[channel_id][from_sample_id]));
//  }
//  Sample** GetWritePointers() noexcept { return data_; }

//  /** Adds all the samples from another buffer. The buffer has to be of the
//   same type and have the same number of channels
//   and samples (checked only through debugging asserts).
//   */
//  virtual void AddSamples(const Buffer& buffer) noexcept {
//    ASSERT(num_channels() == buffer.num_channels());
//    ASSERT(num_samples() == buffer.num_samples());
//
//    for (Int chan_id = 0; chan_id<num_channels(); ++chan_id) {
//      dsp::Add(GetReadView(chan_id),
//               buffer.GetReadView(chan_id),
//               num_samples(),
//               GetWriteView(chan_id));
//    }
//  }

//  void SetFrame(const Int channel_id,
//                const Int frame_id,
//                const Int frame_length,
//                const Signal& signal) {
//    for (dsp::Int n=0; n<num_samples(); ++n) {
//      dsp::Int index = frame_id*frame_length + n;
//      if (index < (dsp::Int) signal.size()) {
//        SetSample(channel_id, n, signal[index]);
//      } else {
//        SetSample(channel_id, n, 0.0);
//      }
//    }
//  }

class MonoBuffer : public Buffer {
 public:
  explicit MonoBuffer(const Int num_samples) noexcept
      : Buffer(1, num_samples) {}

  //  /** This first multiplies all the input samples by a certain constant
  //   and then adds the result to the samples in the buffer. */
  //  void MultiplyAddSamples(const Int from_sample_id,
  //                          const Int num_samples,
  //                          const Sample* samples,
  //                          const Sample constant) noexcept {
  //    Buffer::MultiplyAddSamples(kMonoChannel, from_sample_id, num_samples,
  //                               samples, constant);
  //  }

  inline void SetSample(const Int sample_id,
                        const Sample sample_value) noexcept {
    Buffer::SetSample(kMonoChannel, sample_id, sample_value);
  }

  using Buffer::SetSamples;

  void SetSamples(const Int from_sample_id, const Int num_samples,
                  const Sample* samples) noexcept {
    Buffer::SetSamples(kMonoChannel, from_sample_id, num_samples, samples);
  }

  //  void SetFrame(const Int frame_id,
  //                const Int frame_length,
  //                const Signal& signal) {
  //    Buffer::SetFrame(0, frame_id, frame_length, signal);
  //  }

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

  //  using Buffer::AddSamples;
  //
  //  void AddSamples(const Int from_sample_id,
  //                  const Int num_samples,
  //                  const Sample* samples) noexcept {
  //    Buffer::AddSamples(kMonoChannel, from_sample_id, num_samples,
  //                                   samples);
  //  }
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

  //  void AddSamplesLeft(const Sample* samples,
  //                      const Int from_sample_id,
  //                      const Int num_samples_to_add) noexcept {
  //    Buffer::AddSamples(kLeftChannel, from_sample_id,
  //                                   num_samples_to_add, samples);
  //  }
  //
  //  void FilterAddSamplesLeft(const Int from_sample_id,
  //                            const Int num_samples,
  //                            const Sample* samples,
  //                            dsp::Filter& filter) noexcept {
  //    Buffer::FilterAddSamples(kLeftChannel, from_sample_id,
  //                                         num_samples, samples, filter);
  //  }
  //
  //  void FilterAddSamplesRight(const Int from_sample_id,
  //                             const Int num_samples,
  //                             const Sample* samples,
  //                             dsp::Filter& filter) noexcept {
  //    Buffer::FilterAddSamples(kRightChannel, from_sample_id,
  //                                         num_samples, samples, filter);
  //  }
  //
  //  void AddSamplesRight(const Sample* samples,
  //                       const Int from_sample_id,
  //                       const Int num_samples_to_add) noexcept {
  //    Buffer::AddSamples(kRightChannel, from_sample_id,
  //                                   num_samples_to_add, samples);
  //  }
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

  //  using Buffer::AddSamples;

  //  void AddSamples(const Int order, const Int degree,
  //                  const Int from_sample_id,
  //                  const Int num_samples,
  //                  const Sample* samples) {
  //    Buffer::AddSamples(GetChannelId(order, degree, ordering_),
  //                                   from_sample_id, num_samples, samples);
  //  }
  //
  //  /** This first multiplies all the input samples by a certain constant
  //   and then adds the result to the samples in the buffer. */
  //  void MultiplyAddSamples(const Int order, const Int degree,
  //                          const Int from_sample_id,
  //                          const Int num_samples,
  //                          const Sample* samples,
  //                          const Sample constant) noexcept {
  //    Buffer::MultiplyAddSamples(GetChannelId(order, degree, ordering_),
  //                               from_sample_id, num_samples,
  //                               samples, constant);
  //  }

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
