/*
 pawrapper.h
 Spatial Audio Library (SAL)
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_pawrapper_h
#define SAL_pawrapper_h

#include "saltypes.h"
#include "mcltypes.h"
#include "portaudio.h"
#include "decoder.h"

namespace sal {
  
/** A wrapper for portaudio */
class PaWrapper {
public:

  PaWrapper(Decoder& decoder, Time sampling_frequency, UInt frames_per_buffer,
            Int out_dev_num, std::vector<Int> channel_ids);
  
  void StartStream();
  void WriteStream();
  void StopStream();
  
  ~PaWrapper();
private:
  void PaErrorHandler(PaError err);
  
  PaStream* stream_;
  Decoder* decoder_;
  std::vector<Int> channel_ids_;
  UInt frames_per_buffer_;
};
  
  
} // namespace sal

#endif
