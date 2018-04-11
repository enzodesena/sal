/*
 sdnc_sound_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "pawrapper.h"
#include "matrixop.h"
#include "propagationline.h"
#include <unistd.h>

using namespace sal;

Signal GenerateSine(const Time length, const Time sine_frequency,
                    const Time sampling_frequency,
                    const Sample amplitude, const Time trailing_length);

int main(int argc, char * const argv[]) {
  
  const Time sampling_frequency = 44100;
  const UInt samples_per_buffer = 1000;
  
  const Time sine_frequency = 1000.0;
  const Sample sine_amplitude = 0.1;
  
  const Time sine_length = 2.0;
  
  const Length speed = 171.5 ; //[m/s];
  
  
  Signal sine = GenerateSine(sine_length, sine_frequency, sampling_frequency, sine_amplitude, 0);
  mcl::Matrix<sal::Sample> original_sine(sine.size(), 1);
  mcl::Matrix<sal::Sample> doppler_sine(sine.size(), 1);
  mcl::Matrix<sal::Time> current_latency(sine.size(), 1);
  
  Length current_distance = SOUND_SPEED/sampling_frequency;
  PropagationLine line_doppler(current_distance, sampling_frequency, 1000.0, true);
  
  PropagationLine line(current_distance, sampling_frequency, 1000.0, true);
  line.set_gain(1.0, 0.0);
  
  UInt index = 0;
  while (index<ceil(sine_length*sampling_frequency)) {
    Time update_period = (Time) samples_per_buffer / sampling_frequency; // In seconds
    Length space_covered = update_period * speed;
    current_distance += space_covered;
    line_doppler.set_distance(current_distance, update_period);
    
    line_doppler.set_gain(1.0, 0.0); // Sets gain to 1 instantly, to bypass the slow update of set_distance
    
    for (UInt i=0; i<samples_per_buffer; ++i) {
      if (index>=ceil(sine_length*sampling_frequency)) { continue; }
      
      line_doppler.Tick();
      line_doppler.Write(sine.at(index));
      
      line.Tick();
      line.Write(sine.at(index));
      
      doppler_sine.set_element(index, 0, line_doppler.Read());
      original_sine.set_element(index, 0, line.Read());
      
      current_latency.set_element(index, 0, line_doppler.current_latency());
      
      index++;
    }
  }
  
  original_sine.Save("/Users/enzodesena/repos/sal/src/bin/sine_original.txt", 5);
  doppler_sine.Save("/Users/enzodesena/repos/sal/src/bin/sine_doppler.txt", 5);
  current_latency.Save("/Users/enzodesena/repos/sal/src/bin/current_latency.txt", 10);
  
  return 0;
}

Signal GenerateSine(const Time length, const Time sine_frequency,
                    const Time sampling_frequency,
                    const Sample amplitude, const Time trailing_length) {
  Signal t = mcl::ColonOperator(0, 1.0/sampling_frequency, length);
  Signal sine_wave = mcl::Sin(mcl::Multiply(t, 2.0*PI*sine_frequency));
  sine_wave = mcl::Multiply(sine_wave, amplitude);
  sine_wave = mcl::Concatenate(sine_wave,
                               mcl::Zeros<mcl::Real>(trailing_length*sampling_frequency));
  return sine_wave;
}

