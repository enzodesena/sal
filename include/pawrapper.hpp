/*
 pawrapper.h
 Spatial Audio Library (SAL)
 Copyright (c) 2016-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "saltypes.hpp"
#include "mcltypes.hpp"
#include "portaudio.h"
#include "decoder.hpp"
#include <strings.h>
#include "salconstants.hpp"
#include "audiobuffer.hpp"

namespace sal
{
/** A wrapper for portaudio */
class PaWrapper
{
public:
  PaWrapper(
    Time sampling_frequency,
    Int out_dev_num,
    mcl::Vector<size_t> channel_ids);

  PaError StartStream();

  template<typename T>
  PaError WriteStream(
    const Buffer<T>& output_buffer);

  PaError StopStream();


  Int max_num_channels()
  {
    return channel_ids_.size();
  }


  static void PrintError(
    PaError err);

  static void PrintDevicesInfo();
  static size_t NumOutputChannels(
    size_t out_dev_id);
  static mcl::Vector<mcl::size_t> SelectChannelIds(
    size_t num_loudspeakers,
    size_t out_dev_id);

  ~PaWrapper();
private:
  static PaError Init();
  static PaError Terminate();

  PaStream* stream_;
  mcl::Vector<size_t> channel_ids_;
};
} // namespace sal

#include "pawrapper.ipp"
