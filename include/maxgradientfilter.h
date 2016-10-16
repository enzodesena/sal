/*
 maxgradientfilter.h
 MCL
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#ifndef MCL_MAXGRADIENTFILTER_H
#define MCL_MAXGRADIENTFILTER_H

#include "elementaryop.h"
#include "digitalfilter.h"

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {
  
class MCL_API MaxGradientFilter : public DigitalFilter {
public:
  MaxGradientFilter(Real max_gradient) :
  max_gradient_(max_gradient), previous_output_(0.0) {}
  
  virtual Real Filter(const Real input) {
    Real output;
    if (mcl::Abs(previous_output_-input) < max_gradient_) {
      output = input;
    } else if (previous_output_ < input) {
      output = previous_output_ + max_gradient_;
    } else {
      output = previous_output_ - max_gradient_;
    }
    previous_output_ = output;
    return output;
  }
  
  virtual void Reset() {
    previous_output_ = 0.0;
  }
  
private:
  Real max_gradient_;
  Real previous_output_;
};

} // namespace mcl
#endif
