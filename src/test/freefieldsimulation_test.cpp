/*
 freefieldsimulation_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "freefieldsimulation.h"
#include "microphone.h"
#include "monomics.h"

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
  
  std::vector<std::unique_ptr<MonoBuffer> > input_buffers;
  input_buffers.emplace_back(std::make_unique<MonoBuffer>(input_buffer_a));
  input_buffers.emplace_back(std::make_unique<MonoBuffer>(input_buffer_b));
  
  Source source_a(Point(-one_sample_space, 0.0, 0.0));
  Source source_b(Point(3*one_sample_space, 0.0, 0.0));
  std::vector<Source*> sources(2);
  sources[0] = &source_a;
  sources[1] = &source_b;
  // Source/mics distribution
  // -1  0  1  2  3 Position in space
  //  x  o  o     x (x: source) (o: microphone)
  
  OmniMic mic_a(Point(0,0,0));
  MonoBuffer output_stream_a(num_output_samples);
  
  OmniMic mic_b(Point(one_sample_space,0,0));
  MonoBuffer output_stream_b(num_output_samples);
  
  std::vector<Microphone*> microphones(2);
  microphones[0] = &mic_a;
  microphones[1] = &mic_b;
  
  std::vector<std::unique_ptr<Buffer> > output_buffers;
  output_buffers.emplace_back(std::make_unique<Buffer>(output_stream_a));
  output_buffers.emplace_back(std::make_unique<Buffer>(output_stream_b));
  
  FreeFieldSim sim(microphones, sources, sampling_frequency, SOUND_SPEED);
  sim.ProcessBlock(input_buffers, output_buffers);
  
  std::vector<Sample> output_mic_0_cmp = mcl::Zeros<Sample>(4);
  output_mic_0_cmp[1] = 0.5;
  output_mic_0_cmp[3] = 0.5/3.0;
  std::vector<Sample> output_mic_1_cmp = mcl::Zeros<Sample>(4);
  output_mic_1_cmp[2] = 0.5/2.0+0.5/2.0;
  
  ASSERT(mcl::IsEqual(output_mic_0_cmp, output_buffers[0]->GetReadView(Buffer::kMonoChannel)));
  ASSERT(mcl::IsEqual(output_mic_1_cmp, output_buffers[1]->GetReadView(Buffer::kMonoChannel)));
  
  return true;
}
  
} // namespace sal
