/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
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

#ifdef OSXIOS
#include <Accelerate/Accelerate.h>
#endif

namespace mcl {
  
MCL_API void Multiply(const Real* input_data,
                      const Int num_samples,
                      const Real gain,
                      Real* output_data) noexcept {
#ifdef OSXIOS
  #ifdef MCL_DATA_TYPE_DOUBLE
  vDSP_vmulD(input_data, 1,
             &gain, 0,
             output_data, 1,
             num_samples);
  #else
  vDSP_vmul(input_data, 1,
            &gain, 0,
            output_data, 1,
            num_samples);
  #endif
#else
  for (UInt i=0; i<num_samples; ++i) { output_data[i] = input_data[i]*gain; }
#endif
}
  
MCL_API void MultiplyAdd(const Real* input_data_mult,
                         const Real gain,
                         const Real* input_data_add,
                         const Int num_samples,
                         Real* output_data) noexcept {
#ifndef OSXIOS
  #ifdef MCL_DATA_TYPE_DOUBLE
  vDSP_vmaD(input_data_mult, 1,
            &gain, 0,
            input_data_add, 1,
            output_data, 1,
            num_samples);
  #else
  vDSP_vma(input_data_mult, 1,
           &gain, 0,
           input_data_add, 1,
           output_data, 1,
           num_samples);
  #endif
#else
  for (UInt i=0; i<num_samples; ++i) {
    output_data[i] = input_data_mult[i]*gain + input_data_add[i];
  }
#endif
}
  
std::vector<Real> LinSpace(Real min, Real max, UInt num_elements) noexcept {
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


std::vector<Real> Ones(UInt length) noexcept {
  return std::vector<Real>(length, (Real) 1.0);
}
  
Real Sum(const std::vector<Real>& input) noexcept {
  Real output(0.0);
  for (UInt i=0; i<input.size(); ++i) { output += input[i]; }
  return output;
}

Real Mean(const std::vector<Real>& input) noexcept {
  return Sum(input) / ((Real) input.size());
}
  
Real Mean(const std::vector<Real>& input,
          const std::vector<Real>& weights) noexcept {
  if (input.size() != weights.size()) { ASSERT(false); }
  if (! IsNonNegative(weights)) { ASSERT(false); }
  
  // Normalise the weigths
  std::vector<Real> normalised_weights = Multiply<Real>(weights, 1.0/Sum(weights));
  ASSERT(IsEqual(Sum(normalised_weights), 1.0));
  return Sum(Multiply(input, normalised_weights));
}

Real Geomean(const std::vector<Real>& input) noexcept {
  // TODO: Throw error for negative entries
  return Pow(Prod(input), 1.0/((Real)input.size()));
}

Real Std(const std::vector<Real>& input) noexcept {
  return sqrt(Var(input));
}
  
Real Var(const std::vector<Real>& input) noexcept {
  Real mean = Mean(input);
  Real output(0.0);
  for (UInt i=0; i<input.size(); ++i) { output += pow(input[i] - mean,2.0); }
  return output/((Real) (input.size()-1));
}
  
Real Var(const std::vector<Real>& input,
         const std::vector<Real>& weights) noexcept {
  if(! IsNonNegative(weights)) { ASSERT(false); }
  
  Real weighted_mean = Mean(input, weights);
  std::vector<Real> temp = Pow(Add(input, -weighted_mean), 2.0);
  std::vector<Real> norm_weights = Multiply<Real>(weights, 1.0/Sum(weights));
  
  return (Sum(Multiply(norm_weights, temp)));
}
  
std::vector<std::string> Split(const std::string &s, char delim) noexcept {
  std::vector<std::string> elems;
  std::stringstream ss(s);
  std::string item;
  while(std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}
  
std::vector<Complex> Poly(const std::vector<Complex> roots) noexcept {
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
  
std::vector<Complex> Poly(const std::vector<Real> roots) noexcept {
  return Poly(ComplexVector(roots));
}
  
std::vector<Real>
ColonOperator(const Real from, const Real step, const Real to) noexcept {
  if (islessequal(step, 0)) { ASSERT(false); }
  std::vector<Real> output;
  output.push_back(from);
  UInt i = 1;
  while (islessequal(((Real) i)*step+from, to)) {
    output.push_back(((Real) i++)*step+from);
  }
  return output;
}

std::vector<Real> TukeyWin(const UInt length, const Real ratio) noexcept {
  if (length == 1) { return UnaryVector<Real>(1.0); }
  if (ratio <= 0) { return Ones(length); }
  else if (ratio >= 1.0) { return Hann(length); }
  else {
    std::vector<Real> t = LinSpace(0.0, 1.0, length);
    // Defines period of the taper as 1/2 period of a sine wave.
    Real per = ratio/2.0;
    Int tl = (Int) floor(per*(((Real) length)-1.0))+1;
    Int th = length-tl+1;
    // Window is defined in three sections: taper, constant, taper
    // w1 = ((1+cos(PI/per*(t(1:tl) - per)))/2);
    std::vector<Real> w = Ones(length);
    for (Int i=0; i<tl; ++i) {
      w[i] = (1.0+cos(PI/per*(t[i] - per)))/2.0;
    }
    for (UInt i=th-1; i<length; ++i) {
      w[i] = (1.0+cos(PI/per*(t[i] - 1.0 + per)))/2.0;
    }
    return w;
  }
}
  
std::vector<Real> Hann(const UInt length) noexcept {
  std::vector<Real> w = Ones(length);
  for (UInt i=0; i<length; ++i) {
    w[i] = (1.0-cos(2.0*PI*((Real)i)/((Real)(length-1))))/2.0;
  }
  return w;
}
  
Real Norm(const std::vector<Real>& vector, Real l_norm) noexcept {
  const UInt num_elements = vector.size();
  Real output = 0.0;
  for (UInt i=0; i<num_elements; ++i) {
    output += pow(fabs(vector[i]), l_norm);
  }
  return pow(output, 1.0/l_norm);
}
  
bool IsNonNegative(const std::vector<Real>& input) noexcept {
  const UInt num_elem = input.size();
  for (UInt i=0; i<num_elem; ++i) {
    if (input[i] < 0.0) { return false; }
  }
  return true;
}
  
Matrix<Real> Cov(const std::vector<Real>& x,
                 const std::vector<Real>& y) noexcept {
  std::vector<std::vector<Real> > input(2);
  input[0] = x;
  input[1] = y;
  return Cov(input);
}
  
Matrix<Real> Cov(const std::vector<std::vector<Real> >& input) noexcept {
  const UInt N = input.size();
  Matrix<Real> output(N, N);
  for (UInt i=0; i<N; ++i) {
    for (UInt j=0; j<N; ++j) {
      output.set_element(i, j, CovElement(input[i], input[j]));
    }
  }
  return output;
}
  
Real CovElement(const std::vector<Real>& x,
                const std::vector<Real>& y) noexcept {
  const UInt N = x.size();
  if (N != y.size()) { ASSERT(false); }
  
  Real output = Sum(Multiply(Add(x, -Mean(x)), Add(y, -Mean(y))));
  // In case N>1 use the unbiased estimator of covariance.
  output = (N > 1) ? output/((Real) (N-1)) : output/((Real) (N));
  return output;
}
  
std::vector<Real> CumSum(const std::vector<Real>& input) noexcept {
  const UInt N = input.size();
  std::vector<Real> output(input.size());
  
  output[N-1] = Sum(input);
  for (Int i=N-2; i>=0; --i) { output[i] = output[i+1]-input[i+1]; }
  
  return output;
}
  
std::vector<Real> Hamming(const UInt length) noexcept {
  const Real alpha = 0.54;
  const Real beta = 1.0-alpha;
  std::vector<Real> w = Ones(length);
  for (UInt i=0; i<length; ++i) {
    w[i] = alpha-beta*cos(2.0*PI*i/(length-1));
  }
  return w;
}

std::vector<std::vector<Real> > Enframe(const std::vector<Real>& input,
                                        const std::vector<Real>& window,
                                        const UInt frame_increment) noexcept {
  std::vector<std::vector<Real> > output;
  UInt i = 0;
  while ((i + window.size())<=input.size()) {
    UInt from_sample = i;
    UInt to_sample = i + window.size()-1;
    
    ASSERT(from_sample>=0 && from_sample<input.size());
    ASSERT(to_sample>=0 && to_sample<input.size());
    
    output.push_back(Multiply(Elements(input, from_sample, to_sample),
                              window));
    
    i = i + frame_increment;
  }
  return output;
}

std::vector<Real> OverlapAdd(const std::vector<std::vector<Real> >& frames,
                             const std::vector<Real>& window,
                             const UInt frame_increment) noexcept {
  const UInt num_frames = frames.size();
  std::vector<Real> output(window.size()+(num_frames-1)*frame_increment);
  for (UInt frame_i=0; frame_i<num_frames; ++frame_i) {
    if (frames[frame_i].size() != window.size()) {
      ASSERT_WITH_MESSAGE(false, "Frame length different from window length");
    }
    for (UInt k=0; k<window.size(); ++k) {
      output[frame_i*frame_increment+k] += window[k] * frames[frame_i][k];
    }
  }
  return output;
}
  
std::vector<Complex> ConvertToComplex(std::vector<Real> input) noexcept {
  std::vector<Complex> output;
  for (UInt i=0; i<input.size(); ++i) {
    output.push_back(Complex(input[i], 0.0));
  }
  return output;
}
  
} // namespace mcl
