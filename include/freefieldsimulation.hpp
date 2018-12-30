/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.

 This class calculates the signals recorded by N microphones due to M acoustic
 sources in free-field.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "source.hpp"
#include "saltypes.hpp"
#include "source.hpp"
#include "point.hpp"
#include "receiver.hpp"
#include "propagationline.hpp"
#include "vector.hpp"
#include "salconstants.hpp"

namespace sal
{
template<typename T>
class FreeFieldSim
{
public:
  FreeFieldSim(
    const mcl::Vector<Receiver<T>>& microphones,
    const mcl::Vector<Source>& sources,
    const Time sampling_frequency,
    const Length sound_speed);

  FreeFieldSim(
    const Receiver<T>& microphones,
    const mcl::Vector<Source>& sources,
    const Time sampling_frequency,
    const Length sound_speed);

  FreeFieldSim(
    const mcl::Vector<Receiver<T>>& microphones,
    const Source& sources,
    const Time sampling_frequency,
    const Length sound_speed);

  FreeFieldSim(
    const Receiver<T>& microphones,
    const Source& sources,
    const Time sampling_frequency,
    const Length sound_speed);

  void Init(
    const mcl::Vector<Receiver<T>>& microphones,
    const mcl::Vector<Source>& sources,
    const Time sampling_frequency,
    const Length sound_speed);

  void Run(
    const Buffer<T>& input_buffer,
    Buffer<T>& output_buffers) noexcept;

private:

  void Tick();

  /** Returns the minimum distance between any source and any microphone. */
  static Length MinimumDistance(
    const mcl::Vector<Receiver<T>>& microphones,
    const mcl::Vector<Source>& sources);

  /** Returns the maximum distance between any source and any microphone. */
  static Length MaximumDistance(
    const mcl::Vector<Receiver<T>>& microphones,
    const mcl::Vector<Source>& sources);

  static mcl::Vector<Length>
  AllDistances(
    const mcl::Vector<Receiver<T>>& microphones,
    const mcl::Vector<Source>& sources);

  mcl::Vector<mcl::Vector<PropagationLine<T>>> propagation_lines_;

  mcl::Vector<Receiver<T>> receivers_;
  mcl::Vector<Source> sources_;
  Time sampling_frequency_;
  Speed sound_speed_;
};
} // namespace sal


#include "freefieldsimulation.ipp"
