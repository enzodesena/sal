/*
 simulation.h
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.

 This class calculates the signals recorded by N microphones due to M acoustic
 sources in free-field.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_SIMULATION_H
#define SAL_SIMULATION_H

#define DEFAULT_MAX_BUFFER 10

#include "source.hpp"
#include "saltypes.hpp"
#include "point.hpp"
#include "receiver.hpp"
#include "propagationline.hpp"
#include "vector.hpp"
#include "salconstants.hpp"

namespace sal
{
class FreeFieldSim
{
public:
  FreeFieldSim(
    mcl::Vector<Microphone*> microphones,
    mcl::Vector<Source*> sources,
    Time sampling_frequency,
    Length sound_speed);

  FreeFieldSim(
    Microphone* microphones,
    mcl::Vector<Source*> sources,
    Time sampling_frequency,
    Length sound_speed);

  FreeFieldSim(
    mcl::Vector<Microphone*> microphones,
    Source* sources,
    Time sampling_frequency,
    Length sound_speed);

  FreeFieldSim(
    Microphone* microphones,
    Source* sources,
    Time sampling_frequency,
    Length sound_speed);

  void Init(
    mcl::Vector<Microphone*> microphones,
    mcl::Vector<Source*> sources,
    Time sampling_frequency,
    Length sound_speed);

  void Run(
    mcl::Vector<MonoBuffer*> input_buffers,
    Int num_output_samples,
    mcl::Vector<Buffer*> output_buffers);

  void AllocateTempBuffers(
    Int num_samples);
  void DeallocateTempBuffers();

  ~FreeFieldSim();

  static bool Test();
private:

  void Tick();

  /** Returns the minimum distance between any source and any microphone. */
  static Length MinimumDistance(
    const mcl::Vector<Microphone*>& microphones,
    const mcl::Vector<Source*>& sources);

  /** Returns the maximum distance between any source and any microphone. */
  static Length MaximumDistance(
    const mcl::Vector<Microphone*>& microphones,
    const mcl::Vector<Source*>& sources);

  static mcl::Vector<Length>
  AllDistances(
    const mcl::Vector<Microphone*>& microphones,
    const mcl::Vector<Source*>& sources);

  std::vector<mcl::Vector<PropagationLine*>> propagation_lines_;
  std::vector<mcl::Vector<MonoBuffer*>> temp_buffers_;

  mcl::Vector<Microphone*> microphones_;
  mcl::Vector<Source*> sources_;
  Time sampling_frequency_;
  Speed sound_speed_;
};
} // namespace sal

#endif
