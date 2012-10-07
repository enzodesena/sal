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
#include <vector>



namespace mcl {
  
  
std::vector<Real> LinSpace(Real min, Real max, UInt num_elements) {
  assert(num_elements > 0);
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

  
} // namespace mcl
