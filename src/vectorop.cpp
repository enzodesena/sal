/*
 vectorop.cpp
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
 */

#include "vectorop.h"
#include <cassert>
#include <fstream>
#include "mcltypes.h"
#include "pointwiseop.h"
#include "transformop.h"
#include <cmath>
#include "equalityop.h"
#include <vector>



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

  

Real Std(const std::vector<Real>& input) {
  Real mean = Mean(input);
  Real output(0.0);
  for (UInt i=0; i<input.size(); ++i) { output += pow(input[i] - mean,2.0); }
  return sqrt(output/((Real) (input.size()-1)));
}
  
std::vector<Real> XCorr(const std::vector<Real>& vector_a,
                        const std::vector<Real>& vector_b) {
  // TODO: implement for different sizes
  assert(vector_a.size() == vector_b.size());
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
  
void Save(const std::vector<Real>& vector, const char* file_name) {
  std::ofstream output_file;
  output_file.open(file_name);
  for (UInt i=0; i<vector.size(); ++i) {
    output_file<<vector[i]<<"\n";
  }
  output_file.close();
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
  assert(step>0);
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
  
  
} // namespace mcl
