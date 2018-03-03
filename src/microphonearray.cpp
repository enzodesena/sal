/*
 microphonearray.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "microphonearray.h"
#include "exception.h"
#include <vector>

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
  
void MicrophoneArray::set_position(const mcl::Point& position) {
  Point position_delta(position.x()-position_.x(),
                       position.y()-position_.y(),
                       position.z()-position_.z());
  for (UInt i=0; i<microphone_pointers_.size(); ++i) {
    Point old_mic_position = microphone_pointers_[i]->position();
    Point new_mic_position(old_mic_position.x()+position_delta.x(),
                           old_mic_position.y()+position_delta.y(),
                           old_mic_position.z()+position_delta.z());
    microphone_pointers_[i]->set_position(new_mic_position);
  }
  position_ = position;
}
  
void MicrophoneArray::RecordPlaneWaveRelative(const Sample& sample,
                                              const Point& point,
                                              const UInt& wave_id) {
  UInt num_microphones(microphone_pointers_.size());
  for (UInt i=0; i<num_microphones; ++i) {
    // Each microphone will push in his own mono stream. The multichannel
    // stream is merely a vector of pointers to the individual mono streams
    microphone_pointers_[i]->RecordPlaneWaveRelative(sample, point, wave_id);
  }
}
  
void MicrophoneArray::Tick() {
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
    if (! IsEqual(microphone_pointers_[i]->position(), position)) {
      return false;
    }
  }
  return true;
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
  
CircularArray::CircularArray(const sal::MonoMic& mic_prototype,
                             const UInt num_microphones,
                             const Length radius,
                             const Angle span_angle,
                             const mcl::Point& position,
                             const mcl::Quaternion& orientation,
                             const mcl::Handedness& handedness) :
  MicrophoneArray(mic_prototype, num_microphones,
                  position, orientation, handedness) {
  
  std::vector<sal::Angle> angles =
      GetAngles(num_microphones, 0, span_angle);
  std::vector<mcl::Point> positions =
      GetPositions(position, radius, num_microphones, 0, span_angle);
  for (mcl::UInt i=0; i<num_microphones; ++i) {
    microphones_[i].set_position(positions[i]);
    microphones_[i].set_orientation(mcl::AxAng2Quat(0, 0, 1, angles[i]));
  }
}
  
} // namespace sal
