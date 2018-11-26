/*
 freefieldsimulation_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "freefieldsimulation.hpp"
#include "microphone.hpp"
#include "monomics.hpp"

using mcl::Point;

namespace sal {
  
bool FreeFieldSim::Test() {
  Time sampling_frequency = 44100;
  Length one_sample_space(SOUND_SPEED/sampling_frequency);
  
  const Int num_samples = 1;
  const Int num_output_samples = 4;
  MonoBuffer input_buffer_a(num_samples);
  input_buffer_a.SetSample(0, 0.5);
  MonoBuffer input_buffer_b(num_samples);
  input_buffer_b.SetSample(0, 0.5);
  
  mcl::Vector<MonoBuffer*> input_buffers;
  input_buffers.push_back(&input_buffer_a);
  input_buffers.push_back(&input_buffer_b);
  
  Source source_a(Point(-one_sample_space, 0.0, 0.0));
  Source source_b(Point(3*one_sample_space, 0.0, 0.0));
  mcl::Vector<Source*> sources(2);
  sources[0] = &source_a;
  sources[1] = &source_b;
  // Source/mics distribution
  // -1  0  1  2  3 Position in space
  //  x  o  o     x (x: source) (o: microphone)
  
  OmniMic mic_a(Point(0,0,0));
  MonoBuffer output_stream_a(num_output_samples);
  
  OmniMic mic_b(Point(one_sample_space,0,0));
  MonoBuffer output_stream_b(num_output_samples);
  
  mcl::Vector<Microphone*> microphones(2);
  microphones[0] = &mic_a;
  microphones[1] = &mic_b;
  
  mcl::Vector<Buffer*> output_buffers;
  output_buffers.push_back(&output_stream_a);
  output_buffers.push_back(&output_stream_b);
  
  FreeFieldSim sim(microphones, sources, sampling_frequency, SOUND_SPEED);
  sim.Run(input_buffers, num_output_samples, output_buffers);
  
  mcl::Vector<Sample> output_mic_0_cmp = mcl::Zeros<Sample>(4);
  output_mic_0_cmp[1] = 0.5;
  output_mic_0_cmp[3] = 0.5/3.0;
  mcl::Vector<Sample> output_mic_1_cmp = mcl::Zeros<Sample>(4);
  output_mic_1_cmp[2] = 0.5/2.0+0.5/2.0;
  
  ASSERT(mcl::IsEqual(output_mic_0_cmp, output_stream_a.GetReadPointer()));
  ASSERT(mcl::IsEqual(output_mic_1_cmp, output_stream_b.GetReadPointer()));
  
  return true;
}
  
} // namespace sal
