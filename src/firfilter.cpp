/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "firfilter.h"
#include "vectorop.h"
#include "mcltypes.h"
#include <vector>
#include <numeric>
#include <functional>

#if defined(MCL_APPLE_ACCELERATE)
  #include <Accelerate/Accelerate.h>
#elif defined(MCL_AVX_ACCELERATE)
  #include <pmmintrin.h>
  #include <xmmintrin.h>
  #include <immintrin.h>
#endif

#ifdef MCL_NEON_ACCELERATE
  #include "arm_neon.h"
#endif

#ifdef MCL_ENVWINDOWS
  #define ALIGNED(n) __declspec(align(n))
#else
  #define ALIGNED(n) __attribute__ ((aligned (n)))
#endif

namespace mcl {

std::vector<Real> FirFilter::impulse_response() noexcept {
  return std::vector<Real>(impulse_response_.begin(),
                           impulse_response_.end());
}
  
FirFilter::FirFilter() noexcept :
        impulse_response_(mcl::UnaryVector<Real>(1.0)),
        impulse_response_old_(mcl::UnaryVector<Real>(1.0)),
        update_index_(0), update_length_(0), updating_(false),
        coefficients_(mcl::UnaryVector<Real>(1.0)),
        counter_(0), length_(1) {
  delay_line_.assign(1, 0.0);
}
  
FirFilter::FirFilter(std::vector<Real> B) noexcept :
        impulse_response_(B),
        impulse_response_old_(B),
        update_index_(0), update_length_(0), updating_(false),
        coefficients_(B),
        counter_(B.size()-1), length_(B.size()) {
  delay_line_.assign(length_, 0.0);
}

Real FirFilter::ProcessSample(Real input_sample) noexcept {
  if (updating_) { UpdateCoefficients(); }
  if (length_ == 1) {
    delay_line_[0] = input_sample;
    return input_sample*coefficients_[0];
  }
#ifdef MCL_APPLE_ACCELERATE
  return ProcessSampleAppleDsp(input_sample);
#else
  return FilterStraight(input_sample);
#endif
}
  

void FirFilter::ProcessBlock(const Real* __restrict input_data,
                       const Int num_samples,
                       Real* __restrict output_data) noexcept {
  if (updating_) { UpdateCoefficients(); }
  if (length_ == 1) {
    delay_line_[0] = input_data[num_samples-1];
    mcl::Multiply(input_data, num_samples, coefficients_[0], output_data);
    return;
  }
#if defined(MCL_APPLE_ACCELERATE)
  ProcessBlockAppleDsp(input_data, num_samples, output_data);
#elif defined(MCL_AVX_ACCELERATE) || defined(MCL_NEON_ACCELERATE)
  if (num_samples < length_ || (num_samples+length_-1) > MCL_MAX_VLA_LENGTH) {
    FilterSerial(input_data, num_samples, output_data);
    return;
  } else {
#if defined(MCL_ENVWINDOWS) // defined(MCL_AVX_ACCELERATE)
    // Some Intel CPUs do not support AVX instructions.
    // Here we check whether they AVX supported or not, and in that case
    // we filter serially.
    if (! RuntimeArchInfo::GetInstance().IsAvxSupported()) {
      FilterSerial(input_data, num_samples, output_data);
      return;
    }
#endif
    
    MCL_STACK_ALLOCATE(float, extended_input_data, num_samples+length_-1); // TODO: handle stack overflow
    MCL_STACK_ALLOCATE(float, output_data_float, num_samples); // TODO: handle stack overflow
    GetExtendedInput<float>(input_data, num_samples, extended_input_data);
    
#ifdef MCL_AVX_ACCELERATE
    const Int batch_size = 8;
    ALIGNED(16) __m256 input_frame;
    ALIGNED(16) __m256 coefficient;
    ALIGNED(16) __m256 accumulator;
    
    for(Int n=0; (n+batch_size)<=num_samples; n+=batch_size) {
      accumulator = _mm256_setzero_ps();
      for(Int k=0; k<length_; k++) {
        coefficient = _mm256_set1_ps((float) coefficients_[length_ - k - 1]);
        input_frame = _mm256_loadu_ps(extended_input_data + n + k);
        accumulator = _mm256_add_ps(_mm256_mul_ps(coefficient, input_frame), accumulator);
        // accumulator = _mm256_fmadd_ps(coefficient, input_frame, accumulator);
      }
      _mm256_storeu_ps(output_data_float+n, accumulator);
    }
#else // MCL_NEON_ACCELERATE
    const Int batch_size = 4;
    float32x4_t input_frame;
    float32x4_t coefficient;
    float32x4_t accumulator;
    
    for(Int n=0; (n+batch_size)<=num_samples; n+=batch_size) {
      accumulator = vdupq_n_f32(0.0);
      for(Int k=0; k<length_; k++) {
        coefficient = vdupq_n_f32((float) coefficients_[length_ - k - 1]);
        input_frame = vld1q_f32(extended_input_data + n + k);
        accumulator = vmlaq_f32(accumulator, coefficient, input_frame);
      }
      vst1q_f32(output_data_float+n, accumulator);
    }
#endif
    
    const Int num_samples_left = num_samples % batch_size;
    const Int num_samples_completed = num_samples - num_samples_left;
    
    for (Int n=0; n<num_samples_completed; ++n) {
      output_data[n] = (Real) output_data_float[n];
    }
    
    for (Int n=num_samples_completed; n<num_samples; ++n) {
      output_data[n] = 0.0;
      for (Int p=0; p<length_; ++p) {
        output_data[n] += coefficients_[length_-p-1] * extended_input_data[n+p];
      }
    }
    
    // Reorganise state for the next run
    for (Int i=0; i<length_; ++i) {
      delay_line_[i] = input_data[num_samples-1-i];
    }
    counter_ = length_-1;
  }
#else // defined(MCL_NO_ACCELERATE)
  FilterSerial(input_data, num_samples, output_data);
#endif
}
  
#ifdef _MSC_VER // If compiling under Visual Studio
 #define UNLIKELY(x) x // Visual studio does not seem to support __builtin_expect, so bypass
#else
 #define UNLIKELY(x) __builtin_expect(x, false)
#endif

Real FirFilter::ProcessSampleStraight(Real input_sample) noexcept {
  delay_line_[counter_] = input_sample;
  Real result = std::inner_product(delay_line_.begin()+counter_,
                                   delay_line_.end(),
                                   coefficients_.begin(),
                                   0.0);
  result = std::inner_product(coefficients_.begin()+length_-counter_,
                              coefficients_.end(),
                              delay_line_.begin(),
                              result);
  
  if (UNLIKELY(--counter_ < 0)) { counter_ = length_-1; }
  
  return result;
}
  
  
#ifdef MCL_APPLE_ACCELERATE
Real FirFilter::ProcessSampleAppleDsp(Real input_sample) noexcept {
  if (length_-counter_ > MCL_MAX_VLA_LENGTH) {
    return ProcessSampleStraight(input_sample);
  }
  
  delay_line_[counter_] = input_sample;
  Real result = 0.0;
  
  MCL_STACK_ALLOCATE(mcl::Real, result_a, length_-counter_); // TODO: handle stack overflow
  Multiply(&coefficients_[0],
           &delay_line_[counter_],
           length_-counter_, result_a);
  
  for (Int i=0; i<length_-counter_; i++) { result += result_a[i]; }
  
  if (counter_ > 0) {
    Real result_b[counter_];
    Multiply(&coefficients_[length_-counter_],
             &delay_line_[0],
             counter_, result_b);
    
    for (Int i=0; i<counter_; i++) { result += result_b[i]; }
  }
  
  if (--counter_ < 0) { counter_ = length_-1; }
  
  return result;
}
  
void FirFilter::ProcessBlockAppleDsp(const Real* __restrict input_data,
                               const Int num_samples,
                               Real* __restrict output_data) noexcept {
  if (num_samples < length_ || (num_samples+length_-1) > MCL_MAX_VLA_LENGTH) {
    FilterSerial(input_data, num_samples, output_data);
    return;
  }
  
  MCL_STACK_ALLOCATE(mcl::Real, padded_data, num_samples+length_-1); // TODO: handle stack overflow
  GetExtendedInput(input_data, num_samples, padded_data);
  
#if MCL_DATA_TYPE_DOUBLE
  vDSP_convD(padded_data, 1,
             coefficients_.data()+length_-1, -1,
             output_data, 1,
             num_samples, length_);
#else // Type is float
  vDSP_conv(padded_data, 1,
            coefficients_.data()+length_-1, -1,
            output_data, 1,
            num_samples, length_);
#endif
  
  // Reorganise state for the next run
  for (Int i=0; i<length_; ++i) {
    delay_line_[i] = input_data[num_samples-1-i];
  }
  
  counter_ = length_-1;
}
#endif
  

  

FirFilter FirFilter::GainFilter(Real gain) noexcept {
  std::vector<Real> B(1);
  B[0] = 1.0*gain;
  
  return FirFilter(B);
}
  
void FirFilter::Reset() noexcept {
  delay_line_ = Zeros<Real>(delay_line_.size());
}
  
void FirFilter::SetImpulseResponse(const std::vector<Real>& impulse_response,
                                   const Int update_length) noexcept {
  if (mcl::IsEqual(impulse_response, impulse_response_)) { return; }
  

  
  if ((Int)impulse_response.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = impulse_response.size();
    delay_line_.assign(length_, 0.0);
    counter_ = length_-1;
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
    
    if (! updating_) { // If there is no update being carried out
      impulse_response_old_ = impulse_response_;
    } else {
      impulse_response_old_ = coefficients_;
    }
  }
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
}

void FirFilter::UpdateCoefficients() noexcept {
  ASSERT(update_index_>=0 && update_index_<=update_length_);
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
  ASSERT(impulse_response_.size() == coefficients_.size());
  Real weight_new = ((Real)update_index_+1)/((Real)update_length_+1);
  Real weight_old = 1.0f-weight_new;
  Multiply(impulse_response_.data(), impulse_response_.size(), weight_new, coefficients_.data());
  MultiplyAdd(impulse_response_old_.data(), weight_old,
              coefficients_.data(), impulse_response_.size(),
              coefficients_.data());
  // The above is a lock-free equivalent version to
  // coefficients_ = mcl::Add(mcl::Multiply(impulse_response_, weight_new),
  //                          mcl::Multiply(impulse_response_old_, weight_old));
  
  if (update_index_ == update_length_) {
    updating_ = false;
  } else {
    update_index_++;
  }
}
  

} // namespace mcl




