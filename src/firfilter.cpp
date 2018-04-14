/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "firfilter.h"
#include "vectorop.h"
#include "mcltypes.h"
#include <vector>

#ifdef OSXIOS
  #include <Accelerate/Accelerate.h>
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
  
  delay_line_[counter_] = input_sample;
  Real result = 0.0f;
  
#ifdef OSXIOS
  std::vector<Real> result_a(length_-counter_, 0.0);
  #ifdef MCL_DATA_TYPE_DOUBLE
  vDSP_vmulD(&coefficients_.at(0), 1,
            &delay_line_.at(counter_), 1,
            &result_a.at(0), 1,
            length_-counter_);
  #else
  vDSP_vmul(&coefficients_.at(0), 1,
            &delay_line_.at(counter_), 1,
            &result_a.at(0), 1,
            length_-counter_);
  #endif
  for (UInt i=0; i<length_-counter_; i++) { result += result_a[i]; }
  
  if (counter_ > 0) {
    std::vector<Real> result_b(counter_, 0.0);
  #ifdef MCL_DATA_TYPE_DOUBLE
    vDSP_vmulD(&coefficients_[length_-counter_], 1,
              &delay_line_[0], 1,
              &result_b[0], 1,
              counter_);
  #else
    vDSP_vmul(&coefficients_[length_-counter_], 1,
              &delay_line_[0], 1,
              &result_b[0], 1,
              counter_);
  #endif
    for (UInt i=0; i<counter_; i++) { result += result_b[i]; }
  }
#else
  Int index = (Int) counter_;
  
  if (length_%8 != 0) {
    for (int i=0; i<length_; ++i) {
      result += coefficients_[i] * delay_line_[index++];
      if (index >= length_) { index = 0; }
    }
  } else {
    // This is easier for the compiler to vectorise
    Real result_a = 0;
    Real result_b = 0;
    Real result_c = 0;
    Real result_d = 0;
    Real result_e = 0;
    Real result_f = 0;
    Real result_g = 0;
    Real result_h = 0;
    Int i = 0;
    while (i < length_) {
      if (index < (length_-8)) {
        result_a += coefficients_[i++] * delay_line_[index++];
        result_b += coefficients_[i++] * delay_line_[index++];
        result_c += coefficients_[i++] * delay_line_[index++];
        result_d += coefficients_[i++] * delay_line_[index++];
        result_e += coefficients_[i++] * delay_line_[index++];
        result_f += coefficients_[i++] * delay_line_[index++];
        result_g += coefficients_[i++] * delay_line_[index++];
        result_h += coefficients_[i++] * delay_line_[index++];
      } else {
        for (Int k=0; k<8; ++k) {
          result += coefficients_[i++] * delay_line_[index++];
          if (index >= length_) { index = 0; }
        }
      }
    }
    result += result_a + result_b + result_c + result_d + result_e + result_f +
              result_g + result_h;
  }
#endif
  
  if (--counter_ < 0) { counter_ = length_-1; }
  
  return (Real) result;
}
  
  
void FirFilter::Filter(const Real* input_data, const Int num_samples,
                       Real* output_data) noexcept {
  if (updating_) { UpdateCoefficients(); }
#ifdef OSXIOS
  if (num_samples < length_) {
    FilterSerial(input_data, num_samples, output_data);
    return;
  }
  
  Real padded_data[num_samples+length_-1];
  
  // Stage 1
  for (Int i=0; i<counter_; ++i) {
    padded_data[i] = delay_line_[counter_-i-1];
  }
  
  // Stage 2
  // Starting from counter_ in padded_data
  // Ending in counter_+(length_-counter_-1)
  for (Int i=counter_; i<(length_-1); ++i) {
    padded_data[i] = delay_line_[length_-1-(i-counter_)];
  }
  
  // Stage 3
  // Append input signal
  for (Int i=(length_-1); i<(length_-1+num_samples); ++i) {
    padded_data[i] = input_data[i-(length_-1)];
  }
  
  #ifdef MCL_DATA_TYPE_DOUBLE
  vDSP_convD(padded_data, 1,
             coefficients_.data()+length_-1, -1,
             output_data, 1,
             num_samples, length_);
  #else // Type is Real
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
#else // If not OSXIOS
  FilterSerial(input_data, num_samples, output_data);
#endif
}
  
std::vector<Real>
FirFilter::FilterSequential(const std::vector<Real>& input) noexcept {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) {
    output[i] = this->Filter(input[i]);
  }
  return output;
}
  

FirFilter FirFilter::GainFilter(Real gain) noexcept {
  std::vector<Real> B(1);
  B[0] = 1.0*gain;
  
  return FirFilter(B);
}
  
void FirFilter::Reset() noexcept {
  delay_line_ = Zeros<Real>(delay_line_.size());
}
  
void FirFilter::set_impulse_response(const std::vector<Real>& impulse_response,
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
  
  if (impulse_response.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = impulse_response.size();
    delay_line_.assign(length_, 0.0);
    counter_ = impulse_response.size()-1;
  }
  assert(impulse_response_.size() == impulse_response_old_.size());
}

void FirFilter::UpdateCoefficients() noexcept {
  assert(update_index_>=0 & update_index_<=update_length_);
  assert(impulse_response_.size() == impulse_response_old_.size());
  Real weight_new = ((Real)update_index_+1)/((Real)update_length_+1);
  Real weight_old = 1.0f-weight_new;
  coefficients_ = mcl::Add(mcl::Multiply<Real>(impulse_response_, weight_new),
                           mcl::Multiply<Real>(impulse_response_old_, weight_old));
  
  if (update_index_ == update_length_) {
    updating_ = false;
  } else {
    update_index_++;
  }
}
  
} // namespace mcl




