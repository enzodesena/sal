/*
 sdnc_sound_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


#include "pawrapper.h"
#include <unistd.h>

using namespace sal;

Signal GenerateSine(const Time length, const Time sine_frequency,
                    const Time sampling_frequency,
                    const Sample amplitude, const Time trailing_length);

int main(int argc, char * const argv[]) {
  
  const Time sampling_frequency = 44100;
  const Int samples_per_buffer = 1000;
  
  const Time pulse_frequency = 1000.0;
  const Sample pulse_amplitude = 0.1;
  
  const Time pulse_length = 0.5;
  const Time trailing_length = 0.5;
  const Time long_sine_length = 0.0;
  
  PaWrapper::PrintDevicesInfo();
  
  std::cout<<"Choose devide id: ";
  Int dev_id;
  std::cin>>dev_id;
  
  const Int max_num_channels = PaWrapper::NumOutputChannels(dev_id);
  std::vector<Int> channel_ids =
  mcl::ColonOperator<mcl::Int>(0, max_num_channels-1);
  
  // Set streams
  Signal t = mcl::ColonOperator(0, 1.0/sampling_frequency, pulse_length);
  Signal sine_wave = GenerateSine(pulse_length, pulse_frequency,
                                  sampling_frequency,
                                  pulse_amplitude, trailing_length);
  Signal long_sine_wave = GenerateSine(long_sine_length, pulse_frequency,
                                       sampling_frequency,
                                       pulse_amplitude, trailing_length);
  
  MultichannelBuffer buffer(max_num_channels, samples_per_buffer);
  
  PaWrapper pa(sampling_frequency,
               dev_id, channel_ids);
  
  pa.StartStream();
  
  for (Int i=0; i<max_num_channels; ++i) {
    Signal signal;
    for (Int j=1; j<=i+1; ++j) { signal = mcl::Concatenate(signal, sine_wave); }
    
    signal = mcl::Concatenate(signal, mcl::Zeros<mcl::Real>(sine_wave.size()));
    signal = mcl::Concatenate(signal, long_sine_wave);
    signal = mcl::Concatenate(signal, mcl::Zeros<mcl::Real>(sine_wave.size()));
    
    Int segment_id = 0;
    while (segment_id*samples_per_buffer < signal.size()) {
      buffer.SetSamples(i, 0, samples_per_buffer, mcl::GetSegment(signal, segment_id++, samples_per_buffer, true).data());
    
      PaError error = pa.WriteStream(buffer);
      if (error) {
        PaWrapper::PrintError(error);
        exit(1);
      }
      
      buffer.Reset();
    }
  }
  
  pa.StopStream();
  
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

