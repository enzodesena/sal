/*
 transformop.cpp
 Matlab Cpp Library (MCL)
 
 This contains transforming operations (fft, ifft, hilbert...)
 
 Authors: Enzo De Sena, enzodesena@me.com

 */

#include "transformop.h"
#include <complex>
#include "kissfft.hh"
#include "mcltypes.h"
#include "vectorop.h"
#include "pointwiseop.h"
#include <vector>



namespace mcl {

  
  
std::vector<Complex> Fft(const std::vector<Complex>& input, UInt n_point) {
  std::vector<Complex> padded = ZeroPad(input, n_point);
  kissfft<Real> fft((int) n_point, false);
  std::vector<Complex> outbuf(n_point);
  
  fft.transform(&padded[0], &outbuf[0]);
  
  return outbuf;
}

std::vector<Complex> Ifft(const std::vector<Complex>& input, UInt n_point) {
  std::vector<Complex> padded = ZeroPad(input, n_point);
  kissfft<Real> fft((int) n_point, true);
  std::vector<Complex> outbuf(n_point);
  
  fft.transform(&padded[0], &outbuf[0]);
  
  return Multiply(outbuf, Complex(1.0/n_point, 0.0));
}



std::vector<Complex> Hilbert(const std::vector<Real>& input) {
  UInt n = input.size();
  
  std::vector<Complex> x = Fft(ComplexVector(input), n);
  
  std::vector<Complex> h = Zeros<Complex>(n);
  
  // if (n > 0 && 2*fix(n/2)) == n
  if (n > 0 && 2*floor(n/2) == n) {
    // even and nonempty
    h[0] = 1.0;     // h([1 n/2+1]) = 1;
    h[n/2] = 1.0;
    for (UInt i=1; i<(n/2); ++i) {
      h[i] = 2.0;   // h(2:n/2) = 2;
    }
  }
  else if (n>0) {
    // odd and nonempty
    h[0] = 1.0;     // h(1) = 1;
    for (UInt i=1; i<((n+1)/2); ++i) {
      h[i] = 2.0;   // h(2:(n+1)/2) = 2;
    }
  }
  
  // x = ifft(x.*h(:,ones(1,size(x,2))));
  return Ifft(Multiply(x, h), n);
}
  
  

// Returns the real cepstrum of the real sequence X.
// Equivalent to Matlab's rceps(vector)
std::vector<Real> RCeps(const std::vector<Real>& x) {
  UInt n = Length(x);
  std::vector<Real> fftxabs = Abs(Fft(ComplexVector(x), n));
  //  TODO: implement this check.
  //  if any(fftxabs == 0)
  //    error(generatemsgid('ZeroInFFT'),...
  //          'The Fourier transform of X contains zeros. Therefore, the real cepstrum of X does not exist.');
  //  end
  
  return RealPart(Ifft(ComplexVector(Log(fftxabs)), n));
}

// Returns the (unique) minimum-phase sequence that has the same real 
// cepstrum as vector. Equivalent to Matlab's [~, out] = rceps(vector).
std::vector<Real> MinPhase(const std::vector<Real>& x) {
  UInt n = Length(x);
  // odd = fix(rem(n,2));
  UInt odd = Fix(Rem((Int) n,2));
  
  //  wn = [1; 2*ones((n+odd)/2-1,1) ; ones(1-rem(n,2),1); zeros((n+odd)/2-1,1)];
  std::vector<Real> wn_1 = Concatenate(UnaryVector((Real) 1.0), 
                                       Multiply(Ones((n+odd)/2-1), (Real) 2.0));
  std::vector<Real> wn_2 = Concatenate(wn_1, Ones(1-Rem((Int)n,2)));
  std::vector<Real> wn = Concatenate(wn_2, Zeros<Real>((n+odd)/2-1));
  
  // yhat(:) = real(ifft(exp(fft(wn.*xhat(:)))));
  return RealPart(Ifft(Exp(Fft(ComplexVector(Multiply(wn, RCeps(x))),n)),n));
}


  
} // namespace mcl