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

#include "source.h"
#include "saltypes.h"
#include "point.h"
#include "microphone.h"
#include "propagationline.h"
#include <vector>
#include "salconstants.h"

namespace sal {
  
class FreeFieldSim {
public:
  FreeFieldSim(std::vector<Microphone*> microphones,
               std::vector<Source*> sources,
               const Time sampling_frequency,
               const Length sound_speed);
  
  FreeFieldSim(Microphone* microphones,
               std::vector<Source*> sources,
               const Time sampling_frequency,
               const Length sound_speed);
  
  FreeFieldSim(std::vector<Microphone*> microphones,
               Source* sources,
               const Time sampling_frequency,
               const Length sound_speed);
  
  FreeFieldSim(Microphone* microphones,
               Source* sources,
               const Time sampling_frequency,
               const Length sound_speed);
  
  void Init(std::vector<Microphone*> microphones,
            std::vector<Source*> sources,
            const Time sampling_frequency,
            const Length sound_speed);
  
  void Run(std::vector<MonoBuffer*> input_buffers,
           const Int num_output_samples,
           std::vector<Buffer*> output_buffers);
  
  void AllocateTempBuffers(const Int num_samples);
  void DeallocateTempBuffers();
  
  ~FreeFieldSim();
  
  static bool Test();
private:
  
  
  void Tick();
  
  /** Returns the minimum distance between any source and any microphone. */
  static Length MinimumDistance(const std::vector<Microphone*>& microphones,
                                const std::vector<Source*>& sources);
  
  /** Returns the maximum distance between any source and any microphone. */
  static Length MaximumDistance(const std::vector<Microphone*>& microphones,
                                const std::vector<Source*>& sources);
  
  static std::vector<Length>
  AllDistances(const std::vector<Microphone*>& microphones,
               const std::vector<Source*>& sources);
  
  
  
  std::vector<std::vector<PropagationLine*> > propagation_lines_;
  std::vector<std::vector<MonoBuffer*> > temp_buffers_;
  
  std::vector<Microphone*> microphones_;
  std::vector<Source*> sources_;
  Time sampling_frequency_;
  Speed sound_speed_;
};
  
} // namespace sal

#endif
