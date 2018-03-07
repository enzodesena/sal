/*
 microphonearray.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_MICROPHONEARRAY_H
#define SAL_MICROPHONEARRAY_H


#include "microphone.h"
#include "point.h"
#include "source.h"
#include "microphone.h"
#include "saltypes.h"
#include "stream.h"
#include "salconstants.h"
#include "monomics.h"
#include "salconstants.h"

namespace sal {

  
/** This object creates a microphone array based on a microphone prototype.
 So, for instance, you can take an omnimic, and create an array of N omni mics.
 This class is a parent for more specific classes, e.g. a circular array etc.
 This class simply creates N copies of the mic_prototype and repositions them
 in the set `position`, with given `orientation` and `handedness`. A call
 to RecordPlaneWave will call the RecordPlaneWave of each microphone.
 You can then access the streams of each microphone by first extracting
 pointers to the microphone with the microphones() method and then accessing
 each one's stream. Alternatively, the object also has a multichannel stream
 object, which contains pointers to the individual streams. */
template<class T>
class SAL_API MicrophoneArray : public Microphone {
static_assert(std::is_base_of<MonoMic, T>::value,
              "You can only create microphone arrays of MonoMics");
  
public:
  MicrophoneArray(const mcl::Point& position,
                  const mcl::Quaternion& orientation,
                  const T& mic_prototype,
                  const mcl::UInt num_microphones) :
          Microphone(position, orientation) {
    std::vector<MonoStream*> streams;
            
    for (UInt i=0; i<num_microphones; ++i) {
      T* microphone = new T(mic_prototype);
      microphone->set_position(position);
      streams.push_back(microphone->stream());
      microphones_.push_back(microphone);
    }

    stream_ = MultichannelStream(streams);
  }
  
  virtual void Tick() {
    UInt num_microphones(microphones_.size());
    for (UInt i=0; i<num_microphones; ++i) {
      // Each microphone will push in his own mono stream. The multichannel
      // stream is merely a vector of pointers to the individual mono streams
      microphones_[i]->Tick();
    }
  }
  
  /**
   This method will move all the internal microphones to a new position.
   The relative positions of the different microphones will stay unchanged.
   */
  virtual void set_position(const mcl::Point& position) {
    mcl::Point position_delta(position.x()-position_.x(),
                              position.y()-position_.y(),
                              position.z()-position_.z());
    for (UInt i=0; i<microphones_.size(); ++i) {
      mcl::Point old_mic_position = microphones_[i]->position();
      mcl::Point new_mic_position(old_mic_position.x()+position_delta.x(),
                                  old_mic_position.y()+position_delta.y(),
                                  old_mic_position.z()+position_delta.z());
      microphones_[i]->set_position(new_mic_position);
    }
    position_ = position;
  }

  /**
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  virtual bool IsCoincident() {
    const UInt num_microphones = microphones_.size();
    
    if (num_microphones == 0 || num_microphones == 1) { return true; }
    
    mcl::Point position(microphones_[0]->position());
    for (UInt i=1; i<num_microphones; ++i) {
      if (! IsEqual(microphones_[i]->position(), position)) {
        return false;
      }
    }
    return true;
  }

  MultichannelStream* stream() { return &stream_; }

  std::vector<T*> microphones() { return microphones_; }

  static bool Test();
  
  ~MicrophoneArray() {
    for (UInt i=0; i<microphones_.size();++i) {
      delete microphones_[i];
    }
    microphones_.clear();
  }
  
private:
  
  MultichannelStream stream_;
  
  /**
   Simulates the output of the microphone array to a source in the direction
   of source.position() and with input signal `source.signal()`.
   This does not include attenuation nor delay due to propagation. These
   are in fact included in the `FreeFieldSimulation` in SAL.
   */
  virtual void RecordPlaneWaveRelative(const Sample& sample,
                                       const mcl::Point& point,
                                       const UInt& wave_id) {
    UInt num_microphones(microphones_.size());
    for (UInt i=0; i<num_microphones; ++i) {
      // Each microphone will push in his own mono stream. The multichannel
      // stream is merely a vector of pointers to the individual mono streams
      ((MonoMic*) microphones_[i])->RecordPlaneWaveRelative(sample, point, wave_id);
    }
  }
  
protected:
  std::vector<T*> microphones_;
  

};
  

/**
 This generates a microphone array centered in position, with radius
 `radius`, `num_microphones` is the number of microphones,
 first_element_heading is the heading of first microphone - e.g.
 first_element_heading=0, means that the first microphone is
 at (position.x() + radius, position.y(), position.z()).
 A span_angle == 0 will position the microphones uniformly around 2PI
 */
template<class T>
class SAL_API CircularArray : public MicrophoneArray<T> {
public:
  CircularArray(const mcl::Point& position,
                const mcl::Quaternion& orientation,
                const T& mic_prototype,
                const Length radius,
                const std::vector<Angle>& angles) :
        MicrophoneArray<T>(position, orientation,
                           mic_prototype, angles.size()) {
    
    std::vector<mcl::Point> positions = GetPositions(position, radius, angles);
          
    for (mcl::UInt i=0; i<angles.size(); ++i) {
      this->microphones_[i]->set_position(positions[i]);
      this->microphones_[i]->set_orientation(mcl::AxAng2Quat(0, 0, 1, angles[i]));
    }
  }
  
  static std::vector<Angle> UniformAngles(const UInt num_microphones,
                                          const Angle first_element_heading) {
    std::vector<Angle> angles(num_microphones);
    for (UInt i=0; i<num_microphones; ++i) {
      // In this case position the microphones uniformly around 2PI
      angles[i] = first_element_heading +
      2.0*PI/((Angle) num_microphones)*((Angle) i);
    }
    return angles;
  }

  
private:
  static std::vector<mcl::Point> GetPositions(const mcl::Point& position,
                                              const Length radius,
                                              const std::vector<Angle>& angles) {
    std::vector<mcl::Point> positions(angles.size());
    for (UInt i=0; i<angles.size(); ++i) {
      positions[i] = mcl::Point(radius*cos(angles[i])+position.x(),
                                radius*sin(angles[i])+position.y(),
                                position.z());
    }
    return positions;
  }
};
  

/**
 This generates a stereophonic microphone array centered in position,
 with radius `radius`. For identity orientation, the first microphone
 element is at angle 0, i.e.
 at (position.x() + radius, position.y(), position.z()).
 */
template<class T>
class SAL_API StereoMic : public CircularArray<T> {
public:
  StereoMic(const mcl::Point& position,
            const mcl::Quaternion& orientation,
            const T& mic_prototype,
            const Length radius,
            const Angle base_angle,
            const Angle center_angle) :
  CircularArray<T>(position, orientation,
                   mic_prototype, radius,
                   StereoAngles(base_angle, center_angle)) {}
  
private:
  static std::vector<Angle> StereoAngles(const Angle base_angle,
                                         const Angle center_angle) {
    std::vector<Angle> angles(2);
    angles[0] = center_angle-base_angle/2.0;
    angles[1] = center_angle+base_angle/2.0;
    return angles;
  }
};


bool MicrophoneArrayTest();
  
} // namespace sal
  
#endif
