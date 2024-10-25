/*
 MCL
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.
 
 Authors:
 Joshua Mannall
 Enzo De Sena, enzodesena@gmail.com
 */

#ifndef MCL_PEAKINGFILTERS_H
#define MCL_PEAKINGFILTERS_H

#include "iirfilter.h"
#include "vectorop.h"
#include "constants.h"

namespace mcl {

/**
* Class that implements a 2nd order high shelf IIR filter (used by GraphicEQ)
*/
class PeakHighShelf : public IirFilter {
public:
  /**
  * Constructor that initialises an 2nd order high shelf filter with a given cut off frequency
  *
  * @param fc The cut off frequency of the filter
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakHighShelf(const Real fc, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
  }

  /**
  * Constructor that initialises an 2nd order high shelf filter with a given cut off frequency and shelf gain
  *
  * @param fc The cut off frequency of the filter
  * @param g The shelf gain of the filter (linear)
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakHighShelf(const Real fc, const Real g, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
    UpdateGain(g);
  }

  /**
  * Default deconstructor
  */
  ~PeakHighShelf() {};

  /**
  * Updates the gain of the high shelf filter
  *
  * @param g The shelf gain of the filter (linear)
  */
  void UpdateGain(const Real g);

private:
  /**
  * Sets the cut off frequency of the high shelf filter
  *
  * @param fs The cut off frequency of the filter
  * @param Q The quality factor of the filter
  */
  void SetParameters(const Real fc, const Real Q, const int sample_rate);

  Real cosOmega;
  Real alpha;
};

/**
* Class that implements a 2nd order low shelf IIR filter (used by GraphicEQ)
*/
class PeakLowShelf : public IirFilter {
public:
  /**
  * Constructor that initialises an 2nd order low shelf filter with a given cut off frequency
  *
  * @param fc The cut off frequency of the filter
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakLowShelf(const Real fc, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
  }

  /**
  * Constructor that initialises an 2nd order low shelf filter with a given cut off frequency and shelf gain
  *
  * @param fc The cut off frequency of the filter
  * @param g The shelf gain of the filter (linear)
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakLowShelf(const Real fc, const Real g, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
    UpdateGain(g);
  }

  /**
  * Default deconstructor
  */
  ~PeakLowShelf() {};

  /**
  * Updates the gain of the low shelf filter
  *
  * @param g The shelf gain of the filter (linear)
  */
  void UpdateGain(const Real g);

private:
  /**
  * Sets the cut off frequency of the low shelf filter
  *
  * @param fs The cut off frequency of the filter
  * @param Q The quality factor of the filter
  */
  void SetParameters(const Real fc, const Real Q, const int sample_rate);

  Real cosOmega;
  Real alpha;
};

/**
* Class that implements a 2nd order peaking IIR filter (used by GraphicEQ)
*/
class PeakingFilter : public IirFilter {
public:
  /**
  * Constructor that initialises an 2nd order peaking filter with a given cut off frequency
  *
  * @param fc The center frequency of the filter
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakingFilter(const Real fc, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
  }

  /**
  * Constructor that initialises an 2nd order peaking filter with a given cut off frequency and gain
  *
  * @param fc The center frequency of the filter
  * @param g The gain of the filter (linear)
  * @param Q The quality factor of the filter
  * @param sample_rate The sample rate for calculating filter coefficients
  */
  PeakingFilter(const Real fc, const Real g, const Real Q, const int sample_rate) : IirFilter(mcl::Zeros<Real>(3), mcl::Zeros<Real>(3)) {
    SetParameters(fc, Q, sample_rate);
    UpdateGain(g);
  }

  /**
  * Default deconstructor
  */
  ~PeakingFilter() {};

  /**
  * Updates the gain of the peaking filter
  *
  * @param g The gain of the filter (linear)
  */
  void UpdateGain(const Real g);

private:
  /**
  * Sets the center frequency of the peaking filter
  *
  * @param fs The center frequency of the filter
  * @param Q The quality factor of the filter
  */
  void SetParameters(const Real fc, const Real Q, const int sample_rate);

  Real cosOmega;
  Real alpha;
};
  
} // namespace mcl

#endif
