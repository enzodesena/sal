/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "iirfilter.h"
#include "comparisonop.h"
#include "vectorop.h"
#include "butter.h"
#include <limits>

namespace mcl {
  
void PeakHighShelf::SetParameters(const Real fc, const Real Q) {
  Real omega = PI_2 * fc * T;
  cosOmega = cos(omega);
  alpha = sin(omega) / Q;
}

////////////////////////////////////////

void PeakHighShelf::UpdateGain(const Real g) {
  Real A = sqrt(g);
  Real v1 = A + 1.0;
  Real v2 = A - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(A) * alpha; // 2 * sqrt(A) * alpha

  a[0] = 1.0 / (v1 - v4 + v5); // a[0] isn't used in GetOutput
  a[1] = (2.0 * (v2 - v3)) * a[0];
  a[2] = (v1 - v4 - v5) * a[0];

  b[0] = A * (v1 + v4 + v5) * a[0];
  b[1] = -2.0 * A * (v2 + v3) * a[0];
  b[2] = A * (v1 + v4 - v5) * a[0];
}

////////////////////////////////////////

void PeakLowShelf::SetParameters(const Real fc, const Real Q)
{
  Real omega = PI_2 * fc * T;
  cosOmega = cos(omega);
  alpha = sin(omega) / Q; // sin(omega) / (2 * Q) (factor of two cancelled out)
}

////////////////////////////////////////

void PeakLowShelf::UpdateGain(const Real g)
{
  Real A = sqrt(g);
  Real v1 = A + 1.0;
  Real v2 = A - 1.0;
  Real v3 = v1 * cosOmega;
  Real v4 = v2 * cosOmega;
  Real v5 = sqrt(A) * alpha; // 2 * sqrt(A) * alpha

  a[0] = 1.0 / (v1 + v4 + v5); // a[0] isn't used in GetOutput
  a[1] = (-2.0 * (v2 + v3)) * a[0];
  a[2] = (v1 + v4 - v5) * a[0];

  b[0] = A * (v1 - v4 + v5) * a[0];
  b[1] = 2.0 * A * (v2 - v3) * a[0];
  b[2] = A * (v1 - v4 - v5) * a[0];
}

////////////////////////////////////////

void PeakingFilter::SetParameters(const Real fc, const Real Q)
{
  Real omega = PI_2 * fc * T;
  cosOmega = -2.0 * cos(omega);
  alpha = sin(omega) / (2.0 * Q);
}

////////////////////////////////////////

void PeakingFilter::UpdateGain(const Real g)
{
  Real A = sqrt(g);
  Real v1 = alpha * A;
  Real v2 = alpha / A;

  a[0] = 1.0 / (1 + v2); // a[0] isn't used in GetOutput
  a[1] = cosOmega * a[0];
  a[2] = (1 - v2) * a[0];

  b[0] = (1 + v1) * a[0];
  b[1] = a[1];
  b[2] = (1 - v1) * a[0];
}

  
} // namespace mcl




