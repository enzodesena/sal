 /*
 iirfilter.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
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

std::vector<Real> FirFilter::impulse_response() { 
  return impulse_response_; 
}
  
FirFilter::FirFilter() : impulse_response_(mcl::UnaryVector<Real>(1.0)),
counter_(0), length_(1) {
  delay_line_.assign(length_, 0.0);
  impulse_response_float_ = std::vector<float>(impulse_response_.begin(),
                                               impulse_response_.end());
}
  
FirFilter::FirFilter(std::vector<Real> B) : impulse_response_(B),
        counter_(B.size()-1), length_(B.size()) {
  delay_line_.assign(length_, 0.0);
    impulse_response_float_ = std::vector<float>(impulse_response_.begin(),
                                                 impulse_response_.end());
}
  
void FirFilter::UpdateFilter(std::vector<Real> B) {
  impulse_response_ = B;
  if (B.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = B.size();
    delay_line_.assign(length_, 0.0);
    counter_ = B.size()-1;
  }
  impulse_response_float_ = std::vector<float>(impulse_response_.begin(),
                                               impulse_response_.end());
}


Real FirFilter::Filter(Real input_sample) {
  delay_line_[counter_] = input_sample;
  float result = 0.0;
  
#ifdef OSXIOS
  std::vector<float> result_a(length_-counter_, 0.0);
  vDSP_vmul(&impulse_response_float_.at(0), 1,
             &delay_line_.at(counter_), 1,
             &result_a.at(0), 1,
             length_-counter_);
  for (UInt i=0; i<length_-counter_; i++) { result += result_a[i]; }
  
  if (counter_ > 0) {
    std::vector<float> result_b(counter_, 0.0);
    vDSP_vmul(&impulse_response_float_[length_-counter_], 1,
               &delay_line_[0], 1,
               &result_b[0], 1,
               counter_);
    for (UInt i=0; i<counter_; i++) { result += result_b[i]; }
  }
#else
  Int index = (Int) counter_;
  
  if (length_%8 != 0) { //
    for (int i=0; i<length_; ++i) {
      result += impulse_response_[i] * delay_line_[index++];
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
        result_a += impulse_response_[i++] * delay_line_[index++];
        result_b += impulse_response_[i++] * delay_line_[index++];
        result_c += impulse_response_[i++] * delay_line_[index++];
        result_d += impulse_response_[i++] * delay_line_[index++];
        result_e += impulse_response_[i++] * delay_line_[index++];
        result_f += impulse_response_[i++] * delay_line_[index++];
        result_g += impulse_response_[i++] * delay_line_[index++];
        result_h += impulse_response_[i++] * delay_line_[index++];
      } else {
        for (Int k=0; k<8; ++k) {
          result += impulse_response_[i++] * delay_line_[index++];
          if (index >= length_) { index = 0; }
        }
      }
    }
    result += result_a + result_b + result_c + result_d + result_e + result_f +
    result_g + result_h;
  }
#endif
  
  if (--counter_ < 0) { counter_ = length_-1; }
  
  return result;
}
  
std::vector<Real> FirFilter::Filter(const std::vector<Real>& input) {
  
#ifdef OSXIOS
  if (input.size() < length_) { return FilterSequential(input); }
  
  std::vector<float> output(input.size(), 0.0);
  
  // Construct long input signal
  std::vector<float> conv_input;
  conv_input.insert(conv_input.begin(),
                    delay_line_.begin()+counter_, delay_line_.end());
  if (counter_ >= 1) {
    conv_input.insert(conv_input.end(),
                      delay_line_.begin(), delay_line_.begin()+counter_);
  }
  
  std::reverse(conv_input.begin(), conv_input.end());
  conv_input.pop_back();
  assert(conv_input.size() == length_-1);
  
  conv_input.insert(conv_input.end(), input.begin(), input.end());
  assert(conv_input.size() == (input.size()+length_-1));
  
  vDSP_conv(&conv_input[0], 1,
            &impulse_response_float_[length_-1], -1,
            &output[0], 1,
            input.size(), length_);
  
  assert(output.size() == input.size());
  delay_line_ = std::vector<float>(input.rbegin(), input.rbegin()+length_);
  counter_ = length_-1;
  
  return std::vector<Real>(output.begin(), output.end());
#else
  return FilterSequential(input);
#endif
}
  
std::vector<Real> FirFilter::FilterSequential(const std::vector<Real>& input) {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) {
    output[i] = this->Filter(input[i]);
  }
  return output;
}
  

FirFilter FirFilter::GainFilter(Real gain) {
  std::vector<Real> B(1);
  B[0] = 1.0*gain;
  
  return FirFilter(B);
}
  
void FirFilter::Reset() {
  delay_line_ = Zeros<float>(delay_line_.size());
}
  
} // namespace mcl




