/*
 vectorop.cpp
 MCL
 
 This contains vector operations.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "vectorop.h"
#include <fstream>
#include "mcltypes.h"
#include "pointwiseop.h"
#include "transformop.h"
#include <cmath>
#include "comparisonop.h"
#include <vector>
#include <cassert>



namespace mcl {
  
  
std::vector<Real> LinSpace(Real min, Real max, UInt num_elements) {
  // This is to mimic Matlab's behaviour.
  if (num_elements <= 1) { return UnaryVector(max); }
  
  Real interval = max-min;
  Real slot = interval / ((Real) (num_elements-1));
  
  std::vector<Real> output(num_elements);
  for (UInt i=0; i<num_elements; ++i) { 
    output[i] = min + slot*((Real) i);
  }
  return output;
}


std::vector<Real> Ones(UInt length) {
  return std::vector<Real>(length, (Real) 1.0);
}
  
Real Sum(const std::vector<Real>& input) {
  Real output(0.0);
  for (UInt i=0; i<input.size(); ++i) { output += input[i]; }
  return output;
}

Real Mean(const std::vector<Real>& input) {
  return Sum(input) / ((Real) input.size());
}
  
Real Mean(const std::vector<Real>& input,
          const std::vector<Real>& weights) {
  if (input.size() != weights.size()) { throw_line(); }
  if (! IsNonNegative(weights)) { throw_line(); }
  
  // Normalise the weigths
  std::vector<Real> normalised_weights = Multiply(weights, 1.0/Sum(weights));
  assert(IsEqual(Sum(normalised_weights), 1.0));
  return Sum(Multiply(input, normalised_weights));
}

Real Geomean(const std::vector<Real>& input) {
  // TODO: Throw error for negative entries
  return Pow(Prod(input), 1.0/((Real)input.size()));
}

Real Std(const std::vector<Real>& input) {
  return sqrt(Var(input));
}
  
Real Var(const std::vector<Real>& input) {
  Real mean = Mean(input);
  Real output(0.0);
  for (UInt i=0; i<input.size(); ++i) { output += pow(input[i] - mean,2.0); }
  return output/((Real) (input.size()-1));
}
  
Real Var(const std::vector<Real>& input, const std::vector<Real>& weights) {
  if(! IsNonNegative(weights)) { throw_line(); }
  
  Real weighted_mean = Mean(input, weights);
  std::vector<Real> temp = Pow(Add(input, -weighted_mean), 2.0);
  std::vector<Real> norm_weights = Multiply(weights, 1.0/Sum(weights));
  
  return (Sum(Multiply(norm_weights, temp)));
}
  
std::vector<Real> XCorr(const std::vector<Real>& vector_a,
                        const std::vector<Real>& vector_b) {
  // TODO: implement for different sizes
  if (vector_a.size() != vector_b.size()) { throw_line(); }
  
  UInt M = vector_a.size();
  
  UInt n_fft = pow(2.0, NextPow2(2*M-1));
  
  std::vector<Complex> x = Fft(ComplexVector(vector_a), n_fft);
  std::vector<Complex> y = Fft(ComplexVector(vector_b), n_fft);
  
  std::vector<Complex> c = Ifft(Multiply(x, Conj(y)), n_fft);
  
  // Ignore residual imaginary part
  std::vector<Real> c_real = RealPart(c);
  std::vector<Real> output = Zeros<Real>(2*M-1);
  UInt end = c_real.size(); // Matlab's index of the last element of c
  UInt maxlag = M-1;
  // c = [c(end-maxlag+1:end,:);c(1:maxlag+1,:)];
  UInt k = 0; // running index
  for (UInt i=end-maxlag+1-1; i<=(end-1); ++i) { output[k++] = c_real[i]; }
  for (UInt i=1-1; i<=(maxlag+1-1); ++i) { output[k++] = c_real[i]; }
  
  return output;
}
  
  
  
std::vector<std::string> Split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}
  
std::vector<Complex> Poly(const std::vector<Complex> roots) {
  UInt n(Length(roots));
  std::vector<Complex> output = Zeros<Complex>(n+1);
  // c = [1 zeros(1,n,class(x))];
  output[0] = 1.0;
  
  // for j=1:n
  for (UInt j=1; j<=n; ++j) {
    // c(2:(j+1)) = c(2:(j+1)) - e(j).*c(1:j);
    std::vector<Complex> temp(output);
    for (UInt i=2; i<=(j+1); ++i) {
      output[i-1] = temp[i-1] - roots[j-1]*temp[i-2];
    }
  }
  
  return output;
}
  
std::vector<Complex> Poly(const std::vector<Real> roots) {
  return Poly(ComplexVector(roots));
}
  
std::vector<Real>
ColonOperator(const Real from, const Real step, const Real to) {
  if (step <= 0) { throw_line(); }
  std::vector<Real> output;
  output.push_back(from);
  UInt i = 0;
  while (IsSmallerOrEqual(output[i] + step, to)) {
    output.push_back(output[i++] + step);
  }
  return output;
}

std::vector<Real> TukeyWin(const UInt length, const Real ratio) {
  if (length == 1) { return UnaryVector(1.0); }
  if (ratio <= 0) { return Ones(length); }
  else if (ratio >= 1.0) { return Hann(length); }
  else {
    std::vector<Real> t = LinSpace(0.0, 1.0, length);
    // Defines period of the taper as 1/2 period of a sine wave.
    Real per = ratio/2.0;
    Int tl = floor(per*(((Real) length)-1.0))+1;
    Int th = length-tl+1;
    // Window is defined in three sections: taper, constant, taper
    // w1 = ((1+cos(PI/per*(t(1:tl) - per)))/2);
    std::vector<Real> w = Ones(length);
    for (UInt i=0; i<tl; ++i) {
      w[i] = (1.0+cos(PI/per*(t[i] - per)))/2.0;
    }
    for (UInt i=th-1; i<length; ++i) {
      w[i] = (1.0+cos(PI/per*(t[i] - 1.0 + per)))/2.0;
    }
    return w;
  }
}
  
std::vector<Real> Hann(const UInt length) {
  std::vector<Real> w = Ones(length);
  for (UInt i=0; i<length; ++i) {
    w[i] = (1.0-cos(2.0*PI*((Real)i)/((Real)(length-1))))/2.0;
  }
  return w;
}
  
Real Norm(const std::vector<Real>& vector, Real l_norm) {
  const UInt num_elements = vector.size();
  Real output = 0.0;
  for (UInt i=0; i<num_elements; ++i) {
    output += pow(fabs(vector[i]), l_norm);
  }
  return pow(output, 1.0/l_norm);
}
  
bool IsNonNegative(const std::vector<Real>& input) {
  const UInt num_elem = input.size();
  for (UInt i=0; i<num_elem; ++i) {
    if (input[i] < 0.0) { return false; }
  }
  return true;
}
  
Matrix<Real> Cov(const std::vector<Real>& x, const std::vector<Real>& y) {
  std::vector<std::vector<Real> > input(2);
  input[0] = x;
  input[1] = y;
  return Cov(input);
}
  
Matrix<Real> Cov(const std::vector<std::vector<Real> >& input) {
  const UInt N = input.size();
  Matrix<Real> output(N, N);
  for (UInt i=0; i<N; ++i) {
    for (UInt j=0; j<N; ++j) {
      output.set_element(i, j, CovElement(input[i], input[j]));
    }
  }
  return output;
}
  
Real CovElement(const std::vector<Real>& x, const std::vector<Real>& y) {
  const UInt N = x.size();
  if (N != y.size()) { throw_line(); }
  
  Real output = Sum(Multiply(Add(x, -Mean(x)), Add(y, -Mean(y))));
  // In case N>1 use the unbiased estimator of covariance.
  output = (N > 1) ? output/((Real) (N-1)) : output/((Real) (N));
  return output;
}
  
std::vector<Real> CumSum(const std::vector<Real>& input) {
  const UInt N = input.size();
  std::vector<Real> output(input.size());
  
  output[N-1] = Sum(input);
  for (Int i=N-2; i>=0; --i) { output[i] = output[i+1]-input[i+1]; }
  
  return output;
}

  
  
} // namespace mcl
