/*
 firfilter.h
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 109 $
 Last changed date:  $Date: 2012-07-16 19:33:28 +0100 (Mon, 16 Jul 2012) $
 */

#ifndef MCL_FIRFILTER_H
#define MCL_FIRFILTER_H

#include <vector>

#include "mcltypes.h"
#include "digitalfilter.h"

namespace mcl {
  
class FirFilter : public DigitalFilter {
public:
  // Constructs an FIR filter with impulse response B.
  FirFilter(std::vector<Real> B);
  
  // Returns the output of the filter for an input equal to `input`. 
  // For example, if B=1, A=1, output will be equal to input. 
  // As a second example, if B=[0,1], A=[1], you will have 
  // (1) Filter(0.5)==0 and then
  // (2) Filter(0.0)==0.5
  virtual Real Filter(const Real input);
  
  // Returns the output of the filter for an input signal equal to `input`.
  virtual std::vector<Real> Filter(const std::vector<Real>& input);
  
  // Updates the filter coefficients. May cause articafts if the coefficients are
  // updated too rapidly. If the `impulse_response` changes length than the
  // filter is resetted to zero state.
  void UpdateFilter(std::vector<Real> impulse_response);
  
  std::vector<Real> impulse_response();
  
  // Constructs a filter for which output==gain*input always.
  static FirFilter GainFilter(const Real gain);
  
  static bool Test();
  
  virtual ~FirFilter() {}
  
private:
  std::vector<Real> impulse_response_;
  std::vector<Real> delay_line_;
  UInt counter_;
  UInt length_;
};
  
} // namespace mcl


#endif
