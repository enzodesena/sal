/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_FIRFILTER_H
#define MCL_FIRFILTER_H

#include <vector>

#include "mcltypes.h"
#include "digitalfilter.h"
#include "vectorop.h"

namespace mcl {
/** FIR Filter */
class FirFilter : public DigitalFilter {
public:
  /** Constructs a default FIR filter, i.e. identical filter */
  FirFilter() noexcept;
  
  /** Constructs an FIR filter with impulse response B. */
  FirFilter(const std::vector<Real> B) noexcept;
  
  /** 
   Returns the output of the filter for an input equal to `input`.
   For example, if B=1, A=1, output will be equal to input. 
   As a second example, if B=[0,1], A=[1], you will have 
   (1) Filter(0.5)==0 and then
   (2) Filter(0.0)==0.5
   */
  virtual Real Filter(const Real input_sample) noexcept;
  
  virtual void Filter(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept;
  
  using DigitalFilter::Filter;
  
  /** 
   Updates the filter coefficients. You can set how long it takes to 
   update the coefficients (using linear interpolation between old and new
   impulse response). If an update is requested while another is already in
   progress, the new interpolation will pick up from where the old one left
   off to avoid audible artifacts.
   @param[in] update_length How many calls to Filter it takes to update the
   coefficients. A value of 0 means that the update is instantaneous. A call
   to Filter(const Real input) counts one, just like
   Filter(const std::vector<Real>& input). This is due to the fact that it
   is difficult to update a filter when using DSP-enabled batch filtering. 
   */
  void SetImpulseResponse(const std::vector<Real>& impulse_response,
                            const Int update_length = 0) noexcept;
  
  /** Resets the state of the filter */
  void Reset() noexcept;
  
  /** Returns the impulse response of the filter */
  std::vector<Real> impulse_response() noexcept;
  
  /** Constructs a filter for which output==gain*input always. */
  static FirFilter GainFilter(const Real gain) noexcept;
  
  /** Tests */
  static bool Test();
  
  static void SpeedTests();
  
  virtual ~FirFilter() {}
  
private:
#ifdef OSXIOS
  Real FilterAppleDsp(Real input_sample) noexcept;
  void FilterAppleDsp(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept;
#endif
  
  Real FilterStraight(Real input_sample) noexcept;
  
  std::vector<Real> FilterSequential(const std::vector<Real>& input) noexcept;
  
  /** Method called to slowly update the filter coefficients. It is called
   every time one of the Filter method is called and is activated only
   if updating_ = true. TODO: uniformise action between sequential and
   batch. */
  void UpdateCoefficients() noexcept;
  
  std::vector<Real> impulse_response_;
  std::vector<Real> impulse_response_old_;
  Int update_index_;
  Int update_length_;
  
  bool updating_;
  
  std::vector<Real> coefficients_;
  std::vector<Real> delay_line_;
  Int counter_;
  Int length_;
};
  
  
class GainFilter : public DigitalFilter {
public:
  GainFilter(const Real gain) : gain_(gain) {}
  
  virtual Real Filter(const Real input) noexcept {
    return input*gain_;
  }
  
  virtual void Filter(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept {
    Multiply(input_data, num_samples, gain_, output_data);
  }
  
  virtual void Reset() {}
private:
  Real gain_;
};

class IdenticalFilter : public DigitalFilter {
public:
  IdenticalFilter() {}
  
  virtual Real Filter(const Real input) noexcept {
    return input;
  }
  
  virtual void Filter(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept {
    for (Int i=0; i<num_samples; ++i) {
      output_data[i] = input_data[i];
    }
  }
  
  virtual void Reset() {}
};
  
} // namespace mcl


#endif
