/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */


namespace sal
{
template<typename T>
DelayFilter<T>::DelayFilter(
  const size_t latency,
  const size_t max_latency) noexcept
  : latency_(std::numeric_limits<size_t>::max())
  , data_(max_latency+1, T(0.0))
  , write_index_(0)
{
  ASSERT_WITH_MESSAGE(latency >= 0, "The latency cannot be nagative.");
  ASSERT_WITH_MESSAGE(
    max_latency >= 0,
    "The maximum latency cannot be nagative.");

  this->SetLatency(latency);
}


template<typename T>
void DelayFilter<T>::Write(
  const T sample) noexcept
{
  data_[write_index_] = sample;
}


template<typename T>
void DelayFilter<T>::Write(
  const mcl::Vector<T>& input) noexcept
{
  const size_t num_samples = input.size();
  ASSERT(num_samples >= 0);
  if (num_samples > (max_latency() - latency_ + 1))
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Writing more samples (%d) than max_latency-latency+1 (%d)."
        "This operation will go ahead, but some samples will be "
        "overwritten. ",
        num_samples,
        max_latency() - latency_ + 1);
  }

  size_t write_index = write_index_;
  for (size_t i = 0; i < num_samples; ++i)
  {
    ASSERT(write_index >= 0 && write_index <= max_latency());
    data_[write_index++] = input[i];
    if (write_index > max_latency())
    {
      write_index = 0;
    }
  }
}


template<typename T>
void DelayFilter<T>::ResetState() noexcept
{
  mcl::SetToZero(data_);
}


template<typename T>
inline T DelayFilter<T>::Read() const noexcept
{
  return data_[read_index_];
}


template<typename T>
T DelayFilter<T>::ReadAt(
  const size_t delay_tap) const noexcept
{
#ifndef NOLOGGING
  if (delay_tap > max_latency())
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Trying to read at a delay tap (%d) larger than the maximum latency "
        "of the delay line (%d). Giving back the value at the maximum "
        "latency instead. ",
        delay_tap,
        max_latency());
  }
#endif

  ASSERT(write_index_ >= 0 && write_index_ <= max_latency());
  size_t sanitised_delay_tap = std::min(delay_tap, max_latency());
  return (write_index_ >= sanitised_delay_tap) ?
           data_[write_index_ - sanitised_delay_tap] :
           data_[write_index_ - sanitised_delay_tap + max_latency() + 1];
}


template<typename T>
void DelayFilter<T>::Read(
  mcl::Vector<T>& output) const noexcept
{
  const size_t num_samples = output.size();
  if (num_samples > max_latency())
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Trying to read a number of samples (%d) larger than the latency "
        "of the delay line (%d). This operation will go ahead, but it "
        "means you will be reading samples that haven't been written yet.",
        num_samples,
        latency_);
  }

  size_t read_index = read_index_;
  for (size_t i = 0; i < num_samples; ++i)
  {
    ASSERT(read_index >= 0 && read_index <= max_latency());
    output[i] = data_[read_index++];
    if (read_index > max_latency())
    {
      read_index = 0;
    }
  }
}


template<typename T>
T DelayFilter<T>::FractionalReadAt(
  const Time fractional_delay_tap) const noexcept
{
#ifndef NOLOGGING
  if (fractional_delay_tap >= (Time)max_latency())
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Trying to read at a delay tap (%f) larger than the maximum latency "
        "of the delay line (%d). Giving back the value at the maximum "
        "latency instead. ",
        fractional_delay_tap,
        max_latency());
  }
#endif

  Time sanitised_delay_tap = std::min(
    fractional_delay_tap, (Time)max_latency());
  Int x_a = (Int)sanitised_delay_tap; // Cast to int is equivalent to floor
  Int x_b = x_a + 1;
  T f_x_a = ReadAt(x_a);
  T f_x_b = ReadAt(x_b);
  return (f_x_b - f_x_a) / (x_b - x_a) * (sanitised_delay_tap - x_a) + f_x_a;
}


template<typename T>
void DelayFilter<T>::Tick() noexcept
{
  write_index_ = (write_index_ != max_latency()) ? (write_index_ + 1) : 0;
  read_index_ = (read_index_ != max_latency()) ? (read_index_ + 1) : 0;
}


template<typename T>
void DelayFilter<T>::Tick(
  size_t num_samples) noexcept
{
  ASSERT(num_samples >= 0);
  if (num_samples > max_latency())
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Ticking by more samples (%d) than the max latency of the delay "
        "line (%d). The operation will go ahead, but this implies that "
        "some samples may never be read.",
        num_samples,
        latency_);
  }

  size_t wrapped_num_samples = num_samples % max_latency();
  ASSERT(wrapped_num_samples >= 0 && wrapped_num_samples < max_latency());
  if (write_index_ + wrapped_num_samples <= max_latency())
  {
    write_index_ += wrapped_num_samples;
  }
  else
  {
    write_index_ = 0 + (wrapped_num_samples - (max_latency() - write_index_) - 1);
  }
  if (read_index_ + wrapped_num_samples <= max_latency())
  {
    read_index_ += wrapped_num_samples;
  }
  else
  {
    read_index_ = 0 + (wrapped_num_samples - (max_latency() - read_index_) - 1);
  }
  ASSERT(write_index_ >= 0 && write_index_ <= max_latency());
  ASSERT(read_index_ >= 0 && read_index_ <= max_latency());
}


template<typename T>
void DelayFilter<T>::SetLatency(
  size_t latency) noexcept
{
  if (latency_ == latency)
  {
    return;
  }
  if (latency > max_latency())
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Trying to set a delay filter latency (%d) larger than "
        "the maximum latency (%d). The latency will be set to the "
        "the maximum latency instead. ",
        latency,
        max_latency());
  }
  latency_ = std::min(latency, max_latency());
  read_index_ = (write_index_ >= latency_)
    ? write_index_ - latency_
    : write_index_ - latency_ + max_latency() + 1;
  ASSERT((read_index_ >= 0) & (read_index_ <= max_latency()));
}


template<typename T>
size_t DelayFilter<T>::latency() const noexcept
{
  return latency_;
}


template<typename T>
size_t DelayFilter<T>::max_latency() const noexcept
{
  return data_.size()-1;
}


template<typename T>
void DelayFilter<T>::Filter(
  const mcl::Vector<T>& input,
  mcl::Vector<T>& output) noexcept
{
  ASSERT(input.size() == output.size());
  for (size_t i = 0; i < input.size(); ++i)
  {
    Write(input[i]);
    output[i] = Read();
    Tick();
  }
}

} // namespace sal
