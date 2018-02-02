/*
 wavhandler.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "wavhandler.h"
#include "sndfile.hh"
#include "vectorop.h"
#include "exception.h"
#include <assert.h>

namespace sal {

std::vector<Signal> WavHandler::Read(const std::string file_name) {
  SNDFILE* input_file;
  SF_INFO input_file_info;
  
  if (! (input_file = sf_open(file_name.c_str(), SFM_READ, &input_file_info))) {
    throw(mcl::Exception("Error : could not open file."));
  }
  
  if (! sf_format_check (&input_file_info)) {
    sf_close (input_file);
    throw(mcl::Exception("Error : could not open file."));
  }
  
  assert(input_file_info.frames > 0);
  
  int file_length = (int) input_file_info.frames;
  int num_channels = input_file_info.channels;
  
  double* data = new double[num_channels*file_length];
  
  sf_readf_double(input_file, data, file_length);
  
  sf_close(input_file);
  
  std::vector<Signal> signals(num_channels,
                              mcl::Zeros<Sample>(file_length));
  
  // input_file_info.samplerate
  
  int k(0);
  for (int i=0; i<(num_channels*file_length); i += num_channels) {
    for (int j=0; j<num_channels; ++j) {
      signals[j][k] = (Sample) data[i+j];
    }
    k++;
  }
  
  delete [] data;
  
  return signals;
}
  
Time WavHandler::ReadSamplingFrequency(const std::string file_name) {
  SNDFILE* input_file;
  SF_INFO input_file_info;
  
  if (! (input_file = sf_open (file_name.c_str(), SFM_READ, &input_file_info))) {
    std::cout<<"Error : could not open file : "<<file_name<<"\n";
    exit (1);
  }
  
  if (! sf_format_check (&input_file_info)) {
    sf_close (input_file);
    std::cout<<"Invalid encoding\n";
    exit(1);
  }
  
  return input_file_info.samplerate;
}

void WavHandler::Write(const StereoSignal& stereo_signals,
                       const Time sampling_frequency,
                       std::string file_name) {
  std::vector<Signal> signals(2);
  signals[0] = stereo_signals.left;
  signals[1] = stereo_signals.right;
  Write(signals, sampling_frequency, file_name);
}
  
void WavHandler::Write(const std::vector<Signal>& signals,
                  const Time sampling_frequency,
                  std::string file_name) {
  // TODO: add support for more than 2 channels (wave_format_extensible)
  assert(signals.size() <= 2);
  
  UInt num_channels = signals.size();
  UInt file_length = signals[0].size();
  
  SNDFILE* output_file;
  SF_INFO output_file_info;
  output_file_info.frames = (int) file_length;
  output_file_info.channels = (int) num_channels;
  output_file_info.samplerate = sampling_frequency;
  output_file_info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
  
  assert(sf_format_check (&output_file_info));
  
  if (! (output_file = sf_open (file_name.c_str(), SFM_WRITE, &output_file_info))) {
    std::cout<<"Error: could not open file : "<<file_name<<"\n";
    exit (1);
  }
  
  double* samples = new double[num_channels*file_length];
  UInt k = 0;
  for (UInt i=0; i<(num_channels*file_length); i += num_channels) {
    for (UInt j=0; j<num_channels; ++j) {
      if (fabs(signals[j][k]) > 1.0) {
        std::cout<<"Warning: wav writer clipped (value: "<<
                fabs(signals[j][k])<<")"<<std::endl;
      }
      samples[i+j] = (double) signals[j][k];
    }
    k++;
  }
  
  sf_write_double(output_file, samples,
                  ((int) num_channels)*((int) file_length));
  
  sf_close(output_file);
  delete [] samples;
}
  
} // namespace sal
