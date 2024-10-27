/*
 pawrapper.h
 Spatial Audio Library (SAL)
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_pawrapper_h
#define SAL_pawrapper_h

#include <strings.h>

#include "audiobuffer.h"
#include "decoder.h"
#include "dsptypes.h"
#include "portaudio.h"
#include "salconstants.h"
#include "saltypes.h"

namespace sal {

/** A wrapper for portaudio */
class PaWrapper {
 public:
  PaWrapper(Time sampling_frequency, Int out_dev_num,
            std::vector<Int> channel_ids);

  PaError StartStream();

  PaError WriteStream(const Buffer& output_buffer);

  PaError StopStream();

  Int max_num_channels() { return channel_ids_.size(); }

  static void PrintError(PaError err);

  static void PrintDevicesInfo();
  static Int NumOutputChannels(const Int out_dev_id);
  static std::vector<dsp::Int> SelectChannelIds(const Int num_loudspeakers,
                                                const Int out_dev_id);

  ~PaWrapper();

 private:
  static PaError Init();
  static PaError Terminate();

  PaStream* stream_;
  std::vector<Int> channel_ids_;
};

}  // namespace sal

#endif
