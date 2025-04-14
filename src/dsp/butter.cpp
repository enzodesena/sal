/*
 butter.cpp
 MCL

 Authors: user261002 @ stackoverflow.com
 Authors: Enzo De Sena, enzodesena@gmail.com

 Adapted from
 "http://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c"
 */

#include <math.h>

#include <complex>
#include <vector>

#include "butter.h"
#include "dsptypes.h"

namespace sal {

namespace dsp {

#define N \
  10  // The number of images which construct a time series for each pixel

IirFilter Butter(const Int order, const Real w_low, const Real w_high) {
  std::vector<double> DenC = ComputeDenCoeffs((int)order, w_low, w_high);
  std::vector<double> NumC = ComputeNumCoeffs((int)order, w_low, w_high, DenC);
  std::vector<Real> denominator(DenC.begin(), DenC.end());
  std::vector<Real> numerator(NumC.begin(), NumC.end());

  return IirFilter(numerator, denominator);
}

IirFilter OctaveFilter(const Int order, const Real center_frequency,
                       const Real sampling_frequency) {
  // beta = pi/2/N/sin(pi/2/N);
  Real beta = PI / 2.0 / ((Real)order) / sin(PI / 2.0 / ((Real)order));

  // alpha = (1+sqrt(1+8*beta^2))/4/beta;
  Real alpha = (1.0 + sqrt(1.0 + 8.0 * pow(beta, 2.0))) / 4.0 / beta;

  // W1 = Fc/(Fs/2)*sqrt(1/2)/alpha;
  Real W1 =
      center_frequency / (sampling_frequency / 2.0) * sqrt(1.0 / 2.0) / alpha;

  // W2 = Fc/(Fs/2)*sqrt(2)*alpha;
  Real W2 = center_frequency / (sampling_frequency / 2.0) * sqrt(2.0) * alpha;

  // [B,A] = butter(N,[W1,W2]);
  return Butter(order, W1, W2);
}

IirFilterBank OctaveFilterBank(const Int order, const Int num_bands,
                               const Real starting_frequency,
                               const Real sampling_frequency) {
  Real current_frequency = starting_frequency;
  std::vector<IirFilter> filters;
  for (Int i = 0; i < num_bands; ++i) {
    dsp::IirFilter octave_filter =
        dsp::OctaveFilter(order, current_frequency, sampling_frequency);
    filters.push_back(octave_filter);
    current_frequency = current_frequency * 2.0;
  }
  return IirFilterBank(filters);
}

std::vector<double> ComputeLP(int FilterOrder) {
  int m;
  int i;

  std::vector<double> NumCoeffs(FilterOrder + 1);

  NumCoeffs.at(0) = 1;
  NumCoeffs.at(1) = FilterOrder;
  m = FilterOrder / 2;
  for (i = 2; i <= m; ++i) {
    NumCoeffs.at(i) = (double)(FilterOrder - i + 1) * NumCoeffs.at(i - 1) / i;
    NumCoeffs.at(FilterOrder - i) = NumCoeffs.at(i);
  }

  NumCoeffs.at(FilterOrder - 1) = FilterOrder;
  NumCoeffs.at(FilterOrder) = 1;

  return NumCoeffs;
}

std::vector<double> ComputeHP(int FilterOrder) {
  int i;

  std::vector<double> NumCoeffs = ComputeLP(FilterOrder);
  // if(NumCoeffs == NULL ) return( NULL );

  for (i = 0; i <= FilterOrder; ++i)
    if (i % 2) NumCoeffs.at(i) = -NumCoeffs.at(i);

  return NumCoeffs;
}

std::vector<double> TrinomialMultiply(int FilterOrder, std::vector<double> b,
                                      std::vector<double> c) {
  int i, j;

  std::vector<double> RetVal(4 * FilterOrder);
  // if( RetVal == NULL ) return( NULL );

  RetVal.at(2) = c.at(0);
  RetVal.at(3) = c.at(1);
  RetVal.at(0) = b.at(0);
  RetVal.at(1) = b.at(1);

  for (i = 1; i < FilterOrder; ++i) {
    RetVal.at(2 * (2 * i + 1)) +=
        c.at(2 * i) * RetVal.at(2 * (2 * i - 1)) -
        c.at(2 * i + 1) * RetVal.at(2 * (2 * i - 1) + 1);
    RetVal.at(2 * (2 * i + 1) + 1) +=
        c.at(2 * i) * RetVal.at(2 * (2 * i - 1) + 1) +
        c.at(2 * i + 1) * RetVal.at(2 * (2 * i - 1));

    for (j = 2 * i; j > 1; --j) {
      RetVal.at(2 * j) += b.at(2 * i) * RetVal.at(2 * (j - 1)) -
                          b.at(2 * i + 1) * RetVal.at(2 * (j - 1) + 1) +
                          c.at(2 * i) * RetVal.at(2 * (j - 2)) -
                          c.at(2 * i + 1) * RetVal.at(2 * (j - 2) + 1);
      RetVal.at(2 * j + 1) += b.at(2 * i) * RetVal.at(2 * (j - 1) + 1) +
                              b.at(2 * i + 1) * RetVal.at(2 * (j - 1)) +
                              c.at(2 * i) * RetVal.at(2 * (j - 2) + 1) +
                              c.at(2 * i + 1) * RetVal.at(2 * (j - 2));
    }

    RetVal.at(2) += b.at(2 * i) * RetVal.at(0) -
                    b.at(2 * i + 1) * RetVal.at(1) + c.at(2 * i);
    RetVal.at(3) += b.at(2 * i) * RetVal.at(1) +
                    b.at(2 * i + 1) * RetVal.at(0) + c.at(2 * i + 1);
    RetVal.at(0) += b.at(2 * i);
    RetVal.at(1) += b.at(2 * i + 1);
  }

  return RetVal;
}

std::vector<double> ComputeNumCoeffs(int FilterOrder, double Lcutoff,
                                     double Ucutoff, std::vector<double> DenC) {
  ASSERT(std::isgreaterequal(Lcutoff, 0.0) && std::isless(Ucutoff, 1.0));

  int i;

  std::vector<double> NumCoeffs(2 * FilterOrder + 1);

  std::vector<std::complex<double> > NormalizedKernel(2 * FilterOrder + 1);
  // if( NormalizedKernel == NULL ) return( NULL );

  std::vector<double> TCoeffs = ComputeHP(FilterOrder);
  // if( TCoeffs == NULL ) return( NULL );

  for (i = 0; i < FilterOrder; ++i) {
    NumCoeffs.at(2 * i) = TCoeffs.at(i);
    NumCoeffs.at(2 * i + 1) = 0.0;
  }
  NumCoeffs.at(2 * FilterOrder) = TCoeffs.at(FilterOrder);
  std::vector<double> cp(2);
  double Bw, Wn;
  cp.at(0) = 2 * 2.0 * tan(PI * Lcutoff / 2.0);
  cp.at(1) = 2 * 2.0 * tan(PI * Ucutoff / 2.0);

  Bw = cp.at(1) - cp.at(0);
  // center frequency
  Wn = sqrt(cp.at(0) * cp.at(1));
  Wn = 2 * atan2(Wn, 4);

  const std::complex<double> result = std::complex<double>(-1, 0);

  for (int k = 0; k < (2 * FilterOrder + 1); k++) {
    NormalizedKernel.at(k) = std::exp(-sqrt(result) * Wn * ((double)k));
  }
  double b = 0;
  double den = 0;
  for (int d = 0; d < (2 * FilterOrder + 1); d++) {
    b += real(NormalizedKernel.at(d) * NumCoeffs.at(d));
    den += real(NormalizedKernel.at(d) * DenC.at(d));
  }
  for (int c = 0; c < (2 * FilterOrder + 1); c++) {
    NumCoeffs.at(c) = (NumCoeffs.at(c) * den) / b;
  }

  return NumCoeffs;
}

std::vector<double> ComputeDenCoeffs(int FilterOrder, double Lcutoff,
                                     double Ucutoff) {
  if ((Lcutoff < 0.0) | (Ucutoff > 1.0)) {
    ASSERT(false);
  }

  int k;                // loop variables
  double theta;         // PI * (Ucutoff - Lcutoff) / 2.0
  double cp;            // cosine of phi
  double st;            // sine of theta
  double ct;            // cosine of theta
  double s2t;           // sine of 2*theta
  double c2t;           // cosine 0f 2*theta
  double PoleAngle;     // pole angle
  double SinPoleAngle;  // sine of pole angle
  double CosPoleAngle;  // cosine of pole angle
  double a;             // workspace variables

  cp = cos(PI * (Ucutoff + Lcutoff) / 2.0);
  theta = PI * (Ucutoff - Lcutoff) / 2.0;
  st = sin(theta);
  ct = cos(theta);
  s2t = 2.0 * st * ct;        // sine of 2*theta
  c2t = 2.0 * ct * ct - 1.0;  // cosine of 2*theta

  std::vector<double> RCoeffs(2 * FilterOrder);  // z^-2 coefficients
  std::vector<double> TCoeffs(2 * FilterOrder);  // z^-1 coefficients

  for (k = 0; k < FilterOrder; ++k) {
    PoleAngle = PI * (double)(2 * k + 1) / (double)(2 * FilterOrder);
    SinPoleAngle = sin(PoleAngle);
    CosPoleAngle = cos(PoleAngle);
    a = 1.0 + s2t * SinPoleAngle;
    RCoeffs.at(2 * k) = c2t / a;
    RCoeffs.at(2 * k + 1) = s2t * CosPoleAngle / a;
    TCoeffs.at(2 * k) = -2.0 * cp * (ct + st * SinPoleAngle) / a;
    TCoeffs.at(2 * k + 1) = -2.0 * cp * st * CosPoleAngle / a;
  }

  std::vector<double> DenomCoeffsTemp =
      TrinomialMultiply(FilterOrder, TCoeffs, RCoeffs);

  std::vector<double> DenomCoeffs(2 * FilterOrder + 1);

  DenomCoeffs.at(0) = 1.0;
  DenomCoeffs.at(1) = DenomCoeffsTemp.at(0);
  DenomCoeffs.at(2) = DenomCoeffsTemp.at(2);
  for (k = 3; k <= 2 * FilterOrder; ++k) {
    DenomCoeffs.at(k) = DenomCoeffsTemp.at(2 * k - 2);
  }

  return DenomCoeffs;
}

} // namespace dsp

} // namespace sal
