/*
 decoder
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_decoder_h
#define SAL_decoder_h

#include "audiobuffer.hpp"
#include "vectorop.hpp"
#include "salconstants.hpp"

namespace sal {

class Decoder {
public:
  Decoder() {}
  
  /**
   Decodes and puts in the output streams. It stops when the inputs stream
   is depleted.
   */
  virtual void Decode(const Buffer& input_buffer,
                      Buffer& output_buffer) = 0;
  
  
  virtual ~Decoder() {}
};
  
  
class IdenticalDecoder : public Decoder {
public:
  IdenticalDecoder() {}
  
  virtual void Decode(const Buffer& input_buffer,
                      Buffer& output_buffer) {
    output_buffer.SetSamples(input_buffer);
  }
  
  virtual ~IdenticalDecoder() {}
};

} // namespace sal

#endif
