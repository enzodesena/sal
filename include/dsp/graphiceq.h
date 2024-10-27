/*
 MCL
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors:
 Joshua Mannall
 Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_GRAPHICEQ_H
#define MCL_GRAPHICEQ_H

#include <vector>

#include "digitalfilter.h"
#include "matrixop.h"
#include "mcltypes.h"
#include "peakingfilters.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

/**
 * Class that implements a graphic equaliser
 */
class GraphicEq : public Filter {
 public:
  /**
   * Constructor that initialises the GraphicEq with given frequency bands, Q
   * factor and sample rate
   *
   * @param fc The filter band center frequencies
   * @param Q The Q factor for the filters
   */
  GraphicEq(const std::vector<Real>& fc, const Real Q,
            const Real sampling_frequency);

  /**
   * Constructor that initialises the GraphicEq with given gains, frequency
   * bands, Q factor and sample rate
   *
   * @param gain The gain for each filter band
   * @param fc The filter band center frequencies
   * @param Q The Q factor for the filters
   */
  GraphicEq(const std::vector<Real>& gain, const std::vector<Real>& fc,
            const Real Q, const Real sampling_frequency);

  /**
   * Default deconstructor
   */
  ~GraphicEq() {};

  /**
   * Initialises the filter parameters with the given gains
   *
   * @param gain The gains for each filter band
   */
  void InitParameters(const std::vector<Real>& gain);

  /**
   * Interpolates between the current and target gains and updates the filter
   * parameters
   *
   * @param lerp_factor The linear interpolation factor
   */
  inline void UpdateParameters(const Real lerp_factor) {
    if (equal_) {
      return;
    }
    if (IsEqual(current_gain_, target_gain_)) {
      current_gain_ = target_gain_;
      equal_ = true;
      UpdateParameters();
    } else {
      // TODO: implement Lerp
      //      Lerp(current_gain_, target_gain_, lerp_factor);
      current_gain_ = target_gain_;
      UpdateParameters();
    }
  }

  /**
   * Sets the current gain of the filter bank
   *
   * @param gains The new gains for each filter band
   */
  void SetGain(const std::vector<Real>& gains);

  /**
   * Returns the output of the GraphicEq given an input
   *
   * @param input The input to the GraphicEq
   * @return The output of the GraphicEq
   */
  virtual Real ProcessSample(const Real input) noexcept;

  /**
   * Processes an input buffer and updates the output buffer
   *
   * @param input_data The input buffer
   * @param output_data The output buffer
   * @param num_samples The number of frames in the buffer
   */
  virtual void ProcessBlock(const Real* input_data, const Int num_samples,
                            Real* output_data) noexcept;

  /**
   * Resets the filter buffers
   */
  inline void ClearBuffers() {
    low_shelf_.Reset();
    for (PeakingFilter& filter : peaking_filters_) filter.Reset();
    high_shelf_.Reset();
  }

  void PrintCoefficients();

  virtual void Reset();

  static bool Test() noexcept;

 private:
  /**
   * Updates the filter parameters using the current gain
   */
  void UpdateParameters();

  /**
   * Initialises the filter bands with the given center frequencies and Q factor
   *
   * @param fc The filter band center frequencies
   * @param Q The Q factor
   * @param sampling_frequency The sample rate for calculating the filter coefficients
   */
  void InitFilters(const std::vector<Real>& fc, const Real Q,
                   const Real sampling_frequency);

  /**
   * Initialises the matrix used to calculate the input gains
   *
   * @param fc The filter band center frequencies
   */
  void InitMatrix(const std::vector<Real>& fc, const Real sampling_frequency);

  /**
   * Number of filters
   */
  int num_filters_;

  /**
   * Filters
   */
  PeakLowShelf low_shelf_;
  PeakHighShelf high_shelf_;
  std::vector<PeakingFilter> peaking_filters_;

  /**
   * Gain matrix
   */
  Matrix<Real> mat_;

  /**
   * The stored input, target and current gains
   */
  std::vector<Real> last_input_;
  std::vector<Real> target_gain_;
  std::vector<Real> current_gain_;

  /**
   * Rowvecs used to store dB and gain values during SetGain()
   */
  std::vector<Real> db_gain_;
  std::vector<Real> input_gain_;

  /**
   * Booleans to skip unnecessary update and audio calculations
   */
  bool equal_;
  bool valid_;
};

} // namespace dsp

} // namespace sal

#endif
