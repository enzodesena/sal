/*
 simulation.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This class calculates the signals recorded by N microphones due to M acoustic
 sources in free-field.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */



#include "freefieldsimulation.h"
#include "saltypes.h"
#include "propagationline.h"
#include "point.h"
#include "source.h"
#include "microphone.h"
#include "mcl.h"
#include <vector>

using mcl::Point;

namespace sal {
  
FreeFieldSim::FreeFieldSim(std::vector<Microphone*> microphones,
                           std::vector<Source*> sources,
                           const Time sampling_frequency,
                           const Length sound_speed) {
  Init(microphones, sources, sampling_frequency, sound_speed);
}

FreeFieldSim::FreeFieldSim(Microphone* microphone,
                           std::vector<Source*> sources,
                           const Time sampling_frequency,
                           const Length sound_speed) {
  Init(mcl::UnaryVector<Microphone*>(microphone),
       sources,
       sampling_frequency, sound_speed);
}
  
FreeFieldSim::FreeFieldSim(std::vector<Microphone*> microphones,
                           Source* source,
                           const Time sampling_frequency,
                           const Length sound_speed) {
  Init(microphones,
       mcl::UnaryVector<Source*>(source),
       sampling_frequency, sound_speed);
}
  
FreeFieldSim::FreeFieldSim(Microphone* microphone,
                           Source* source,
                           const Time sampling_frequency,
                           const Length sound_speed) {
  Init(mcl::UnaryVector<Microphone*>(microphone),
       mcl::UnaryVector<Source*>(source),
       sampling_frequency, sound_speed);
}
  
void FreeFieldSim::Init(std::vector<Microphone*> microphones,
                        std::vector<Source*> sources,
                        const Time sampling_frequency,
                        const Length sound_speed) {
  microphones_ = microphones;
  sources_ = sources;
  sampling_frequency_ = sampling_frequency;
  sound_speed_ = sound_speed;
  const UInt num_microphones = microphones_.size();
  const UInt num_sources = sources_.size();
  
  propagation_lines_ = std::vector<std::vector<PropagationLine*> >(num_sources);
  
  // Define the propagation lines
  for (UInt source_i=0; source_i<num_sources; ++source_i) {
    Source* source = sources_[source_i];
    propagation_lines_[source_i] =
    std::vector<PropagationLine*>(num_microphones);
    for (UInt mic_i=0; mic_i<num_microphones; ++mic_i) {
      Microphone* microphone = microphones_[mic_i];
      
      Length distance = Point::Distance(source->position(),
                                        microphone->position());
      
      propagation_lines_[source_i][mic_i] =
      new PropagationLine(distance, sampling_frequency);
    }
  }
}
  


FreeFieldSim::~FreeFieldSim() {
  const UInt num_microphones = microphones_.size();
  const UInt num_sources = sources_.size();
  
  for (UInt mic_i=0; mic_i<num_microphones; ++mic_i) {
    for (UInt source_i=0; source_i<num_sources; ++source_i) {
      delete propagation_lines_[source_i][mic_i];
    }
  }
}

void FreeFieldSim::Run() {
  const UInt num_microphones = microphones_.size();
  const UInt num_sources = sources_.size();
  //const Length min_distance = MinimumDistance(microphones, sources);
  
  // This is the time that we need to wait until all samples are delivered to
  // all microphones
  const Time void_time = MaximumDistance(microphones_, sources_)/sound_speed_;
  Int void_samples = (Int) round(void_time*sampling_frequency_);
  
  while(true) {
    // Run break checks
    if (AllSourcesEmpty(sources_)) {
      // After all streams are depleted, wait until all samples reach
      // all microphones.
      if (void_samples-- == 0) {
        break;
      }
    }
    
    for (UInt source_i=0; source_i<num_sources; ++source_i) {
      Source* source = sources_[source_i];
      MonoStream* stream = source->stream();
      Sample sample = (stream->IsEmpty()) ? 0.0 : stream->Pull();
      
      for (UInt mic_i=0; mic_i<num_microphones; ++mic_i) {
        Microphone* microphone = microphones_[mic_i];
      
        propagation_lines_[source_i][mic_i]->Write(sample);
        microphone->RecordPlaneWave(propagation_lines_[source_i][mic_i]->Read(),
                                    source->position(), source_i);
      }
    }
    
    // Pass to next time sample (tick microphones and propagation lines).
    Tick();
  }
}

  
void FreeFieldSim::Tick() {
  const UInt num_microphones = microphones_.size();
  const UInt num_sources = sources_.size();
  
  for (UInt mic_i=0; mic_i<num_microphones; ++mic_i) {
    Microphone* microphone = microphones_[mic_i];
    microphone->Tick();
    
    for (UInt source_i=0; source_i<num_sources; ++source_i) {
      propagation_lines_[source_i][mic_i]->Tick();
    }
  }
}
  
bool FreeFieldSim::AllSourcesEmpty(const std::vector<Source*>& sources) {
  const UInt num_sources = sources.size();
  for (UInt i=0; i<num_sources; ++i) {
    if (! sources[i]->stream()->IsEmpty()) {
      return false;
    }
  }
  return true;
}
  
std::vector<Length>
FreeFieldSim::AllDistances(const std::vector<Microphone*>& microphones,
                           const std::vector<Source*>& sources) {
  const UInt num_microphones = microphones.size();
  const UInt num_sources = sources.size();
  
  std::vector<Length> distances;
  for (UInt mic_index=0; mic_index<num_microphones; ++mic_index) {
    for (UInt source_index=0; source_index<num_sources; ++source_index) {
      distances.push_back(Point::Distance(microphones[mic_index]->position(),
                                          sources[source_index]->position()));
    }
  }
  
  return distances;
}

Length FreeFieldSim::MinimumDistance(const std::vector<Microphone*>& microphones,
                                     const std::vector<Source*>& sources) {
  return mcl::Min(AllDistances(microphones, sources));
}
  
Length FreeFieldSim::MaximumDistance(const std::vector<Microphone*>& microphones,
                                     const std::vector<Source*>& sources) {
  return mcl::Max(AllDistances(microphones, sources));
}
  

} // namespace sal
