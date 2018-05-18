/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_DIGITALFILTER_H
#define MCL_DIGITALFILTER_H

#include <vector>

#include "mcltypes.h"

namespace mcl {
  
/** Digital filter abstract class */
class DigitalFilter {
public:
  
  virtual void Filter(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept {
    FilterSerial(input_data, num_samples, output_data);
  }
  
  void FilterSerial(const Real* input_data, const Int num_samples,
                    Real* output_data) noexcept {
    ASSERT(num_samples>=0);
    ASSERT(input_data);
    ASSERT(output_data);
    for (Int i=0; i<num_samples; ++i) {
      output_data[i] = Filter(input_data[i]);
    }
  }
  
  /** Returns the output of the filter for an input equal to `input` . */
  virtual Real Filter(const Real input) noexcept = 0;
  
  /** Returns the output of the filter for an input signal equal to `input`. */
  std::vector<Real> Filter(const std::vector<Real>& input) noexcept {
    std::vector<Real> output(input.size(), 0.0);
    Filter(input.data(), input.size(), output.data());
    return output;
  }
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual ~DigitalFilter() {};
};
  
/** Filter bank abstract class */
class FilterBank {
public:
  /** Returns the output of the filter bank for an input equal to `input`. Hello world! */
  virtual std::vector<Real> Filter(const Real input) = 0;
  
  /** Returns the output of the filter bank for a given input. */
  virtual std::vector<std::vector<Real> >
  Filter(const std::vector<Real>& input) = 0;
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual Int num_filters() = 0;
  
  virtual ~FilterBank() {};
};
  
  
} // namespace mcl

#endif
