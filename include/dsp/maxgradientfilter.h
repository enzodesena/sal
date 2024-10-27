/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_MAXGRADIENTFILTER_H
#define MCL_MAXGRADIENTFILTER_H

#include "digitalfilter.h"
#include "elementaryop.h"

namespace sal {

namespace dsp {

class MaxGradientFilter : public Filter {
 public:
  MaxGradientFilter(Real max_gradient)
      : max_gradient_(max_gradient), previous_output_(0.0) {}

  virtual Real ProcessSample(const Real input) noexcept {
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

  virtual void Reset() noexcept { previous_output_ = 0.0; }

 private:
  Real max_gradient_;
  Real previous_output_;
};

} // namespace dsp

} // namespace sal
#endif
