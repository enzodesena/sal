/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "audiobuffer.hpp"
#include "vectorop.hpp"
#include "salconstants.hpp"

namespace sal
{
template<typename T>
class Decoder
{
public:
  Decoder()
  {
  }


  /**
   Decodes and puts in the output streams. It stops when the inputs stream
   is depleted.
   */
  virtual void Decode(
    const Buffer<T>& input_buffer,
    Buffer<T>& output_buffer) = 0;
};


template<typename T>
class IdenticalDecoder : public Decoder<T>
{
public:
  IdenticalDecoder()
  {
  }


  void Decode(
    const Buffer<T>& input_buffer,
    Buffer<T>& output_buffer) override
  {
    output_buffer.SetSamples(input_buffer);
  }
};
} // namespace sal
