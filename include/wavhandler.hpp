/*
 wavhandler.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "saltypes.hpp"
#include "vector.hpp"
#include <iostream>
#include "salconstants.hpp"
#include "sndfile.hh"

namespace sal
{
template<typename T>
class WavHandler
{
public:
  static mcl::Vector<Signal<T>> Read(
    std::string file_name);

  static Time ReadSamplingFrequency(
    std::string file_name);

  static void Write(
    const mcl::Vector<Signal<T>>& signals,
    Time sampling_frequency,
    std::string file_name);

  static void Write(
    const StereoSignal<T>& stereo_signals,
    Time sampling_frequency,
    std::string file_name);
};


template<typename T>
mcl::Vector<Signal<T>> WavHandler<T>::Read(
  const std::string file_name)
{
  SNDFILE* input_file;
  SF_INFO input_file_info;

  if (! (input_file = sf_open(file_name.c_str(), SFM_READ, &input_file_info)))
  {
    mcl::Logger::GetInstance().LogErrorToCerr("Error : could not open file.");
    assert(false);
  }

  if (! sf_format_check(&input_file_info))
  {
    sf_close(input_file);
    mcl::Logger::GetInstance().LogErrorToCerr("Error : could not open file.");
    assert(false);
  }

  ASSERT(input_file_info.frames > 0);

  int file_length = (int)input_file_info.frames;
  int num_channels = input_file_info.channels;

  double* data = new double[num_channels * file_length];

  sf_readf_double(input_file, data, file_length);

  sf_close(input_file);

  mcl::Vector<Signal<T>> signals
  (
    num_channels,
    mcl::Zeros<Sample>(file_length));

  // input_file_info.samplerate

  int k(0);
  for (int i = 0; i < (num_channels * file_length); i += num_channels)
  {
    for (int j = 0; j < num_channels; ++j)
    {
      signals[j][k] = (Sample)data[i + j];
    }
    k++;
  }

  delete [] data;

  return signals;
}


template<typename T>
Time WavHandler<T>::ReadSamplingFrequency(
  const std::string file_name)
{
  SNDFILE* input_file;
  SF_INFO input_file_info;

  if (! (input_file = sf_open(file_name.c_str(), SFM_READ, &input_file_info)))
  {
    std::cout << "Error : could not open file : " << file_name << "\n";
    exit(1);
  }

  if (! sf_format_check(&input_file_info))
  {
    sf_close(input_file);
    std::cout << "Invalid encoding\n";
    exit(1);
  }

  return input_file_info.samplerate;
}


template<typename T>
void WavHandler<T>::Write(
  const StereoSignal<T>& stereo_signals,
  const Time sampling_frequency,
  std::string file_name)
{
  mcl::Vector<Signal<T>> signals(2);
  signals[0] = stereo_signals.left;
  signals[1] = stereo_signals.right;
  Write(signals, sampling_frequency, file_name);
}


template<typename T>
void WavHandler<T>::Write(
  const mcl::Vector<Signal<T>>& signals,
  const Time sampling_frequency,
  std::string file_name)
{
  // TODO: add support for more than 2 channels (wave_format_extensible)
  ASSERT(signals.size() <= 2);

  Int num_channels = signals.size();
  Int file_length = signals[0].size();

  SNDFILE* output_file;
  SF_INFO output_file_info;
  output_file_info.frames = (int)file_length;
  output_file_info.channels = (int)num_channels;
  output_file_info.samplerate = (int)sampling_frequency;
  output_file_info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

  ASSERT(sf_format_check(&output_file_info));

  if (! (output_file = sf_open(
    file_name.c_str(), SFM_WRITE, &output_file_info)))
  {
    std::cout << "Error: could not open file : " << file_name << "\n";
    ASSERT(false);
  }

  double* samples = new double[num_channels * file_length];
  Int k = 0;
  for (Int i = 0; i < (num_channels * file_length); i += num_channels)
  {
    for (Int j = 0; j < num_channels; ++j)
    {
      if (std::fabs(signals[j][k]) > 1.0)
      {
        std::cout << "Warning: wav writer clipped (value: " <<
          std::fabs(signals[j][k]) << ")" << std::endl;
      }
      samples[i + j] = (double)signals[j][k];
    }
    k++;
  }

  sf_write_double
  (
    output_file,
    samples,
    ((int)num_channels) * ((int)file_length));

  sf_close(output_file);
  delete [] samples;
}

} // namespace sal

