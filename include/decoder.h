/*
 decoder
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_decoder_h
#define SAL_decoder_h

#include "stream.h"
#include "vectorop.h"
#include "salconstants.h"

namespace sal {

class SAL_API Decoder {
public:
  Decoder(UInt num_loudspeakers) :
          output_streams_(std::vector<MonoStream>(num_loudspeakers)) {}
  
  /**
   Decodes and puts in the output streams. It stops when the inputs stream
   is depleted.
   */
  virtual void Decode() = 0;
  
  MonoStream* stream(Int chan_id) { return &(output_streams_.at(chan_id)); }
  
  UInt num_loudspeakers() { return output_streams_.size(); }
  
  UInt size() {
    std::vector<Int> sizes(num_loudspeakers());
    for (UInt i=0; i<num_loudspeakers(); i++) { sizes[i] = stream(i)->size(); }
    return mcl::Max(sizes);
  }
  
  bool IsEmpty() {
    return size() == 0;
  }
  
  virtual ~Decoder() {}
  
protected:
  std::vector<MonoStream> output_streams_;
};
  
  
class SAL_API IdenticalDecoder : public Decoder {
public:
  IdenticalDecoder(MonoStream* stream) : Decoder(1),
          input_streams_(mcl::UnaryVector(stream)) {}
  
  IdenticalDecoder(std::vector<MonoStream*> streams) : Decoder(streams.size()),
          input_streams_(streams) {}
  
  virtual void Decode() {
    const UInt num_streams = output_streams_.size();
    assert(num_streams == input_streams_.size());
    
    for (UInt i=0; i<num_streams; ++i) {
      output_streams_[i].Push(input_streams_[i]->PullAll());
    }
  }
  
  virtual ~IdenticalDecoder() {}
  
private:
  std::vector<MonoStream*> input_streams_;
};

} // namespace sal

#endif
