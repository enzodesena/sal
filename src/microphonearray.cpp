/*
 microphonearray.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "microphonearray.h"
#include <vector>

namespace sal {

  
void MicrophoneArray::RecordPlaneWaveRelative(const Sample& sample,
                                              const Point& point,
                                              const UInt& wave_id) {
  assert(stream_.initialised());
  UInt num_microphones(microphone_pointers_.size());
  for (UInt i=0; i<num_microphones; ++i) {
    // Each microphone will push in his own mono stream. The multichannel
    // stream is merely a vector of pointers to the individual mono streams
    microphone_pointers_[i]->RecordPlaneWaveRelative(sample, point, wave_id);
  }
}
  
void MicrophoneArray::Tick() {
  assert(stream_.initialised());
  UInt num_microphones(microphone_pointers_.size());
  for (UInt i=0; i<num_microphones; ++i) {
    // Each microphone will push in his own mono stream. The multichannel
    // stream is merely a vector of pointers to the individual mono streams
    microphone_pointers_[i]->Tick();
  }
}

bool MicrophoneArray::IsCoincident() {
  const UInt num_microphones = microphone_pointers_.size();
  
  if (num_microphones == 0 || num_microphones == 1) { return true; }
  
  Point position = microphone_pointers_[0]->position();
  for (UInt i=1; i<num_microphones; ++i) {
    if (! Point::IsEqual(microphone_pointers_[i]->position(), position)) {
      return false;
    }
  }
  return true;
}
  

void MicrophoneArray::InitStream() {
  const UInt num_mics = microphone_pointers_.size();
  std::vector<MonoStream*> streams(num_mics);
  for (UInt i=0; i<num_mics; ++i) {
    streams[i] = microphone_pointers_[i]->stream();
  }
  stream_.InitStream(streams);
}

  
std::vector<Angle> CircularArray::GetAngles(const UInt num_microphones,
                             const Angle first_element_heading,
                             const Angle span_angle) {
  std::vector<Angle> angles(num_microphones);
  for (UInt i=0; i<num_microphones; ++i) {
    if (mcl::IsEqual(span_angle, 0.0, std::numeric_limits<Angle>::epsilon())) {
      // In this case position the microphones uniformly around 2PI
      angles[i] = first_element_heading +
            2.0*PI/((Angle) num_microphones)*((Angle) i);
    } else {
      angles[i] = first_element_heading +
      span_angle/(((Angle) num_microphones) - 1.0)*((Angle) i);
    }
  }
  return angles;
}
  
std::vector<Point> CircularArray::GetPositions(const Point& position,
                                const Length radius,
                                const UInt num_microphones,
                                const Angle first_element_heading,
                                const Angle span_angle) {
  std::vector<Angle> angles = GetAngles(num_microphones,
                                        first_element_heading,
                                        span_angle);
  
  std::vector<Point> positions(num_microphones);
  for (UInt i=0; i<num_microphones; ++i) {
    positions[i] = Point(radius*cos(angles[i])+position.x(),
                          radius*sin(angles[i])+position.y(),
                          position.z());
  }
  return positions;
}
  
CircularArray::CircularArray(const Point& position,
                             const Length radius,
                             const UInt num_microphones,
                             const Angle first_element_heading,
                             const Angle span_angle) {
            
  angles_ = GetAngles(num_microphones, first_element_heading, span_angle);
  
  positions_ = GetPositions(position, radius, num_microphones,
                            first_element_heading, span_angle);
}
  
  
  
CircularTrig::CircularTrig(const Point& position,
                           const Length radius,
                           const UInt num_microphones,
                           const Angle first_element_heading,
                           const Angle span_angle,
                           std::vector<Sample> coefficients) :
          CircularArray(position, radius, num_microphones,
                        first_element_heading, span_angle) {
  microphone_pointers_ = std::vector<MonoMic*>(num_microphones);
            
  // The following reservation makes the position in memory of the vector
  // static (TODO: double-check this)
  microphones_.reserve(num_microphones);
            
  for (UInt i=0; i<num_microphones; ++i) {
    // Initialise streams.
    microphones_.push_back(TrigMic(positions_[i], PI/2.0,
                                   angles_[i], 0.0,
                                   coefficients));
    
    microphone_pointers_[i] = &(microphones_[i]);
  }
            
            
            
  InitStream();
}
  



CircularPSR::CircularPSR(const Point& position,
                         const Length radius,
                         const UInt num_microphones,
                         const Angle first_element_heading,
                         const Angle span_angle,
                         const Length sound_speed) :
          CircularArray(position, radius, num_microphones, first_element_heading,
                        span_angle) {
  assert(! mcl::IsEqual(angles_[0], angles_[1]));
  
  microphone_pointers_ = std::vector<MonoMic*>(num_microphones);
  // I have to reserve because otherwise the `microphones_` vector
  // could reassign variables in memory and then `microphone_pointers_
  // would not point anymore to the correct Microphones object.
  // I still don't feel confident that there wont be memory leaks.
  // TODO: consider using container other than `std::vector`.
  microphones_.reserve(10);
            
  for (UInt i=0; i<num_microphones; ++i) {
    // Initialise streams.
    microphones_.push_back(PSRMic(positions_[i], PI/2.0, angles_[i], 0.0,
                                  radius,
                                  abs(angles_[1]-angles_[0]),
                                  sound_speed));
    microphone_pointers_[i] = (MonoMic*) &(microphones_[i]);
  }
  
  InitStream();
}

StereoPSR::StereoPSR(const Point& position,
                     const Length radius,
                     const Angle midline_heading,
                     const Angle base_angle,
                     const Length sound_speed) :
          StereoMic(position, radius, midline_heading, base_angle) {
  const UInt num_microphones = 2;
  
  microphone_pointers_ = std::vector<MonoMic*>(num_microphones);
  microphones_.reserve(num_microphones);
            
  for (UInt i=0; i<num_microphones; ++i) {
    // Initialise streams.
    microphones_.push_back(PSRMic(positions_[i], PI/2.0, angles_[i], 0.0,
                                  radius,
                                  base_angle,
                                  sound_speed));
    microphone_pointers_[i] = (MonoMic*) (&microphones_[i]);
  }

  InitStream();
}
  
  
  
  
} // namespace sal