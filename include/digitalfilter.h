/*
 digitalfilter.h
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_DIGITALFILTER_H
#define MCL_DIGITALFILTER_H

#include <vector>

#include "mcltypes.h"

namespace mcl {
  
/** Digital filter abstract class */
class DigitalFilter {
public:
  
  /** Returns the output of the filter for an input equal to `input` . */
  virtual Real Filter(const Real input) = 0;
  
  /** Returns the output of the filter for an input signal equal to `input`. */
  virtual std::vector<Real> Filter(const std::vector<Real>& input) = 0;
  
  /** Resets the state of the filter */
  virtual void Reset() = 0;
  
  virtual ~DigitalFilter() {};
};
  
/** Filter bank abstract class */
class FilterBank {
public:
  /** Returns the output of the filter bank for an input equal to `input`. */
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
