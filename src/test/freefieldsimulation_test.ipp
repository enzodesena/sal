/*
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "freefieldsimulation.hpp"
#include "audiobuffer.hpp"


namespace sal
{
inline bool FreeFieldSimTest()
{
  Time sampling_frequency = 44100;
  Length one_sample_space(SOUND_SPEED / sampling_frequency);

  const Int num_samples = 1;
  const Int num_output_samples = 4;
  mcl::Vector<mcl::Vector<Sample>> input_buffers(2);
  input_buffers[0] = mcl::Vector<Sample>(num_samples, 0.5);
  input_buffers[1] = mcl::Vector<Sample>(num_samples, 0.5);
  Buffer<Sample> input = Buffer<Sample>::MakeBufferReference(input_buffers);
  
  mcl::Vector<Source> sources(2);
  sources[0] = Source(Point(-one_sample_space, 0.0, 0.0));
  sources[1] = Source(Point(3 * one_sample_space, 0.0, 0.0));
  
  // Source/mics distribution
  // -1  0  1  2  3 Position in space
  //  x  o  o     x (x: source) (o: microphone)
  mcl::Vector<Receiver<Sample>> receivers
  (
    2,
    Receiver<Sample>
    (
      OmniDirectivity<Sample>(),
      Point(0.0, 0.0, 0.0),
      Quaternion::Identity(),
      sources.size()));
  receivers[1].SetPosition(Point(one_sample_space, 0, 0));

  Buffer<Sample> output(receivers.size(), num_output_samples);
  
  FreeFieldSim<Sample> sim(receivers, sources, sampling_frequency, SOUND_SPEED);
  sim.Run(input, output);

  mcl::Vector<Sample> output_mic_0_cmp = mcl::Zeros<Sample>(4);
  output_mic_0_cmp[1] = 0.5;
  output_mic_0_cmp[3] = 0.5 / 3.0;
  mcl::Vector<Sample> output_mic_1_cmp = mcl::Zeros<Sample>(4);
  output_mic_1_cmp[2] = 0.5 / 2.0 + 0.5 / 2.0;
  ASSERT(mcl::IsApproximatelyEqual(output_mic_0_cmp, output[0], VERY_SMALL));
  ASSERT(mcl::IsApproximatelyEqual(output_mic_1_cmp, output[1], VERY_SMALL));

  return true;
}
} // namespace sal
