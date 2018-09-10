/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "pointwiseop.h"
#include "mcltypes.h"
#include "vectorop.h"
#include <vector>

#ifdef ENVAPPLE
#include <Accelerate/Accelerate.h>
#endif

namespace mcl {

  
void Multiply(const Real* input_data_a,
              const Real* input_data_b,
              Int num_samples,
              Real* output_data) noexcept {
#ifdef ENVAPPLE
  #ifdef MCL_DATA_TYPE_DOUBLE
  vDSP_vmulD(input_data_a, 1,
             input_data_b, 1,
             output_data, 1,
             num_samples);
  #else
  vDSP_vmul(input_data_a, 1,
            input_data_b, 1,
            output_data, 1,
            num_samples);
  #endif
#else
  for (Int i=0; i<num_samples; ++i) {
    output_data[i] = input_data_a[i]*input_data_b[i];
  }
#endif
}
  
void Add(const Real* input_data_a,
                 const Real* input_data_b,
                 Int num_samples,
                 Real* output_data) noexcept {
#ifdef ENVAPPLE
  #ifdef MCL_DATA_TYPE_DOUBLE
    vDSP_vaddD(input_data_a, 1,
               input_data_b, 1,
               output_data, 1,
               num_samples);
  #else
    vDSP_vadd(input_data_a, 1,
              input_data_b, 1,
              output_data, 1,
              num_samples);
  #endif
#else
  for (Int i=0; i<num_samples; ++i) {
    output_data[i] = input_data_a[i]+input_data_b[i];
  }
#endif
}
  
std::vector<Complex> ComplexVector(const std::vector<Real>& input) noexcept {
  std::vector<Complex> output(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) { output[i] = Complex(input[i], 0.0); }
  return output;
}

std::vector<Real> RealPart(const std::vector<Complex>& input) noexcept {
  std::vector<Real> output(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) { output[i] = input[i].real(); }
  return output;
}

std::vector<Real> Imag(const std::vector<Complex>& input) noexcept {
  std::vector<Real> output(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) { output[i] = input[i].imag(); }
  return output;
}


std::vector<Real> Inverse(const std::vector<Real>& vector) noexcept {
  std::vector<Real> output(vector.size());
  for (Int i=0; i<(Int)vector.size(); ++i) { output[i] = 1.0/vector[i]; }
  return output;
}


std::vector<Complex> Conj(const std::vector<Complex>& vector) noexcept {
  std::vector<Complex> output(vector.size());
  for (Int i=0; i<(Int)vector.size(); ++i) { 
    output[i] = Conj(vector[i]); 
  }
  return output;
}



std::vector<Real> Pow(const std::vector<Real>& vector, Real exponent) noexcept {
  std::vector<Real> output(vector.size());
  for (Int i=0; i<(Int)vector.size(); ++i) { 
    output[i] = Pow(vector[i], exponent);
  }
  return output;
}


std::vector<Real> HalfWave(const std::vector<Real>& input) noexcept {
  std::vector<Real> output = Zeros<Real>(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) {
    if (input[i] > 0.0)
      output[i] = input[i];
  }
  return output;
}


std::vector<Real> Abs(const std::vector<Real>& input) noexcept {
  std::vector<Real> output(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) { 
    output[i] = std::fabs(input[i]);
  }
  return output;
}


std::vector<Real> Abs(const std::vector<Complex>& input) noexcept {
  std::vector<Real> output(input.size());
  for (Int i=0; i<(Int)input.size(); ++i) { 
    output[i] = std::abs(input[i]);
  }
  return output;
}


std::vector<Real> Log(const std::vector<Real>& vector) noexcept {
  Int n(vector.size());
  std::vector<Real> output(vector.size());
  for (Int i=0; i<n; ++i) { output[i] = log(vector[i]); }
  return output;
}
  
std::vector<Real> Log10(const std::vector<Real>& vector) noexcept {
  Int n(vector.size());
  std::vector<Real> output(vector.size());
  for (Int i=0; i<n; ++i) { output[i] = log10(vector[i]); }
  return output;
}

std::vector<Real> Cos(const std::vector<Real>& vector) noexcept {
  Int n(vector.size());
  std::vector<Real> output(vector.size());
  for (Int i=0; i<n; ++i) { output[i] = cos(vector[i]); }
  return output;
}
  
std::vector<Real> Sin(const std::vector<Real>& vector) noexcept {
  Int n(vector.size());
  std::vector<Real> output(vector.size());
  for (Int i=0; i<n; ++i) { output[i] = sin(vector[i]); }
  return output;
}

std::vector<Int> ConvertToInt(const std::vector<UInt>& vector) noexcept {
  const Int length = vector.size();
  std::vector<Int> output(length);
  for (Int i=0; i<length; ++i) { output[i] = (Int) vector[i]; }
  return output;
}
  
} // namespace mcl

