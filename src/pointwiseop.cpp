/*
 pointwise.cpp
 MCL
 
 This contains vector point-wise operations
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "pointwiseop.h"
#include "mcltypes.h"
#include "vectorop.h"
#include <vector>


namespace mcl {

std::vector<Complex> ComplexVector(const std::vector<Real>& input) {
  std::vector<Complex> output(input.size());
  for (UInt i=0; i<input.size(); ++i) { output[i] = Complex(input[i], 0.0); }
  return output;
}

std::vector<Real> RealPart(const std::vector<Complex>& input) {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) { output[i] = input[i].real(); }
  return output;
}

std::vector<Real> Imag(const std::vector<Complex>& input) {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) { output[i] = input[i].imag(); }
  return output;
}


std::vector<Real> Inverse(const std::vector<Real>& vector) {
  std::vector<Real> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) { output[i] = 1.0/vector[i]; }
  return output;
}


std::vector<Complex> Conj(const std::vector<Complex>& vector) {
  std::vector<Complex> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) { 
    output[i] = Conj(vector[i]); 
  }
  return output;
}



std::vector<Real> Pow(const std::vector<Real>& vector, Real exponent) {
  std::vector<Real> output(vector.size());
  for (UInt i=0; i<vector.size(); ++i) { 
    output[i] = Pow(vector[i], exponent);
  }
  return output;
}


std::vector<Real> HalfWave(const std::vector<Real>& input) {
  std::vector<Real> output = Zeros<Real>(input.size());
  for (UInt i=0; i<input.size(); ++i) {
    if (input[i] > 0.0)
      output[i] = input[i];
  }
  return output;
}


std::vector<Real> Abs(const std::vector<Real>& input) {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) { 
    output[i] = fabs(input[i]);
  }
  return output;
}


std::vector<Real> Abs(const std::vector<Complex>& input) {
  std::vector<Real> output(input.size());
  for (UInt i=0; i<input.size(); ++i) { 
    output[i] = abs(input[i]);
  }
  return output;
}


std::vector<Real> Log(const std::vector<Real>& vector) {
  UInt n(vector.size());
  std::vector<Real> output(vector.size());
  for (UInt i=0; i<n; ++i) { output[i] = log(vector[i]); }
  return output;
}

std::vector<Real> Cos(const std::vector<Real>& vector) {
  UInt n(vector.size());
  std::vector<Real> output(vector.size());
  for (UInt i=0; i<n; ++i) { output[i] = cos(vector[i]); }
  return output;
}
  
std::vector<Real> Sin(const std::vector<Real>& vector) {
  UInt n(vector.size());
  std::vector<Real> output(vector.size());
  for (UInt i=0; i<n; ++i) { output[i] = sin(vector[i]); }
  return output;
}

std::vector<Int> ConvertToInt(const std::vector<UInt>& vector) {
  const UInt length = vector.size();
  std::vector<Int> output(length);
  for (UInt i=0; i<length; ++i) { output[i] = (Int) vector[i]; }
  return output;
}
  
} // namespace mcl

