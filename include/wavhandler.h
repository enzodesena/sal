/*
 wavhandler.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAT_WAVHANDLER_H
#define SAT_WAVHANDLER_H


#include "sattypes.h"
#include <vector>
#include <iostream>
#include "sndfile.hh"
#include "mcl.h"

namespace sat {

class WavHandler {
public:
  static std::vector<Signal> Read(const std::string file_name);
  
  static Time ReadSamplingFrequency(const std::string file_name);


static void Write(const std::vector<Signal>& signals,
                  const Time sampling_frequency,
                  const std::string file_name);
  
static void Write(const StereoSignal& stereo_signals,
                  const Time sampling_frequency,
                  std::string file_name);
};
  
} // namespace sat

#endif
