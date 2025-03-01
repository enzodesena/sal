/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_MAXGRADIENTFILTER_H
#define SAL_DSP_MAXGRADIENTFILTER_H

#include "digitalfilter.h"
#include "elementaryop.h"
#include "saltypes.h"

namespace sal {

namespace dsp {

class MaxGradientFilter {
 public:
  MaxGradientFilter(Real max_gradient)
      : max_gradient_(max_gradient), previous_output_(0.0) {}

  Real ProcessSample(const Real input) noexcept {
    Real output;
    if (dsp::Abs(previous_output_ - input) < max_gradient_) {
      output = input;
    } else if (previous_output_ < input) {
      output = previous_output_ + max_gradient_;
    } else {
      output = previous_output_ - max_gradient_;
    }
    previous_output_ = output;
    return output;
  }
  
  void ProcessBlock(std::span<const Sample> input_data,
                    std::span<Sample> output_data) {
    for (size_t i = 0; i<input_data.size(); ++i) {
      output_data[i] = ProcessSample(output_data[i]);
    }
  }

  void ResetState() noexcept { previous_output_ = 0.0; }

 private:
  Real max_gradient_;
  Real previous_output_;
};

} // namespace dsp

} // namespace sal
#endif
