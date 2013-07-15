/*
 wavhandler.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_WAVHANDLER_H
#define SAL_WAVHANDLER_H


#include "saltypes.h"
#include <vector>
#include <iostream>
#include "sndfile.hh"
#include "mcl.h"

namespace sal {

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
  
} // namespace sal

#endif
