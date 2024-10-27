/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <complex>
#include <vector>

#include "kissfft.hh"
#include "mcltypes.h"
#include "pointwiseop.h"
#include "transformop.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

std::vector<Complex> Fft(const std::vector<Complex>& input,
                         Int n_point) noexcept {
  std::vector<Complex> padded = ZeroPad(input, n_point);
  kissfft<Real> fft((int)n_point, false);
  std::vector<Complex> outbuf(n_point);

  fft.transform(&padded[0], &outbuf[0]);

  return outbuf;
}

std::vector<Complex> Ifft(const std::vector<Complex>& input,
                          Int n_point) noexcept {
  std::vector<Complex> padded = ZeroPad(input, n_point);
  kissfft<Real> fft((int)n_point, true);
  std::vector<Complex> outbuf(n_point);

  fft.transform(&padded[0], &outbuf[0]);

  return Multiply(outbuf, Complex(1.0 / n_point, 0.0));
}

std::vector<Complex> Hilbert(const std::vector<Real>& input) noexcept {
  Int n = input.size();

  std::vector<Complex> x = Fft(ComplexVector(input), n);

  std::vector<Complex> h = Zeros<Complex>(n);

  // if (n > 0 && 2*fix(n/2)) == n
  if (n > 0 && 2 * floor(n / 2) == n) {
    // even and nonempty
    h[0] = 1.0;  // h([1 n/2+1]) = 1;
    h[n / 2] = 1.0;
    for (Int i = 1; i < (n / 2); ++i) {
      h[i] = 2.0;  // h(2:n/2) = 2;
    }
  } else if (n > 0) {
    // odd and nonempty
    h[0] = 1.0;  // h(1) = 1;
    for (Int i = 1; i < ((n + 1) / 2); ++i) {
      h[i] = 2.0;  // h(2:(n+1)/2) = 2;
    }
  }

  // x = ifft(x.*h(:,ones(1,size(x,2))));
  return Ifft(Multiply(x, h), n);
}

// Returns the real cepstrum of the real sequence X.
// Equivalent to Matlab's rceps(vector)
std::vector<Real> RCeps(const std::vector<Real>& x) noexcept {
  Int n = Length(x);
  std::vector<Real> fftxabs = Abs(Fft(ComplexVector(x), n));
  //  TODO: implement this check.
  //  if any(fftxabs == 0)
  //    error(generatemsgid('ZeroInFFT'),...
  //          'The Fourier transform of X contains zeros. Therefore, the real
  //          cepstrum of X does not exist.');
  //  end

  return RealPart(Ifft(ComplexVector(Log(fftxabs)), n));
}

// Returns the (unique) minimum-phase sequence that has the same real
// cepstrum as vector. Equivalent to Matlab's [~, out] = rceps(vector).
std::vector<Real> MinPhase(const std::vector<Real>& x) noexcept {
  Int n = Length(x);
  // odd = fix(rem(n,2));
  Int odd = Fix(Rem((Int)n, 2));

  //  wn = [1; 2*ones((n+odd)/2-1,1) ; ones(1-rem(n,2),1);
  //  zeros((n+odd)/2-1,1)];
  std::vector<Real> wn_1 = Concatenate(
      UnaryVector((Real)1.0), Multiply(Ones((n + odd) / 2 - 1), (Real)2.0));
  std::vector<Real> wn_2 = Concatenate(wn_1, Ones(1 - (Int)Rem((Int)n, 2)));
  std::vector<Real> wn = Concatenate(wn_2, Zeros<Real>((n + odd) / 2 - 1));

  // yhat(:) = real(ifft(exp(fft(wn.*xhat(:)))));
  return RealPart(Ifft(Exp(Fft(ComplexVector(Multiply(wn, RCeps(x))), n)), n));
}

std::vector<Complex> Rfft(const std::vector<Real>& input,
                          Int n_point) noexcept {
  return Elements(Fft(ConvertToComplex(input), n_point), 0,
                  (Int)floor(1.0 + ((double)n_point) / 2.0) - 1);
}

std::vector<std::vector<Complex> > Rfft(
    const std::vector<std::vector<Real> >& input, Int n_point) noexcept {
  std::vector<std::vector<Complex> > outputs;
  for (Int i = 0; i < (Int)input.size(); ++i) {
    outputs.push_back(Rfft(input[i], n_point));
  }
  return outputs;
}

std::vector<Real> Irfft(const std::vector<Complex>& input,
                        Int n_point) noexcept {
  // If n_point is even, then it includes the Nyquist term (the only
  // non-repeated term, together with DC) and is of dimension M=N/2 + 1
  // whereas if N is odd then there is no Nyquist term
  // and the input is of dimension M=(N+1)/2.
  Int M = (Rem(n_point, 2) == 0) ? (n_point / 2 + 1) : (n_point + 1) / 2;
  const std::vector<Complex> zero_padded = ZeroPad(input, M);
  std::vector<Complex> spectrum;
  if (Rem(n_point, 2) == 0) {  // If n_point is even
    spectrum = Concatenate(
        zero_padded, Flip(Conj(Elements(zero_padded, 1, n_point / 2 - 1))));
  } else {  // If n_point is odd
    spectrum = Concatenate(
        zero_padded,
        Flip(Conj(Elements(zero_padded, 1, (n_point + 1) / 2 - 1))));
  }
  ASSERT((Int)spectrum.size() == n_point);
  std::vector<Complex> output = Ifft(spectrum, n_point);
  ASSERT(IsReal(output));
  return RealPart(output);
}

std::vector<std::vector<Real> > Irfft(
    const std::vector<std::vector<Complex> >& input, Int n_point) noexcept {
  std::vector<std::vector<Real> > outputs;
  for (Int i = 0; i < (Int)input.size(); ++i) {
    outputs.push_back(Irfft(input[i], n_point));
  }
  return outputs;
}

std::vector<Real> XCorr(const std::vector<Real>& vector_a,
                        const std::vector<Real>& vector_b) {
  // TODO: implement for different sizes
  ASSERT(vector_a.size() == vector_b.size());

  Int M = (Int)vector_a.size();

  Int n_fft = (UInt)pow(2.0, NextPow2(2 * M - 1));

  std::vector<Complex> x = Fft(ComplexVector(vector_a), n_fft);
  std::vector<Complex> y = Fft(ComplexVector(vector_b), n_fft);

  std::vector<Complex> c = Ifft(Multiply(x, Conj(y)), n_fft);

  // Ignore residual imaginary part
  std::vector<Real> c_real = RealPart(c);
  std::vector<Real> output = Zeros<Real>(2 * M - 1);
  Int end = c_real.size();  // Matlab's index of the last element of c
  Int maxlag = M - 1;
  // c = [c(end-maxlag+1:end,:);c(1:maxlag+1,:)];
  Int k = 0;  // running index
  for (Int i = end - maxlag + 1 - 1; i <= (end - 1); ++i) {
    output[k++] = c_real[i];
  }
  for (Int i = 1 - 1; i <= (maxlag + 1 - 1); ++i) {
    output[k++] = c_real[i];
  }

  return output;
}

} // namespace dsp

} // namespace sal
