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

#ifdef MCL_APPLE_ACCELERATE
  #include <Accelerate/Accelerate.h>
#else
  #ifndef MCL_ENVARM
    #include <pmmintrin.h>
    #include <xmmintrin.h>
    #include <immintrin.h>
  #endif
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

Real FirFilter::Filter(Real input_sample) noexcept {
  if (updating_) { UpdateCoefficients(); }
  if (length_ == 1) {
    delay_line_[0] = input_sample;
    return input_sample*coefficients_[0];
  }
#ifdef MCL_APPLE_ACCELERATE
  return FilterAppleDsp(input_sample);
#else
  return FilterStraight(input_sample);
#endif
}
  

void FirFilter::Filter(const Real* input_data, const Int num_samples,
                       Real* output_data) noexcept {
  if (updating_) { UpdateCoefficients(); }
  if (length_ == 1) {
    delay_line_[0] = input_data[num_samples-1];
    mcl::Multiply(input_data, num_samples, coefficients_[0], output_data);
    return;
  }
#if defined(MCL_APPLE_ACCELERATE)
  FilterAppleDsp(input_data, num_samples, output_data);
#elif defined(MCL_AVX_ACCELERATE)
  if (num_samples < length_ || (num_samples+length_-1) > MCL_MAX_VLA_LENGTH) {
    FilterSerial(input_data, num_samples, output_data);
    return;
  }
  
  float* extended_input_data = MCL_STACK_ALLOCATE(num_samples+length_-1, float); // TODO: handle stack overflow
  float* output_data_float = MCL_STACK_ALLOCATE(num_samples, float); // TODO: handle stack overflow
  GetExtendedInput<float>(input_data, num_samples, extended_input_data);
  
  ALIGNED(16) __m256 input_frame;
  ALIGNED(16) __m256 coefficient;
  ALIGNED(16) __m256 product;
  ALIGNED(16) __m256 accumulator;
  const Int batch_size = 8;
  
  for(Int n=0; (n+batch_size)<=num_samples; n+=batch_size) {
    accumulator = _mm256_setzero_ps();
    for(Int k=0; k<length_; k++) {
      coefficient = _mm256_set1_ps((float) coefficients_[length_ - k - 1]);
      input_frame = _mm256_loadu_ps(extended_input_data + n + k);
      product = _mm256_mul_ps(coefficient, input_frame);
      accumulator = _mm256_add_ps(accumulator, product);
    }
    _mm256_storeu_ps(output_data_float+n, accumulator);
  }
  
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
#else // defined(MCL_NO_ACCELERATE)
  FilterSerial(input_data, num_samples, output_data);
#endif
}
  

Real FirFilter::FilterStraight(Real input_sample) noexcept {
  delay_line_[counter_] = input_sample;
  Real result = 0.0;
  Int index = (Int) counter_;
  
  for (int i=0; i<length_; ++i) {
    result += coefficients_[i] * delay_line_[index++];
    if (index >= length_) { index = 0; }
  }
  
  if (--counter_ < 0) { counter_ = length_-1; }
  
  return result;
}
  
  
#ifdef MCL_APPLE_ACCELERATE
Real FirFilter::FilterAppleDsp(Real input_sample) noexcept {
  if (length_-counter_ > MCL_MAX_VLA_LENGTH) {
    return FilterStraight(input_sample);
  }
  
  delay_line_[counter_] = input_sample;
  Real result = 0.0;
  
  Real result_a[length_-counter_];
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
  
void FirFilter::FilterAppleDsp(const Real* input_data, const Int num_samples,
                               Real* output_data) noexcept {
  if (num_samples < length_ || (num_samples+length_-1) > MCL_MAX_VLA_LENGTH) {
    FilterSerial(input_data, num_samples, output_data);
    return;
  }
  
  Real padded_data[num_samples+length_-1];
  GetExtendedInput(input_data, num_samples, padded_data);
  
#ifdef MCL_DATA_TYPE_DOUBLE
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
  if (mcl::IsEqual(std::vector<Real>(impulse_response.begin(),
                                      impulse_response.end()),
                   impulse_response_)) {
    return;
  }
  
  if (! updating_) { // If there is no update being carried out
    impulse_response_old_ = mcl::ZeroPad<Real>(impulse_response_,
                                               impulse_response.size());
  } else {
    impulse_response_old_ = mcl::ZeroPad<Real>(coefficients_,
                                               impulse_response.size());
  }
  
  updating_ = true;
  update_length_ = update_length;
  update_index_ = 0;
  impulse_response_ = impulse_response;
  
  if ((Int)impulse_response.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = impulse_response.size();
    delay_line_.assign(length_, 0.0);
    counter_ = impulse_response.size()-1;
  }
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
}

void FirFilter::UpdateCoefficients() noexcept {
  ASSERT(update_index_>=0 && update_index_<=update_length_);
  ASSERT(impulse_response_.size() == impulse_response_old_.size());
  Real weight_new = ((Real)update_index_+1)/((Real)update_length_+1);
  Real weight_old = 1.0f-weight_new;
  coefficients_ = mcl::Add(mcl::Multiply(impulse_response_, weight_new),
                           mcl::Multiply(impulse_response_old_, weight_old));
  
  if (update_index_ == update_length_) {
    updating_ = false;
  } else {
    update_index_++;
  }
}
  
} // namespace mcl




