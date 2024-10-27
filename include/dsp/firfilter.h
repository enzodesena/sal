/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_FIRFILTER_H
#define SAL_DSP_FIRFILTER_H

#include <span>
#include <vector>

#include "digitalfilter.h"
#include "dsptypes.h"
#include "vectorop.h"

namespace sal {

namespace dsp {
/** FIR Filter */
class FirFilter : public Filter {
 public:
  /** Constructs an FIR filter with impulse response B. */
  FirFilter(const std::vector<Real> B = std::vector<Real>(1, 1.0),
            const size_t max_input_length = 1 << 14) noexcept;

  /**
   Returns the output of the filter for an input equal to `input`.
   For example, if B=1, A=1, output will be equal to input.
   As a second example, if B=[0,1], A=[1], you will have
   (1) Filter(0.5)==0 and then
   (2) Filter(0.0)==0.5
   */
  virtual Real ProcessSample(const Real input_sample) noexcept;

  virtual void ProcessBlock(std::span<const Real> input_data,
                            std::span<Real> output_data) noexcept;

  using Filter::ProcessBlock;

  /**
   Updates the filter coefficients. You can set how long it takes to
   update the coefficients (using linear interpolation between old and new
   impulse response). If an update is requested while another is already in
   progress, the new interpolation will pick up from where the old one left
   off to avoid audible artifacts.
   @param[in] update_length How many calls to Filter it takes to update the
   coefficients. A value of 0 means that the update is instantaneous. A call
   to Filter(const Real input) counts one, just like
   Filter(const std::vector<Real>& input).
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
#ifdef SAL_DSP_APPLE_ACCELERATE
  Real ProcessSampleAppleDsp(Real input_sample) noexcept;
  void ProcessBlockAppleDsp(std::span<const Real> input_data,
                            std::span<Real> output_data) noexcept;
#endif

  void GetExtendedInput(std::span<const Real> input_data,
                        std::span<Real> extended_input_data) {
    size_t num_samples = input_data.size();

    // Stage 1
    for (size_t i = 0; i < (size_t)counter_; ++i) {
      extended_input_data[i] = delay_line_[counter_ - i - 1];
    }

    // Stage 2
    // Starting from counter_ in padded_data
    // Ending in counter_+(length_-counter_-1)
    for (size_t i = counter_; i < (length_ - 1); ++i) {
      extended_input_data[i] = delay_line_[length_ - 1 - (i - counter_)];
    }

    // Stage 3
    // Append input signal
    for (size_t i = (length_ - 1); i < (length_ - 1 + num_samples); ++i) {
      extended_input_data[i] = input_data[i - (length_ - 1)];
    }
  }

  Real ProcessSampleStraight(Real input_sample) noexcept;

  /** Method called to slowly update the filter coefficients. It is called
   every time one of the Filter method is called and is activated only
   if updating_ = true. TODO: uniformise action between sequential and
   batch. */
  void UpdateCoefficients() noexcept;

  std::vector<Real> impulse_response_;
  std::vector<Real> impulse_response_old_;
  std::vector<Real> temp_buffer_;
  size_t update_index_;
  size_t update_length_;

  bool updating_;

  /* This is the current vector of coefficients. When the filter is updating
   this will in general be different from impulse_response_. */
  std::vector<Real> coefficients_;
  std::vector<Real> delay_line_;
  Int counter_;
  size_t length_;
};

class GainFilter : public Filter {
 public:
  GainFilter(const Real gain) : gain_(gain) {}

  virtual Real ProcessSample(const Real input) noexcept {
    return input * gain_;
  }

  virtual void ProcessBlock(std::span<const Real> input_data,
                            std::span<Real> output_data) noexcept {
    Multiply(input_data, gain_, output_data);
  }

  virtual void Reset() {}

 private:
  Real gain_;
};

class IdenticalFilter : public Filter {
 public:
  IdenticalFilter() {}

  virtual Real ProcessSample(const Real input) noexcept { return input; }

  virtual void ProcessBlock(std::span<const Real> input_data,
                            std::span<Real> output_data) noexcept {
    for (size_t i = 0; i < input_data.size(); ++i) {
      output_data[i] = input_data[i];
    }
  }

  virtual void Reset() {}
};

} // namespace dsp

} // namespace sal

#endif
