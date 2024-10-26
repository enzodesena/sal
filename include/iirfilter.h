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
  virtual Real ProcessSample(const Real input) noexcept;
  
  virtual void ProcessBlock(const Real* input_data, const size_t num_samples,
                      Real* output_data) noexcept;
  
  using DigitalFilter::ProcessBlock;
  
  /** Returns the order of the filter. */
  size_t order() const noexcept;
  
  /** 
   Updates the filter coefficients. May cause articafts if the coefficients are
   updated too rapidly.
   */
  void SetCoefficients(const std::vector<Real>& B,
                       const std::vector<Real>& A) noexcept;
  
  /** Sets the coefficients as identical to those of another filter. */
  void SetCoefficients(const IirFilter& filter) noexcept;
  
  Real GetNumeratorCoefficient(const size_t coeff_id) const noexcept;
  
  Real GetDenominatorCoefficient(const size_t coeff_id) const noexcept;
  
  void SetNumeratorCoefficient(const size_t coeff_id,
                               const Real value) noexcept;
  
  void SetDenominatorCoefficient(const size_t coeff_id, const Real value) noexcept;
  
  /**
  * Returns the filter response at given frequencies
  *
  * @param frequencies The frequencies at which to calculate the response
  * @return The frequency response of the filter
  */
  std::vector<Real> GetFrequencyResponse(const std::vector<Real>& frequencies, const Real sampling_frequency) const;
  
  /** Returns the forward coefficients */
  std::vector<Real> B() const;
  
  /** Returns the backward coefficients */
  std::vector<Real> A() const;
  
  void Reset();
  
  /** Tests */
  static bool Test();
  static bool PeakingFilterTest() noexcept;
  static bool PeakHighShelfTest() noexcept;
  static bool PeakLowShelfTest() noexcept;
  static bool GraphicEqTest() noexcept;
  
private:
  std::vector<Real> B_;
  std::vector<Real> A_;
  
  // By storing A[0] before normalisation we can output B() and A() before
  // normalisation while keeping the internal representation normalised
  Real A0_;
  
  std::vector<Real> state_;
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
  virtual std::vector<Real> ProcessSample(const Real input);
  
  /** Returns the output of the filter bank for a given input. */
  virtual std::vector<std::vector<Real> >
  ProcessBlock(const std::vector<Real>& input);
  
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

//
///**
//* Initialises a 2nd order high shelf filter with a given cut off frequency and shelf gain
//*
//* @param fc The cut off frequency of the filter
//* @param g The shelf gain of the filter (linear)
//* @param Q The quality factor of the filter
//* @param sample_rate The sample rate for calculating filter coefficients
//*/
//IirFilter PeakHighShelfFilter(const Real fc, const Real g, const Real Q, const int sample_rate);
//
///**
//* Initialises a 2nd order low shelf filter with a given cut off frequency and shelf gain
//*
//* @param fc The cut off frequency of the filter
//* @param g The shelf gain of the filter (linear)
//* @param Q The quality factor of the filter
//* @param sample_rate The sample rate for calculating filter coefficients
//*/
//IirFilter PeakLowShelfFilter(const Real fc, const Real g, const Real Q, const int sample_rate);
//
///**
//* Initialises a  2nd order peaking filter with a given cut off frequency and gain
//*
//* @param fc The center frequency of the filter
//* @param g The gain of the filter (linear)
//* @param Q The quality factor of the filter
//* @param sample_rate The sample rate for calculating filter coefficients
//*/
//IirFilter PeakingFilter(const Real fc, const Real g, const Real Q, const int sample_rate);
//
//IirFilter GraphicEqFilter(const std::vector<Real>& gain, const std::vector<Real>& fc, const Real Q, const Real sampling_frequency);

IirFilter SeriesFilter(const IirFilter& filter_a, const IirFilter& filter_b);

IirFilter SeriesFilter(const std::vector<IirFilter>& filters);

} // namespace mcl

#endif
