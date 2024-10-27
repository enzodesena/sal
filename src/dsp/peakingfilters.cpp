/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <limits>

#include "butter.h"
#include "comparisonop.h"
#include "iirfilter.h"
#include "peakingfilters.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

void PeakHighShelf::SetParameters(const Real fc, const Real Q,
                                  const int sample_rate) {
  Real omega = 2.0 * PI * fc / ((Real)sample_rate);
  cosOmega = cos(omega);
  alpha = sin(omega) / Q;
}

void PeakHighShelf::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = Avalue + 1.0;
  Real v2 = Avalue - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(Avalue) * alpha;  // 2 * sqrt(A) * alpha

  Real a0 = 1.0 / (v1 - v4 + v5);  // a[0] isn't used in GetOutput
  Real a1 = (2.0 * (v2 - v3)) * a0;
  Real a2 = (v1 - v4 - v5) * a0;

  Real b0 = Avalue * (v1 + v4 + v5) * a0;
  Real b1 = -2.0 * Avalue * (v2 + v3) * a0;
  Real b2 = Avalue * (v1 + v4 - v5) * a0;

  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
}

void PeakLowShelf::SetParameters(const Real fc, const Real Q,
                                 const int sample_rate) {
  Real omega = 2.0 * PI * fc / sample_rate;
  cosOmega = cos(omega);
  alpha = sin(omega) / Q;  // sin(omega) / (2 * Q) (factor of two cancelled out)
}

void PeakLowShelf::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = Avalue + 1.0;
  Real v2 = Avalue - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(Avalue) * alpha;  // 2 * sqrt(A) * alpha

  Real a0 = 1.0 / (v1 + v4 + v5);  // a[0] isn't used in GetOutput
  Real a1 = (-2.0 * (v2 + v3)) * a0;
  Real a2 = (v1 + v4 - v5) * a0;

  Real b0 = Avalue * (v1 - v4 + v5) * a0;
  Real b1 = 2.0 * Avalue * (v2 - v3) * a0;
  Real b2 = Avalue * (v1 - v4 - v5) * a0;

  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
}

void PeakingFilter::SetParameters(const Real fc, const Real Q,
                                  const int sample_rate) {
  Real omega = 2.0 * PI * fc / ((Real)sample_rate);
  cosOmega = -2.0 * cos(omega);
  alpha = sin(omega) / (2.0 * Q);
}

void PeakingFilter::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = alpha * Avalue;
  Real v2 = alpha / Avalue;

  Real a0 = 1.0 / (1 + v2);  // a[0] isn't used in GetOutput
  Real a1 = cosOmega * a0;
  Real a2 = (1 - v2) * a0;

  Real b0 = (1 + v1) * a0;
  Real b1 = a1;
  Real b2 = (1 - v1) * a0;

  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
}
} // namespace dsp

} // namespace sal
