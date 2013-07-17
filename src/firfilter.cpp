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


namespace mcl {

std::vector<Real> FirFilter::impulse_response() { 
  return impulse_response_; 
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
  Int index = (Int) counter_;
  for (int i=0; i<length_; ++i) {
    result += impulse_response_[i] * delay_line_[index--];
    if (index < 0) index = length_-1;
  }
  
  if (++counter_ >= length_) { counter_ = 0; }
  
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




