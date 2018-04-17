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
  const Int num_microphones = microphones_.size();
  const Int num_sources = sources_.size();
  
  propagation_lines_ = std::vector<std::vector<PropagationLine*> >(num_sources);
  
  // Define the propagation lines
  for (Int source_i=0; source_i<num_sources; ++source_i) {
    Source* source = sources_[source_i];
    propagation_lines_[source_i] = std::vector<PropagationLine*>(num_microphones);
    
    for (Int mic_i=0; mic_i<num_microphones; ++mic_i) {
      Microphone* microphone = microphones_[mic_i];
      
      Length distance = Distance(source->position(), microphone->position());
      
      propagation_lines_[source_i][mic_i] =
      new PropagationLine(distance, sampling_frequency);
    }
  }
  // Allocate temporary buffers
  AllocateTempBuffers(DEFAULT_MAX_BUFFER);
}
  
void FreeFieldSim::AllocateTempBuffers(const Int num_samples) {
  temp_buffers_ = std::vector<std::vector<MonoBuffer*> >(sources_.size());
  for (Int source_i=0; source_i<sources_.size(); ++source_i) {
    temp_buffers_[source_i] = std::vector<MonoBuffer*>(microphones_.size());
    for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
      temp_buffers_[source_i][mic_i] = new MonoBuffer(num_samples);
    }
  }
}

void FreeFieldSim::DeallocateTempBuffers() {
  for (Int source_i=0; source_i<sources_.size(); ++source_i) {
  for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
      delete temp_buffers_[source_i][mic_i];
    }
  }
  temp_buffers_ = std::vector<std::vector<MonoBuffer*> >();
}

FreeFieldSim::~FreeFieldSim() {
  for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
    for (Int source_i=0; source_i<sources_.size(); ++source_i) {
      delete propagation_lines_[source_i][mic_i];
    }
  }
  
  DeallocateTempBuffers();
}
  
void FreeFieldSim::Run(std::vector<MonoBuffer*> input_buffers,
                       const Int num_output_samples,
                       std::vector<Buffer*> output_buffers) {
  
  if (num_output_samples > temp_buffers_[0][0]->num_samples()) {
    // This would ideally not happen as it is not lock-free
    DeallocateTempBuffers();
    AllocateTempBuffers(num_output_samples);
  }
  
  for (Int sample_id=0; sample_id<num_output_samples; ++sample_id) {
    for (Int source_i=0; source_i<sources_.size(); ++source_i) {
      for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
        Sample next_input_sample =
            (sample_id < input_buffers[source_i]->num_samples()) ?
            input_buffers[source_i]->GetSample(sample_id) : 0.0;
        propagation_lines_[source_i][mic_i]->Write(next_input_sample);
        Sample next_output_sample = propagation_lines_[source_i][mic_i]->Read();
        temp_buffers_[source_i][mic_i]->SetSample(sample_id, next_output_sample);
      }
    }
    
    // Pass to next time sample (tick propagation lines).
    Tick();
  }
  
  // Write to microphones
  for (Int source_i=0; source_i<sources_.size(); ++source_i) {
    for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
      microphones_[mic_i]->AddPlaneWave(temp_buffers_[source_i][mic_i]->GetReadPointer(),
                                        num_output_samples,
                                        sources_[source_i]->position(), source_i,
                                        *(output_buffers[mic_i]));
    }
  }
}

  
void FreeFieldSim::Tick() {
  for (Int mic_i=0; mic_i<microphones_.size(); ++mic_i) {
    for (Int source_i=0; source_i<sources_.size(); ++source_i) {
      propagation_lines_[source_i][mic_i]->Tick();
    }
  }
}
  
std::vector<Length>
FreeFieldSim::AllDistances(const std::vector<Microphone*>& microphones,
                           const std::vector<Source*>& sources) {
  const Int num_microphones = microphones.size();
  const Int num_sources = sources.size();
  
  std::vector<Length> distances;
  for (Int mic_index=0; mic_index<num_microphones; ++mic_index) {
    for (Int source_index=0; source_index<num_sources; ++source_index) {
      distances.push_back(Distance(microphones[mic_index]->position(),
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
