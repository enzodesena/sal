/*
 wavhandler.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <assert.h>

#include "AudioFile.h"
#include "vectorop.h"
#include "wavhandler.h"

namespace sal {

std::vector<Signal> WavHandler::Read(const std::string file_name) {
  AudioFile<double> audioFile;
  audioFile.load(file_name);

  int num_channels = audioFile.getNumChannels();
  int num_samples = audioFile.getNumSamplesPerChannel();

  std::vector<Signal> signals(num_channels, dsp::Zeros<Sample>(num_samples));

  for (int i = 0; i < num_samples; i += num_channels) {
    for (int channel = 0; channel < num_channels; ++channel) {
      signals[channel][i] = (Sample)audioFile.samples[channel][i];
    }
  }

  return signals;
}

Time WavHandler::ReadSamplingFrequency(const std::string file_name) {
  AudioFile<double> audioFile;
  audioFile.load(file_name);
  return (Time)audioFile.getSampleRate();
}

void WavHandler::Write(const StereoSignal& stereo_signals,
                       const std::string& file_name,
                       const Time sampling_frequency,
                       const int num_bits_per_sample) {
  std::vector<Signal> signals(2);
  signals[0] = stereo_signals.left;
  signals[1] = stereo_signals.right;
  Write(signals, file_name, sampling_frequency, num_bits_per_sample);
}



void WavHandler::Write(const std::vector<Signal>& signals,
                       const std::string& file_name,
                       const Time sampling_frequency,
                       const int num_bits_per_sample) {
  AudioFile<double> audioFile;
  int num_channels = static_cast<int>(signals.size());
  ASSERT(num_channels >= 1);
  int num_samples = static_cast<int>(signals[0].size());
  audioFile.setAudioBufferSize(num_channels, (int)num_samples);
  audioFile.setBitDepth(num_bits_per_sample);
  audioFile.setSampleRate((uint32_t)sampling_frequency);

  for (int channel = 0; channel < num_channels; ++channel) {
    for (int i = 0; i < num_samples; ++i) {
      audioFile.samples[channel][i] = signals[channel][i];
    }
  }

  audioFile.save(file_name, AudioFileFormat::Wave);
}


void WavHandler::Write(const Buffer& buffer,
                       const std::string& file_name,
                       const Time sampling_frequency,
                       const int num_bits_per_sample) {
  std::vector<Signal> signals(buffer.num_channels());
  for (int i=0; i<buffer.num_channels(); ++i) {
    signals[i].resize(buffer.num_samples());
    std::copy(buffer.GetReadView(i).begin(),
              buffer.GetReadView(i).begin()+buffer.num_samples(),
              signals[i].begin());
  }
  
  Write(signals, file_name, sampling_frequency, num_bits_per_sample);
}

}  // namespace sal
