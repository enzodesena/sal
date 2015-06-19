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
  
FirFilter::FirFilter() : impulse_response_(mcl::UnaryVector(1.0)),
counter_(0), length_(1) {
  delay_line_.assign(length_, 0.0);
}
  
FirFilter::FirFilter(std::vector<Real> B) : impulse_response_(B),
        counter_(0), length_(B.size()) {
  delay_line_.assign(length_, 0.0);
}
  
void FirFilter::UpdateFilter(std::vector<Real> B) {
  impulse_response_ = B;
  if (B.size() != length_) {
    // If the impulse response changes length, then reset everything.
    length_ = B.size();
    delay_line_.assign(length_, 0.0);
    counter_ = 0;
  }
}


Real FirFilter::Filter(Real input_sample) {
  delay_line_[counter_] = input_sample;
  double result = 0.0;
  
#ifdef OSXIOS
  std::vector<Real> result_a(length_-counter_, 0.0);
  vDSP_vmulD(&impulse_response_[0], 1,
             &delay_line_[counter_], 1,
             &result_a[0], 1,
             length_-counter_);
  for (UInt i=0; i<length_-counter_; i++) { result += result_a[i]; }
  
  if (counter_ > 0) {
    std::vector<Real> result_b(counter_, 0.0);
    vDSP_vmulD(&impulse_response_[length_-counter_], 1,
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
    double result_a = 0;
    double result_b = 0;
    double result_c = 0;
    double result_d = 0;
    double result_e = 0;
    double result_f = 0;
    double result_g = 0;
    double result_h = 0;
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
  delay_line_ = Zeros<Real>(delay_line_.size());
}
  
} // namespace mcl




