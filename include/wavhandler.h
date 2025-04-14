/*
 wavhandler.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_WAVHANDLER_H
#define SAL_WAVHANDLER_H

#include <iostream>
#include <vector>

#include "salconstants.h"
#include "audiobuffer.h"
#include "saltypes.h"

namespace sal {

class WavHandler {
 public:
  static std::vector<Signal> Read(const std::string file_name);

  static Time ReadSamplingFrequency(const std::string file_name);

  static void Write(const std::vector<Signal>& signals,
                    const std::string& file_name,
                    const Time sampling_frequency,
                    const int num_bits_per_sample = 16);

  static void Write(const StereoSignal& stereo_signals,
                    const std::string& file_name,
                    const Time sampling_frequency,
                    const int num_bits_per_sample = 16);
  
  static void Write(const Buffer& signals,
                    const std::string& file_name,
                    const Time sampling_frequency,
                    const int num_bits_per_sample = 16);
};

}  // namespace sal

#endif  // SAL_WAVHANDLER_H
