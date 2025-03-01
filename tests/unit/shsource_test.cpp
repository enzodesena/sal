/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2025, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "shsource.h"
#include "monomics.h"
#include "salconstants.h"
#include "audiobuffer.h"
#include "wavhandler.h"
#include "vectorop.h"

using sal::dsp::Point;
using sal::dsp::Quaternion;

namespace sal {

bool ShSource::Test() {
  dsp::Real sample_rate = 44100.0;
  // These tests were calculated by Josh using Matlab code (I don't have the code)
  ShSource source_a(dsp::Point(0,0,0), dsp::AxAng2Quat(0, 0, 1, PI/2.0), GenelecDirectivityFreqs, GenelecDirectivityShCoeffs, sample_rate);
  
  std::vector<dsp::Real> freqs = {62.5,  125,  250,  500,  1000,  2000,  4000,  8000,  16000};
//  std::vector<dsp::Real> output_a = source_a.GetTransferFunction(freqs, 0.698131701,  0.174532925);
//  
//  ASSERT(dsp::IsEqual(source_a.GetTransferFunction(freqs, 0.698131701,  0.174532925), {0.780453669, 0.990210368, 0.950848385, 0.934653771, 0.81233706, 0.755566864, 0.59692986, 0.663974511, 0.642383082}));
//  
//  std::vector<dsp::Real> output_b = source_a.GetTransferFunction(freqs, 1.919862177, 3.316125579);
//  ASSERT(dsp::IsEqual(source_a.GetTransferFunction(freqs, 1.919862177, 3.316125579), {0.780453669, 0.990210368, 0.950848385, 0.675747598, 0.410316926, 0.26461174, 0.206289169, 0.120467801, 0.010705615}));
//  
  
  dsp::Real distance = 1.0;
  std::vector<dsp::Real> thetas = dsp::LinSpace(0, 180, 19);
  std::vector<dsp::Real> phis =  dsp::LinSpace(0, 360, 37);
  
  size_t num_directions = thetas.size() * phis.size();
  
  size_t num_samples = 500;
  Buffer responses(num_directions, num_samples);
  dsp::Matrix<dsp::Real> coefficients(freqs.size(), num_directions);
  std::vector<dsp::Real> impulse_signal = dsp::Zeros<dsp::Real>(num_samples);
  impulse_signal[0] = 0.5;
  std::vector<dsp::Real> output_signal(num_samples);
  
  dsp::Matrix<dsp::Real> directions(num_directions, 5);
  dsp::Int k=0;
  for (dsp::Real theta : thetas) {
    for (dsp::Real phi : phis) {
      directions.SetElement(k, 0, (int) theta);
      directions.SetElement(k, 1, (int) phi);
      dsp::Real x = distance*std::sin(theta/180.0*M_PI)*std::cos(phi/180.0*M_PI);
      dsp::Real y = distance*std::sin(theta/180.0*M_PI)*std::sin(phi/180.0*M_PI);
      dsp::Real z = distance*std::cos(theta/180.0*M_PI);
      
      directions.SetElement(k, 0, (int) theta);
      directions.SetElement(k, 1, (int) phi);
      directions.SetElement(k, 2, x);
      directions.SetElement(k, 3, y);
      directions.SetElement(k, 4, z);
      
      Point observer_point(x, y, z);
      ShSource source_b(dsp::Point(0,0,0), dsp::AxAng2Quat(0, 0, 1, PI/2.0), GenelecDirectivityFreqs, GenelecDirectivityShCoeffs, sample_rate);
      
      source_b.ProcessAudio(impulse_signal, observer_point, 0, responses.GetWriteView(k));
      source_b.ResetFiltersState();
      coefficients.SetColumn(k, source_b.GetTransferFunction(freqs, observer_point));
      
      k++;
    }
  }
  
  WavHandler::Write(responses, "tests/unit/outputs/shsource_genelec_directivity_irs.wav", sample_rate, 24);
  coefficients.Save("tests/unit/outputs/shsource_genelec_directivity_coeffs.txt", 5);
  directions.Save("tests/unit/outputs/shsource_genelec_directivity_directions.txt", 5);
  
  return true;
}

}
