/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <functional>
#include <numeric>
#include <vector>

#include "firfilter.h"
#include "mcltypes.h"
#include "vectorop.h"

#if defined(SAL_DSP_APPLE_ACCELERATE)
#include <Accelerate/Accelerate.h>
#elif defined(SAL_DSP_AVX_ACCELERATE)
#include <immintrin.h>
#include <pmmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef SAL_DSP_NEON_ACCELERATE
#include "arm_neon.h"
#endif

#ifdef SAL_DSP_ENVWINDOWS
#define ALIGNED(n) __declspec(align(n))
#else
#define ALIGNED(n) __attribute__((aligned(n)))
#endif

namespace sal {

namespace dsp {

std::vector<Real> FirFilter::impulse_response() noexcept {
  return std::vector<Real>(impulse_response_.begin(), impulse_response_.end());
}

FirFilter::FirFilter(std::vector<Real> B,
                     const size_t max_input_length) noexcept
    : impulse_response_(B),
      impulse_response_old_(B),
      update_index_(0),
      update_length_(0),
      updating_(false),
      coefficients_(B),
      counter_(B.size() - 1),
      length_(B.size()),
      temp_buffer_(std::vector(max_input_length + B.size(), 0.0)) {
  delay_line_.assign(length_, 0.0);
}

Real FirFilter::ProcessSample(Real input_sample) noexcept {
  if (updating_) {
    UpdateCoefficients();
  }
  if (length_ == 1) {
    delay_line_[0] = input_sample;
    return input_sample * coefficients_[0];
  }
#ifdef SAL_DSP_APPLE_ACCELERATE
  return ProcessSampleAppleDsp(input_sample);
#else
  return FilterStraight(input_sample);
#endif
}

void FirFilter::ProcessBlock(std::span<const Real> input_data,
                             std::span<Real> output_data) noexcept {
  ASSERT(output_data.size() >= input_data.size());
  size_t num_samples = input_data.size();
  if (updating_) {
    UpdateCoefficients();
  }
  if (length_ == 1) {
    delay_line_[0] = input_data[num_samples - 1];
    dsp::Multiply(input_data, coefficients_[0], output_data);
    return;
  }
#if defined(SAL_DSP_APPLE_ACCELERATE)
  ProcessBlockAppleDsp(input_data, output_data);
#elif defined(SAL_DSP_AVX_ACCELERATE) || defined(SAL_DSP_NEON_ACCELERATE)
  if (num_samples < length_ ||
      (num_samples + length_ - 1) > SAL_DSP_MAX_VLA_LENGTH) {
    ProcessBlockSerial(input_data, output_data);
    return;
  } else {
#if defined(SAL_DSP_ENVWINDOWS)  // defined(SAL_DSP_AVX_ACCELERATE)
    // Some Intel CPUs do not support AVX instructions.
    // Here we check whether they AVX supported or not, and in that case
    // we filter serially.
    if (!RuntimeArchInfo::GetInstance().IsAvxSupported()) {
      ProcessBlockSerial(input_data, output_data);
      return;
    }
#endif

    SAL_DSP_STACK_ALLOCATE(
        float, extended_input_data,
        num_samples + length_ - 1);  // TODO: this does not compile now because
                                     // VLAs are incompatible with std::span.
    SAL_DSP_STACK_ALLOCATE(float, output_data_float, num_samples);
    GetExtendedInput<float>(input_data, extended_input_data);

#ifdef SAL_DSP_AVX_ACCELERATE
    const Int batch_size = 8;
    ALIGNED(16) __m256 input_frame;
    ALIGNED(16) __m256 coefficient;
    ALIGNED(16) __m256 accumulator;

    for (Int n = 0; (n + batch_size) <= num_samples; n += batch_size) {
      accumulator = _mm256_setzero_ps();
      for (Int k = 0; k < length_; k++) {
        coefficient = _mm256_set1_ps((float)coefficients_[length_ - k - 1]);
        input_frame = _mm256_loadu_ps(extended_input_data + n + k);
        accumulator =
            _mm256_add_ps(_mm256_mul_ps(coefficient, input_frame), accumulator);
        // accumulator = _mm256_fmadd_ps(coefficient, input_frame, accumulator);
      }
      _mm256_storeu_ps(output_data_float + n, accumulator);
    }
#else  // SAL_DSP_NEON_ACCELERATE
    const Int batch_size = 4;
    float32x4_t input_frame;
    float32x4_t coefficient;
    float32x4_t accumulator;

    for (Int n = 0; (n + batch_size) <= num_samples; n += batch_size) {
      accumulator = vdupq_n_f32(0.0);
      for (Int k = 0; k < length_; k++) {
        coefficient = vdupq_n_f32((float)coefficients_[length_ - k - 1]);
        input_frame = vld1q_f32(extended_input_data + n + k);
        accumulator = vmlaq_f32(accumulator, coefficient, input_frame);
      }
      vst1q_f32(output_data_float + n, accumulator);
    }
#endif

    const Int num_samples_left = num_samples % batch_size;
    const Int num_samples_completed = num_samples - num_samples_left;

    for (Int n = 0; n < num_samples_completed; ++n) {
      output_data[n] = (Real)output_data_float[n];
    }

    for (Int n = num_samples_completed; n < num_samples; ++n) {
      output_data[n] = 0.0;
      for (Int p = 0; p < length_; ++p) {
        output_data[n] +=
            coefficients_[length_ - p - 1] * extended_input_data[n + p];
      }
    }

    // Reorganise state for the next run
    for (Int i = 0; i < length_; ++i) {
      delay_line_[i] = input_data[num_samples - 1 - i];
    }
    counter_ = length_ - 1;
  }
#else  // defined(SAL_DSP_NO_ACCELERATE)
  FilterSerial(input_data, output_data);
#endif
}

#ifdef _MSC_VER  // If compiling under Visual Studio
#define UNLIKELY(x) \
  x  // Visual studio does not seem to support __builtin_expect, so bypass
#else
#define UNLIKELY(x) __builtin_expect(x, false)
#endif

Real FirFilter::ProcessSampleStraight(Real input_sample) noexcept {
  delay_line_[counter_] = input_sample;
  Real result =
      std::inner_product(delay_line_.begin() + counter_, delay_line_.end(),
                         coefficients_.begin(), 0.0);
  result = std::inner_product(coefficients_.begin() + length_ - counter_,
                              coefficients_.end(), delay_line_.begin(), result);

  if (UNLIKELY(--counter_ < 0)) {
    counter_ = length_ - 1;
  }

  return result;
}

#ifdef SAL_DSP_APPLE_ACCELERATE
Real FirFilter::ProcessSampleAppleDsp(Real input_sample) noexcept {
  if (length_ - counter_ > SAL_DSP_MAX_VLA_LENGTH) {
    return ProcessSampleStraight(input_sample);
  }

  delay_line_[counter_] = input_sample;
  Real result = 0.0;

  ASSERT(temp_buffer_.size() >= length_ - counter_);
  Multiply(coefficients_,
           std::span(delay_line_.begin() + counter_, length_ - counter_),
           temp_buffer_);

  for (size_t i = 0; i < length_ - counter_; i++) {
    result += temp_buffer_[i];
  }

  if (counter_ > 0) {
    ASSERT(temp_buffer_.size() >= (size_t)counter_);
    Multiply(std::span(coefficients_.begin() + length_ - counter_, counter_),
             delay_line_, temp_buffer_);

    for (size_t i = 0; i < (size_t)counter_; i++) {
      result += temp_buffer_[i];
    }
  }

  if (--counter_ < 0) {
    counter_ = length_ - 1;
  }

  return result;
}

void FirFilter::ProcessBlockAppleDsp(std::span<const Real> input_data,
                                     std::span<Real> output_data) noexcept {
  size_t num_samples = input_data.size();
  if (num_samples < length_ ||
      (num_samples + length_ - 1) > SAL_DSP_MAX_VLA_LENGTH) {
    ProcessBlockSerial(input_data, output_data);
    return;
  }

  ASSERT(temp_buffer_.size() >= num_samples + length_ - 1);
  GetExtendedInput(input_data, temp_buffer_);

#if SAL_DSP_DATA_TYPE_DOUBLE
  vDSP_convD(temp_buffer_.data(), 1, coefficients_.data() + length_ - 1, -1,
             output_data.data(), 1, num_samples, length_);
#else  // Type is float
  vDSP_conv(temp_buffer_.data(), 1, coefficients_.data() + length_ - 1, -1,
            output_data.data(), 1, num_samples, length_);
#endif

  // Reorganise state for the next run
  for (size_t i = 0; i < length_; ++i) {
    delay_line_[i] = input_data[num_samples - 1 - i];
  }

  counter_ = length_ - 1;
}
#endif

FirFilter FirFilter::GainFilter(Real gain) noexcept {
  std::vector<Real> B(1);
  B[0] = 1.0 * gain;

  return FirFilter(B);
}

void FirFilter::Reset() noexcept {
  delay_line_ = Zeros<Real>(delay_line_.size());
}

void FirFilter::SetImpulseResponse(const std::vector<Real>& impulse_response,
                                   const Int update_length) noexcept {
  if (dsp::IsEqual(impulse_response, impulse_response_)) {
    return;
  }

  if (impulse_response.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = impulse_response.size();
    delay_line_.assign(length_, 0.0);
    counter_ = length_ - 1;
    impulse_response_ = impulse_response;
    impulse_response_old_ = impulse_response;
    coefficients_ = impulse_response;
    updating_ = false;
    update_length_ = 0;
    update_index_ = 0;
  } else {
    updating_ = true;
    update_length_ = update_length;
    update_index_ = 0;
    impulse_response_ = impulse_response;

    if (!updating_) {  // If there is no update being carried out
      impulse_response_old_ = impulse_response_;
    } else {
      impulse_response_old_ = coefficients_;
    }
  }
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
}

void FirFilter::UpdateCoefficients() noexcept {
  ASSERT(update_index_ >= 0 && update_index_ <= update_length_);
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
  ASSERT(impulse_response_.size() == coefficients_.size());
  Real weight_new = ((Real)update_index_ + 1) / ((Real)update_length_ + 1);
  Real weight_old = 1.0f - weight_new;
  Multiply(impulse_response_, weight_new, coefficients_);
  MultiplyAdd(impulse_response_old_, weight_old, coefficients_, coefficients_);
  // The above is a lock-free equivalent version to
  // coefficients_ = dsp::Add(dsp::Multiply(impulse_response_, weight_new),
  //                          dsp::Multiply(impulse_response_old_, weight_old));

  if (update_index_ == update_length_) {
    updating_ = false;
  } else {
    update_index_++;
  }
}

} // namespace dsp

} // namespace sal
