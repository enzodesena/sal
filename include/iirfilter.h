/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_IIRFILTER_H
#define MCL_IIRFILTER_H

#include "digitalfilter.h"
#include "vectorop.h"
#include "constants.h"

namespace mcl {

  
/** IIR Filter */
class IirFilter : public DigitalFilter {
public:
  /** Constructs a default filter, i.e. identical filter*/
  IirFilter();
  
  /** 
   Constructs an IIR filter (II-type direct implementation). B and A are numerator
   and denominator of the filter, respectively.
   */
  IirFilter(std::vector<Real> B,std::vector<Real> A);
  
  
  /** 
   Returns the output of the filter for an input equal to `input`.
   For example, if B=1, A=1, output will be equal to input. 
   As a second example, if B=[0,1], A=[1, 0], you will have
   (1) Filter(0.5)==0 and then
   (2) Filter(0.0)==0.5
   */
  virtual Real Filter(const Real input) noexcept;
  
  virtual void Filter(const Real* input_data, const Int num_samples,
                      Real* output_data) noexcept;
  
  using DigitalFilter::Filter;
  
  /** Returns the order of the filter. */
  Int order() const noexcept;
  
  /** 
   Updates the filter coefficients. May cause articafts if the coefficients are
   updated too rapidly.
   */
  void SetCoefficients(const std::vector<Real>& B,
                       const std::vector<Real>& A) noexcept;
  
  /** Sets the coefficients as identical to those of another filter. */
  void SetCoefficients(const IirFilter& filter) noexcept;
  
  Real GetNumeratorCoefficient(const Int coeff_id) const noexcept;
  
  Real GetDenominatorCoefficient(const Int coeff_id) const noexcept;
  
  void SetNumeratorCoefficient(const Int coeff_id,
                               const Real value) noexcept;
  
  inline void SetDenominatorCoefficient(const Int coeff_id,
                                        const Real value) noexcept;
  
  /** Returns the forward coefficients */
  std::vector<Real> B() const;
  
  /** Returns the backward coefficients */
  std::vector<Real> A() const;
  
  void Reset();
  
  /** Tests */
  static bool Test();
  
  /** Assignment operator */
  IirFilter & operator= (const IirFilter &);
  
  /** Copy operator */
  IirFilter (const IirFilter&);
  
  virtual ~IirFilter();
  
private:
  std::vector<Real> B_;
  std::vector<Real> A_;
  
  // By storing A[0] before normalisation we can output B() and A() before
  // normalisation while keeping the internal representation normalised
  Real A0_;
  
  Real* state_;
};
  
/** Filter bank abstract class */
class IirFilterBank : public FilterBank {
private:
  std::vector<IirFilter> filters_;
  
public:
  IirFilterBank(const std::vector<IirFilter>& filters) noexcept :
        filters_(filters) {}
  
  virtual Int num_filters() noexcept { return filters_.size(); }
  
  /** Returns the output of the filter bank for an input equal to `input`. */
  virtual std::vector<Real> Filter(const Real input);
  
  /** Returns the output of the filter bank for a given input. */
  virtual std::vector<std::vector<Real> >
  Filter(const std::vector<Real>& input);
  
  /** Resets the state of the filter */
  virtual void Reset();
};
  
//
//  /** Implements a first-order IIR low-pass filter with a given decay constant. */
//  class RampSmoothingFilter : public DigitalFilter {
//  public:
//    
//    /**
//     @param[in] ramp_samples number of samples after which the value is
//     to 1/e away from target value. */
//    RampSmoothingFilter(const Real ramp_samples) noexcept {
//      ASSERT_WITH_MESSAGE(std::isgreaterequal(ramp_samples, 0),
//                          "Decay constant cannot be negative ");
//      
//      
//    }
//    
//    virtual Real Filter(const Real input) noexcept {
//      return filter_.Filter(input);
//    }
//    
//    using DigitalFilter::Filter;
//    
//    virtual void Reset() noexcept { filter_.Reset(); }
//    
//    
//  private:
//  };
//  
///** Implements a first-order IIR low-pass filter with a given decay constant. */
//class LowPassSmoothingFilter : public DigitalFilter {
//public:
//  
//  /**
//   @param[in] ramp_samples number of samples after which the value is
//   to 1/e away from target value. */
//  LowPassSmoothingFilter(const Real ramp_samples) noexcept {
//    ASSERT_WITH_MESSAGE(std::isgreaterequal(ramp_samples, 0),
//                        "Decay constant cannot be negative ");
//    
//    Real a1 = exp(-1.0/ramp_samples);
//    Real b0 = 1.0 - a1;
//    filter_ = IirFilter(mcl::BinaryVector(b0, 0.0),
//                        mcl::BinaryVector(1.0, -a1));
//  }
//  
//  virtual Real Filter(const Real input) noexcept {
//    return filter_.Filter(input);
//  }
//  
//  using DigitalFilter::Filter;
//  
//  virtual void Reset() noexcept { filter_.Reset(); }
//  
//  
//private:
//  IirFilter filter_;
//};
  
/** Constructs a filter for which output==input always. */
IirFilter IdenticalFilter();

/** Constructs a filter for which output==gain*input always. */
IirFilter GainFilter(const Real gain);

/**
 Get wall filters of type wall_type and for FS given by sampling_frequency
 */
IirFilter WallFilter(WallType wall_type, Real sampling_frequency);

/** Returns a pinkifier filter */
IirFilter PinkifierFilter();
  
} // namespace mcl

#endif
