/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_DIGITALFILTER_H
#define MCL_DIGITALFILTER_H

#include <vector>
#include <span>

#include "mcltypes.h"

namespace mcl {
  
/** Digital filter abstract class */
class Filter {
public:
  
  virtual void ProcessBlock(std::span<const Real> input_data, std::span<Real> output_data) noexcept {
    ProcessBlockSerial(input_data, output_data);
  }
  
  void ProcessBlockSerial(std::span<const Real> input_data, std::span<Real> output_data) noexcept {
    for (size_t i=0; i<input_data.size(); ++i) {
      output_data[i] = ProcessSample(input_data[i]);
    }
  }
  
  /** Returns the output of the filter for an input equal to `input` . */
  virtual Real ProcessSample(const Real input) noexcept = 0;
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
};
  
/** Filter bank abstract class */
class FilterBank {
public:
  /** Returns the output of the filter bank for an input equal to `input`. Hello world! */
  virtual void ProcessSample(const Real input_data, std::span<Real> output_data) = 0;
  
  /** Returns the output of the filter bank for a given input. */
  virtual void ProcessBlock(std::span<const Real> input_data, std::span<std::span<Real> > output_data) {
    for (size_t i=0; i<input_data.size(); ++i) {
      ProcessSample(input_data[i], output_data[i]);
    }
  }
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual Int num_filters() = 0;
};
  
  
} // namespace mcl

#endif
