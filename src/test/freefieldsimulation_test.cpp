/*
 freefieldsimulation_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "freefieldsimulation.h"
#include "microphone.h"
#include "monomics.h"

using mcl::Point;

namespace sal {
  
bool FreeFieldSim::Test() {
  Time sampling_frequency = 44100;
  Length one_sample_space(SOUND_SPEED/sampling_frequency);
  
  std::vector<Sample> signal = mcl::Zeros<Sample>(1);
  signal[0] = 0.5;
  
  
  OmniMic mic_a(Point(0,0,0));
  MonoStream* output_stream_a = mic_a.stream();
  
  OmniMic mic_b(Point(one_sample_space,0,0));
  MonoStream* output_stream_b = mic_b.stream();
  
  std::vector<Microphone*> microphones(2);
  microphones[0] = &mic_a;
  microphones[1] = &mic_b;
  
  Source source_a(Point(-one_sample_space, 0.0, 0.0));
  source_a.stream()->Push(signal);
  
  Source source_b(Point(3*one_sample_space, 0.0, 0.0));
  source_b.stream()->Push(signal);
  
  std::vector<Source*> sources(2);
  sources[0] = &source_a;
  sources[1] = &source_b;
  
  FreeFieldSim sim(microphones, sources, sampling_frequency, SOUND_SPEED);
  sim.Run();
  
  std::vector<Sample> output_mic_0_cmp = mcl::Zeros<Sample>(4);
  output_mic_0_cmp[1] = 0.5;
  output_mic_0_cmp[3] = 0.5/3.0;
  std::vector<Sample> output_mic_1_cmp = mcl::Zeros<Sample>(4);
  output_mic_1_cmp[2] = 0.5/2.0+0.5/2.0;
  
  Signal output_a = output_stream_a->PullAll();
  Signal output_b = output_stream_b->PullAll();
  assert(mcl::IsEqual(output_mic_0_cmp, output_a));
  assert(mcl::IsEqual(output_mic_1_cmp, output_b));
  
  
  
  
  return true;
}
  
} // namespace sal
