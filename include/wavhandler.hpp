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



} // namespace sal

#include "wavhandler.ipp"
