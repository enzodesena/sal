/*
Spatial Audio Library (SAL)
Copyright (c) 2012, Enzo De Sena
All rights reserved.

Authors: Enzo De Sena, enzodesena@gmail.com

*/


namespace sal
{
template<typename T>
Buffer<T>::Buffer(
  const size_t num_channels,
  const size_t num_samples)
  : data_(mcl::Vector<mcl::Vector<T>>(num_channels))
{
  ASSERT(num_channels >= 1 && num_samples >= 0);
  for (size_t chan_id = 0; chan_id < num_channels; ++chan_id)
  {
    data_[chan_id] = mcl::Vector<T>(num_samples);
  }
}


template<typename T>
Buffer<T>::Buffer() noexcept
  : Buffer<T>(1, 0)
{
}


template<typename T>
size_t Buffer<T>::num_channels() const noexcept
{
  return data_.size();
}


template<typename T>
size_t Buffer<T>::num_samples() const noexcept
{
  // The minimum number of channels is 1 so the following will never throw
  return data_[0].size();
}


template<typename T>
mcl::Vector<T>& Buffer<T>::operator[](
  const size_t index) noexcept
{
  return data_[index];
}


template<typename T>
const mcl::Vector<T>& Buffer<T>::operator[](
  const size_t index) const noexcept
{
  return data_[index];
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
  return data_[0].OwnsData(); // If one channel owns data then all others do
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
  ASSERT(num_samples() == other.num_samples());
  ASSERT(num_channels() == other.num_channels());
  for (size_t chan_id = 0; chan_id < num_channels(); ++chan_id)
  {
    std::copy
    (
      other.data_[chan_id].begin(),
      other.data_[chan_id].end(),
      data_[chan_id].begin());
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
void Buffer<T>::PrintData() noexcept
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
  Buffer<T>& buffer_referenced,
  const size_t first_sample_index,
  const size_t num_sample,
  const size_t first_channel_index,
  const size_t num_channels)
  : data_(mcl::Vector<mcl::Vector<T>>(num_channels))
{
  // TODO: this is not lock-free as we are still allocating a vector of refs
  for (size_t i=0; i<num_channels; ++i)
  {
    mcl::Vector vec = buffer_referenced.GetChannelReference(i+first_channel_index);
    data_[i] = MakeReference
    (
      vec,
      first_sample_index,
      num_sample);
  }
}


template<typename T>
Buffer<T>::Buffer(
  mcl::Vector<mcl::Vector<T>>& data)
  : data_(mcl::Vector<mcl::Vector<T>>(data.size()))
{
  ASSERT(data.size() >= 1);
  const size_t num_samples = data[0].size();
  // TODO: this is not lock-free as we are still allocating a vector of refs
  for (size_t i=0; i<data.size(); ++i)
  {
    ASSERT(data[i].size() == num_samples);
    data_[i] = MakeReference(data[i], 0, num_samples);
  }
}


} // namespace sal
