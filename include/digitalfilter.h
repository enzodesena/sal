/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef MCL_DIGITALFILTER_H
#define MCL_DIGITALFILTER_H

#include <vector>

#include "mcltypes.h"

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {
  
/** Digital filter abstract class */
class MCL_API DigitalFilter {
public:
  
  /** Returns the output of the filter for an input equal to `input` . */
  virtual Real Filter(const Real input) = 0;
  
  /** Returns the output of the filter for an input signal equal to `input`. */
  virtual std::vector<Real> Filter(const std::vector<Real>& input) {
    std::vector<Real> output(input.size(), 0.0);
    for (UInt i=0; i<input.size(); ++i) {
      output.at(i) = Filter(input.at(i));
    }
    return output;
  }
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual ~DigitalFilter() {};
};
  
/** Filter bank abstract class */
class MCL_API FilterBank {
public:
  /** Returns the output of the filter bank for an input equal to `input`. Hello world! */
  virtual std::vector<Real> Filter(const Real input) = 0;
  
  /** Returns the output of the filter bank for a given input. */
  virtual std::vector<std::vector<Real> >
  Filter(const std::vector<Real>& input) = 0;
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual UInt num_filters() = 0;
  
  virtual ~FilterBank() {};
};
  
  
} // namespace mcl

#endif
