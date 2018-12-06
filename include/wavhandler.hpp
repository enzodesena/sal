/*
 wavhandler.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_WAVHANDLER_H
#define SAL_WAVHANDLER_H

#include "saltypes.hpp"
#include "vector.hpp"
#include <iostream>
#include "salconstants.hpp"

namespace sal
{
class WavHandler
{
public:
  static mcl::Vector<Signal> Read(
    std::string file_name);

  static Time ReadSamplingFrequency(
    std::string file_name);

  static void Write(
    const mcl::Vector<Signal>& signals,
    Time sampling_frequency,
    std::string file_name);

  static void Write(
    const StereoSignal& stereo_signals,
    Time sampling_frequency,
    std::string file_name);
};
} // namespace sal

#endif
