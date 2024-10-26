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
class Filter {
public:
  
  virtual void ProcessBlock(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept {
    ProcessBlockSerial(input_data, num_samples, output_data);
  }
  
  void ProcessBlockSerial(const Real* input_data, const Int num_samples,
                    Real* output_data) noexcept {
    ASSERT(num_samples>=0);
    ASSERT(input_data);
    ASSERT(output_data);
    for (Int i=0; i<num_samples; ++i) {
      output_data[i] = ProcessSample(input_data[i]);
    }
  }
  
  /** Returns the output of the filter for an input equal to `input` . */
  virtual Real ProcessSample(const Real input) noexcept = 0;
  
  /** Returns the output of the filter for an input signal equal to `input`. */
  std::vector<Real> ProcessBlock(const std::vector<Real>& input) noexcept {
    std::vector<Real> output(input.size(), 0.0);
    ProcessBlock(input.data(), input.size(), output.data());
    return output;
  }
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual ~Filter() {};
};
  
/** Filter bank abstract class */
class FilterBank {
public:
  /** Returns the output of the filter bank for an input equal to `input`. Hello world! */
  virtual std::vector<Real> ProcessSample(const Real input) = 0;
  
  /** Returns the output of the filter bank for a given input. */
  virtual std::vector<std::vector<Real> > ProcessBlock(const std::vector<Real>& input) = 0;
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual Int num_filters() = 0;
  
  virtual ~FilterBank() {};
};
  
  
} // namespace mcl

#endif
