/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

namespace sal
{

template<typename T>
class RampSmoother
{
public:
  /**
   @param[in] initial_value The initial assigned value. */
  RampSmoother(
    const T initial_value,
    const Time sampling_frequency) noexcept;

  T GetNextValue() noexcept;

  T GetNextValue(
    const size_t num_jumps) noexcept;


  /** Takes an array of values (`input_data`), multiplies them by the
   next values coming out of the smoother, and writes the result into
   an output array (`output_data`).
   @param[in] input The input vector.
   @param[out] output The output vector (data will be overwritten. */
  void GetNextValuesMultiply(
    const mcl::Vector<T>& input,
    mcl::Vector<T>& output) noexcept;

  /** Takes an array of values (`input_data`), multiplies them by the
   next values coming out of the smoother, and adds the result to
   an input-output array (`input_output_data`).
   @param[in] input_data The input data to multiply by.
   @param[in,out] input_output_data The data onto which we will add the result
   of the multiplication. */
  void GetNextValuesMultiplyAdd(
    const T* input_data,
    const Int num_samples,
    T* input_output_data) noexcept;

  T target_value() const noexcept;

  void SetTargetValue(
    const T target_value,
    const Time ramp_time) noexcept;

  bool IsUpdating() const noexcept;

private:
  T current_value_;
  T target_value_;
  T step_;
  Int countdown_;

  Time sampling_frequency_;
};

} // namespace

#include "rampsmoother_impl.hpp"
