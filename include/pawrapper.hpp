/*
 pawrapper.h
 Spatial Audio Library (SAL)
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_pawrapper_h
#define SAL_pawrapper_h

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
    mcl::Vector<Int> channel_ids);

  PaError StartStream();

  PaError WriteStream(
    const Buffer& output_buffer);

  PaError StopStream();


  Int max_num_channels()
  {
    return channel_ids_.size();
  }


  static void PrintError(
    PaError err);

  static void PrintDevicesInfo();
  static Int NumOutputChannels(
    Int out_dev_id);
  static mcl::Vector<mcl::Int> SelectChannelIds(
    Int num_loudspeakers,
    Int out_dev_id);

  ~PaWrapper();
private:
  static PaError Init();
  static PaError Terminate();

  PaStream* stream_;
  mcl::Vector<Int> channel_ids_;
};
} // namespace sal

#endif
