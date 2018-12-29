/*
Spatial Audio Library (SAL)
Copyright (c) 2012, Enzo De Sena
All rights reserved.

Authors: Enzo De Sena, enzodesena@gmail.com

*/


namespace sal
{
template<typename T>
void Buffer<T>::AllocateMemory()
{
  data_ = mcl::Vector<mcl::Vector<T>>(num_channels_);
  for (size_t chan_id = 0; chan_id < num_channels_; ++chan_id)
  {
    data_[chan_id] = mcl::Vector<T>(num_samples_);
  }
}


template<typename T>
Buffer<T>::Buffer(
  const size_t num_channels,
  const size_t num_samples)
  : num_channels_(num_channels)
  , num_samples_(num_samples)
  , temporary_vector_(mcl::Vector<T>(num_samples, 0.0))
{
  ASSERT(num_channels >= 0 && num_samples >= 0);
  AllocateMemory();
}


template<typename T>
Buffer<T>::Buffer()
  : Buffer(1, 0)
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

template<typename T>
size_t Buffer<T>::num_channels() const noexcept
{
  return num_channels_;
}


template<typename T>
size_t Buffer<T>::num_samples() const noexcept
{
  return num_samples_;
}


template<typename T>
T Buffer<T>::GetSample(
  size_t channel_id,
  size_t sample_id) const noexcept
{
  return data_[channel_id][sample_id];
}


template<typename T>
bool Buffer<T>::OwnsData() const noexcept
{
  return owns_data_;
}


template<typename T>
void Buffer<T>::SetSample(
  const size_t channel_id,
  const size_t sample_id,
  const T sample) noexcept
{
  data_[channel_id][sample_id] = sample;
}


template<typename T>
void Buffer<T>::SetSamples(
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


template<typename T>
void Buffer<T>::SetSamples(
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


template<typename T>
typename mcl::Vector<T>::ConstIterator Buffer<T>::begin(
  const size_t channel_id) const noexcept
{
  return data_[channel_id].begin();
}


template<typename T>
typename mcl::Vector<T>::Iterator Buffer<T>::begin(
  const size_t channel_id) noexcept
{
  return data_[channel_id].begin();
}


template<typename T>
typename mcl::Vector<T>::ConstIterator Buffer<T>::end(
  const size_t channel_id) const noexcept
{
  return data_[channel_id].end();
}


template<typename T>
typename mcl::Vector<T>::Iterator Buffer<T>::end(
  const size_t channel_id) noexcept
{
  return data_[channel_id].end();
}


template<typename T>
mcl::Vector<T> Buffer<T>::GetChannelReference(
  const size_t channel_id) noexcept
{
  return std::move(MakeReference(data_[channel_id]));
}


template<typename T>
mcl::Vector<T> Buffer<T>::GetChannelConstReference(
  const size_t channel_id) const noexcept
{
  return std::move(MakeConstReference(data_[channel_id]));
}


template<typename T>
void Buffer<T>::AddSamples(
  const Buffer& other_buffer) noexcept
{
  ASSERT(num_channels() == other_buffer.num_channels());
  ASSERT(num_samples() == other_buffer.num_samples());

  for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id)
  {
    mcl::Add
    (
      begin(chan_id),
      end(chan_id),
      other_buffer.begin(chan_id),
      begin(chan_id));
  }
}


template<typename T>
void Buffer<T>::PrintData()
{
  for (auto& vector : data_)
  {
    for (auto& sample : vector)
    {
      std::cout << sample << " ";
    }
    std::cout << std::endl;
  }
}


template<typename T>
void Buffer<T>::ResetSamples() noexcept
{
  for (auto& vector : data_)
  {
    SetToZero(vector);
  }
}


template<typename T>
Buffer<T>::Buffer(
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


template<typename T>
Buffer<T>& Buffer<T>::operator=(
  const Buffer<T>& other)
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

template<typename T>
MonoBuffer<T>::MonoBuffer(
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


template<typename T>
void MonoBuffer<T>::SetSample(
  const size_t sample_id,
  const T sample_value) noexcept
{
  Buffer<T>::SetSample(Channel::kMono, sample_id, sample_value);
}


template<typename T>
void MonoBuffer<T>::SetSamples(
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


template<typename T>
T MonoBuffer<T>::GetSample(
  const size_t sample_id) const noexcept
{
  return Buffer<T>::GetSample(Channel::kMono, sample_id);
}


template<typename T>
MonoBuffer<T> MonoBuffer<T>::Unary(
  const T sample) noexcept
{
  MonoBuffer output(1);
  output.SetSample(0, sample);
  return output;
}


template<typename T>
StereoBuffer<T>::StereoBuffer(
  const size_t num_samples) noexcept
  : Buffer<T>(2, num_samples)
{
}


template<typename T>
void StereoBuffer<T>::SetLeftSample(
  const size_t sample_id,
  const T sample_value) noexcept
{
  Buffer<T>::SetSample(Channel::kLeft, sample_id, sample_value);
}


template<typename T>
void StereoBuffer<T>::SetRightSample(
  const size_t sample_id,
  const T sample_value) noexcept
{
  Buffer<T>::SetSample(Channel::kRight, sample_id, sample_value);
}


template<typename T>
T StereoBuffer<T>::GetLeftSample(
  const size_t sample_id) const noexcept
{
  return Buffer<T>::GetSample(Channel::kLeft, sample_id);
}


template<typename T>
T StereoBuffer<T>::GetRightSample(
  const size_t sample_id) const noexcept
{
  return Buffer<T>::GetSample(Channel::kRight, sample_id);
}


template<typename T>
BFormatBuffer<T>::BFormatBuffer(
  const size_t max_degree,
  const size_t num_samples)
  : Buffer<T>(GetNumChannels(max_degree), num_samples)
{
}


template<typename T>
void BFormatBuffer<T>::SetSample(
  const Int degree,
  const size_t order,
  const size_t sample_id,
  const T& sample_value) noexcept
{
  Buffer<T>::SetSample(GetChannelId(degree, order), sample_id, sample_value);
}


template<typename T>
void BFormatBuffer<T>::AddSamples(
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


template<typename T>
T BFormatBuffer<T>::GetSample(
  const Int degree,
  const size_t order,
  const size_t sample_id) const noexcept
{
  return Buffer<T>::GetSample(GetChannelId(degree, order), sample_id);
}




} // namespace sal
