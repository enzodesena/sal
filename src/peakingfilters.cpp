/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "iirfilter.h"
#include "comparisonop.h"
#include "peakingfilters.h"
#include "vectorop.h"
#include "butter.h"
#include <limits>

namespace mcl {
  
void PeakHighShelf::SetParameters(const Real fc, const Real Q, const int sample_rate) {
  Real omega = 2.0 * PI * fc / ((Real) sample_rate);
  cosOmega = cos(omega);
  alpha = sin(omega) / Q;
}


void PeakHighShelf::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = Avalue + 1.0;
  Real v2 = Avalue - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(Avalue) * alpha; // 2 * sqrt(A) * alpha

  Real a0 = 1.0 / (v1 - v4 + v5); // a[0] isn't used in GetOutput
  Real a1 = (2.0 * (v2 - v3)) * a0;
  Real a2 = (v1 - v4 - v5) * a0;

  Real b0 = Avalue * (v1 + v4 + v5) * a0;
  Real b1 = -2.0 * Avalue * (v2 + v3) * a0;
  Real b2 = Avalue * (v1 + v4 - v5) * a0;
  
  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
}

void PeakLowShelf::SetParameters(const Real fc, const Real Q, const int sample_rate) {
  Real omega = 2.0 * PI * fc / sample_rate;
  cosOmega = cos(omega);
  alpha = sin(omega) / Q; // sin(omega) / (2 * Q) (factor of two cancelled out)
}


void PeakLowShelf::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = Avalue + 1.0;
  Real v2 = Avalue - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(Avalue) * alpha; // 2 * sqrt(A) * alpha

  Real a0 = 1.0 / (v1 + v4 + v5); // a[0] isn't used in GetOutput
  Real a1 = (-2.0 * (v2 + v3)) * a0;
  Real a2 = (v1 + v4 - v5) * a0;

  Real b0 = Avalue * (v1 - v4 + v5) * a0;
  Real b1 = 2.0 * Avalue * (v2 - v3) * a0;
  Real b2 = Avalue * (v1 - v4 - v5) * a0;
  
  
  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
}


void Peaking::SetParameters(const Real fc, const Real Q, const int sample_rate) {
  Real omega = 2.0 * PI * fc / ((Real) sample_rate);
  cosOmega = -2.0 * cos(omega);
  alpha = sin(omega) / (2.0 * Q);
}


void Peaking::UpdateGain(const Real g) {
  Real Avalue = sqrt(g);
  Real v1 = alpha * Avalue;
  Real v2 = alpha / Avalue;
  
  Real a0 = 1.0 / (1 + v2); // a[0] isn't used in GetOutput
  Real a1 = cosOmega * a0;
  Real a2 = (1 - v2) * a0;

  Real b0 = (1 + v1) * a0;
  Real b1 = a1;
  Real b2 = (1 - v1) * a0;
  
  std::vector<Real> A = {1.0, a1, a2};
  std::vector<Real> B = {b0, b1, b2};
  SetCoefficients(B, A);
  
}
} // namespace mcl




