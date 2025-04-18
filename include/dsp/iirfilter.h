/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_IIRFILTER_H
#define SAL_DSP_IIRFILTER_H

#include "constants.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

/** IIR Filter */
class IirFilter {
 public:
  /** Constructs a default filter, i.e. identical filter*/
  IirFilter();

  /**
   Constructs an IIR filter (II-type direct implementation). B and A are
   numerator and denominator of the filter, respectively.
   */
  IirFilter(std::vector<Real> B, std::vector<Real> A);

  /**
   Returns the output of the filter for an input equal to `input`.
   For example, if B=1, A=1, output will be equal to input.
   As a second example, if B=[0,1], A=[1, 0], you will have
   (1) Filter(0.5)==0 and then
   (2) Filter(0.0)==0.5
   */
  Real ProcessSample(const Real input) noexcept;

  void ProcessBlock(std::span<const Real> input_data,
                            std::span<Real> output_data) noexcept;

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

  void SetDenominatorCoefficient(const size_t coeff_id,
                                 const Real value) noexcept;

  /**
   * Returns the filter response at given frequencies
   *
   * @param frequencies The frequencies at which to calculate the response
   * @return The frequency response of the filter
   */
  std::vector<Real> GetFrequencyResponse(const std::vector<Real>& frequencies,
                                         const Real sampling_frequency) const;

  /** Returns the forward coefficients */
  std::vector<Real> numerator_coeffs() const;

  /** Returns the backward coefficients */
  std::vector<Real> denominator_coeffs() const;

  void ResetState();

  /** Tests */
  static bool Test();
  static bool PeakingFilterTest() noexcept;
  static bool PeakHighShelfTest() noexcept;
  static bool PeakLowShelfTest() noexcept;
  static bool GraphicEqTest() noexcept;

 private:
  void ProcessBlockSerial(std::span<const Real> input_data,
                          std::span<Real> output_data) noexcept;
  std::vector<Real> numerator_coeffs_;
  std::vector<Real> denominator_coeffs_;

  // By storing A[0] before normalisation we can output B() and A() before
  // normalisation while keeping the internal representation normalised
  Real denominator_coeff_0_;

  std::vector<Real> state_;
};

/** Filter bank abstract class */
class IirFilterBank : public FilterBank {
 private:
  std::vector<IirFilter> filters_;

 public:
  IirFilterBank(const std::vector<IirFilter>& filters) noexcept
      : filters_(filters) {}

  virtual Int num_filters() noexcept { return filters_.size(); }

  /** Returns the output of the filter bank for an input equal to `input`. */
  virtual void ProcessSample(const Real input, std::span<Real> output_data);

  /** Resets the state of the filter */
  virtual void ResetState();
};

/** Constructs a filter for which output==input always. */
IirFilter IdenticalFilter();

/** Constructs a filter for which output==gain*input always. */
IirFilter GainFilter(const Real gain);

/**
 Get wall filters of type wall_type and for FS given by sampling_frequency
 */
IirFilter WallFilter(WallType wall_type, Real sampling_frequency, Real gain = 1.0);

/** Returns a pinkifier filter */
IirFilter PinkifierFilter();

IirFilter SeriesFilter(const IirFilter& filter_a, const IirFilter& filter_b);

IirFilter SeriesFilter(const std::vector<IirFilter>& filters);

} // namespace dsp

} // namespace sal

#endif
