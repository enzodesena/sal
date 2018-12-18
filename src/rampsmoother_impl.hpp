/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


namespace sal
{
template<typename T>
RampSmoother<T>::RampSmoother(
  const T initial_value,
  const Time sampling_frequency) noexcept
  : current_value_(initial_value)
  , target_value_(initial_value)
  , step_(0.0)
  , countdown_(0)
  , sampling_frequency_(sampling_frequency)
{
  ASSERT_WITH_MESSAGE
  (
    std::isgreaterequal(sampling_frequency, 0.0),
    "Sampling frequency cannot be negative ");
}


template<typename T>
T RampSmoother<T>::GetNextValue() noexcept
{
  if (countdown_ <= 0)
  {
    return target_value_;
  }
  else
  {
    --countdown_;
    current_value_ += step_;
    return current_value_;
  }
}


template<typename T>
T RampSmoother<T>::GetNextValue(
  const size_t num_jumps) noexcept
{
  // The +1 below is to make this identical to GetNextValue()
  if ((countdown_ - num_jumps + 1) <= 0)
  {
    countdown_ = 0;
    return target_value_;
  }
  else
  {
    countdown_ -= num_jumps;
    current_value_ += step_ * ((T)num_jumps);
    return current_value_;
  }
}


template<typename T>
void RampSmoother<T>::GetNextValuesMultiply(
  const mcl::Vector<T>& input,
  mcl::Vector<T>& output) noexcept
{
  ASSERT(input.size() == output.size());
  auto input_iter = input.begin();
  auto output_iter = output.begin();
  if (IsUpdating())
  {
    while (input_iter != input.end())
    {
      *output_iter++ = *input_iter++ * GetNextValue();
    }
  }
  else
  {
    mcl::Multiply
    (
      input_iter,
      input.end(),
      target_value_,
      output_iter);
  }
}


template<typename T>
void RampSmoother<T>::GetNextValuesMultiplyAdd(
  const T* input_data,
  const Int num_samples,
  T* input_output_data) noexcept
{
  if (IsUpdating())
  {
    for (Int i = 0; i < num_samples; ++i)
    {
      input_output_data[i] += input_data[i] * GetNextValue();
    }
  }
  else
  {
    for (Int i = 0; i < num_samples; ++i)
    {
      input_output_data[i] += input_data[i] * target_value_;
    }
  }
}


template<typename T>
T RampSmoother<T>::target_value() const noexcept
{
  return target_value_;
}


template<typename T>
void RampSmoother<T>::SetTargetValue(
  const T target_value,
  const Time ramp_time) noexcept
{
  ASSERT_WITH_MESSAGE
  (
    std::isgreaterequal(ramp_time, 0.0),
    "Ramp time cannot be negative ");
  if ((mcl::RoundToInt(ramp_time * sampling_frequency_)) == 0)
  {
    target_value_ = target_value;
    current_value_ = target_value;
    countdown_ = 0;
    return;
  }

  if (std::islessgreater(target_value, target_value_))
  {
    const Int num_update_samples = mcl::RoundToInt(
      ramp_time * sampling_frequency_);
    countdown_ = num_update_samples;
    target_value_ = target_value;

    if (num_update_samples == 0)
    {
      current_value_ = target_value;
    }
    else
    {
      step_ = (target_value_ - current_value_) /
        ((T)num_update_samples);
    }
  }
}


template<typename T>
bool RampSmoother<T>::IsUpdating() const noexcept
{
  return countdown_ > 0;
}

} // namespace sal
