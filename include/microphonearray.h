/*
 microphonearray.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_MICROPHONEARRAY_H
#define SAL_MICROPHONEARRAY_H


#include "microphone.h"
#include "point.h"
#include "source.h"
#include "microphone.h"
#include "saltypes.h"
#include "salconstants.h"
#include "monomics.h"
#include "salconstants.h"

namespace sal {

/** This is a microhone array, containing pointers to other microphones. A call
 to AddPlaneWave will call the AddPlaneWave of each microphone.
 You can then access the streams of each microphone by first extracting
 pointers to the microphone with the microphones() method and then accessing
 each one's stream. Alternatively, the object also has a multichannel stream
 object, which contains pointers to the individual streams. */
class MicrophoneArray : public Microphone {
public:
  MicrophoneArray(const mcl::Point& position,
                  const mcl::Quaternion& orientation,
                  const std::vector<Microphone*>& microphones) :
      Microphone(position, orientation), microphones_(microphones) {}

  
  /**
   This method will move all the internal microphones to a new position.
   The relative positions of the different microphones will stay unchanged.
   */
  virtual void SetPosition(const mcl::Point& position) noexcept {
    mcl::Point position_delta(position.x()-this->position().x(),
                              position.y()-this->position().y(),
                              position.z()-this->position().z());
    for (Int i=0; i<(Int)microphones_.size(); ++i) {
      mcl::Point old_mic_position = microphones_[i]->position();
      mcl::Point new_mic_position(old_mic_position.x()+position_delta.x(),
                                  old_mic_position.y()+position_delta.y(),
                                  old_mic_position.z()+position_delta.z());
      microphones_[i]->SetPosition(new_mic_position);
    }
    
    Microphone::SetPosition(position);
  }
  
  

  void SetOrientation(const mcl::Quaternion& orientation) noexcept {
    for (Int i=0; i<(Int)microphones_.size(); ++i) {
      microphones_[i]->SetOrientation(orientation);
    }
    orientation_ = orientation;
  }

  /**
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  bool IsCoincident() const noexcept {
    const Int num_microphones = (Int)microphones_.size();

    if (num_microphones == 0 || num_microphones == 1) { return true; }

    mcl::Point position(microphones_[0]->position());
    for (Int i=1; i<num_microphones; ++i) {
      if (! IsEqual(microphones_[i]->position(), position)) {
        return false;
      }
    }
    return true;
  }
  
  Int num_channels() const noexcept {
    return microphones_.size();
  }

  const Microphone* GetConstMicrophonePointer(const Int microphone_id) const noexcept {
    return microphones_.at(microphone_id);
  }
  
  Microphone* GetMicrophonePointer(const Int microphone_id) noexcept {
    return microphones_.at(microphone_id);
  }
  
  std::vector<Microphone*> GetMicrophonePointers() const noexcept {
    return microphones_;
  }
  
  std::vector<const Microphone*> GetConstMicrophonePointers() const noexcept {
    std::vector<const Microphone*> microphones(num_microphones());
    for (Int i=0; i<num_microphones(); ++i) {
      microphones[i] = (const Microphone*) microphones_[i];
    }
    return microphones;
  }

  Int num_microphones() const noexcept {
    return microphones_.size();
  }
  
  static bool Test();

  virtual ~MicrophoneArray() {}


  /**
   Simulates the output of the microphone array to a source in the direction
   of source.position() and with input signal `source.signal()`.
   This does not include attenuation nor delay due to propagation.
   If you wish to use this method, the buffer has to be a multichannel buffer,
   and all the underlying microphones need to be monophonic. If that is not
   the case, then you need to use the AddPlaneWave of the underlying microphone
   objects directly.
   */
  virtual void AddPlaneWave(const Sample* input_data,
                            const Int num_samples,
                            const mcl::Point& point,
                            const Int wave_id,
                            Buffer& output_buffer) noexcept {
    Int num_microphones((Int)microphones_.size());
    for (Int mic_i=0; mic_i<num_microphones; ++mic_i) {
      MonoBuffer referencing_buffer(output_buffer, mic_i);
      // Each microphone will push in his own mono stream. The multichannel
      // stream is merely a vector of pointers to the individual mono streams
      microphones_[mic_i]->AddPlaneWave(input_data, num_samples, point,
                                        wave_id, referencing_buffer);
    }
  }
  
  virtual void AddPlaneWaveRelative(const Sample* input_data,
                                    const Int num_samples,
                                    const mcl::Point& point,
                                    const Int wave_id,
                                    Buffer& output_buffer) noexcept {
    // This method can't be called directly (should call AddPlaneWave instead.
    // That's because the relative point needs to be calculated with respect
    // to each microphone individually. If you could call
    // AddPlaneWaveRelative directly, on the other hand, it would carry out
    // the same rotation for all of them.
    ASSERT(false);
  }
protected:
  std::vector<Microphone*> microphones_;
};

  
/** This object creates a microphone array based on a microphone prototype.
 So, for instance, you can take an omnimic, and create an array of N omni mics.
 This class is a parent for more specific classes, e.g. a circular array etc.
 This class simply creates N copies of the mic_prototype and repositions them
 in the set `position`, with given `orientation` and `handedness`. */
template<class T>
class UniformArray : public MicrophoneArray {
public:
  UniformArray(const mcl::Point& position,
               const mcl::Quaternion& orientation,
               const T& mic_prototype,
               const mcl::Int num_microphones) :
  MicrophoneArray(position, orientation,
                  MicrophoneFactory(mic_prototype, num_microphones)) {}
  
  virtual ~UniformArray() {
    for (Int i=0; i<(Int) microphones_.size(); ++i) { delete microphones_[i]; }
  }
  
private:
  std::vector<Microphone*>
  MicrophoneFactory(const T& mic_prototype, const Int num_microphones) {
    std::vector<Microphone*> output(num_microphones);
    for (Int i=0; i<num_microphones; ++i) {
      output[i] = new T(mic_prototype);
    }
    return output;
  }
};

/**
 This generates a microphone array centered in position, with radius
 `radius`, `num_microphones` is the number of microphones,
 first_element_heading is the heading of first microphone - e.g.
 first_element_heading=0, means that the first microphone is
 at (position.x() + radius, position.y(), position.z()).
 */
template<class T>
class CircularArray : public UniformArray<T> {
public:
  CircularArray(const mcl::Point& position,
                const mcl::Quaternion& orientation,
                const T& mic_prototype,
                const Length radius,
                const std::vector<Angle>& angles) :
        UniformArray<T>(position, orientation, mic_prototype, angles.size()),
      radius_(radius), angles_(angles) {
    SetOrientation(orientation);
  }

  virtual void SetOrientation(const mcl::Quaternion& orientation) noexcept {
    mcl::Point position(this->position());
    std::vector<mcl::Point> positions = GetPositions(position, radius_, angles_);

    for (mcl::Int i=0; i<(Int)angles_.size(); ++i) {
      Angle mic_angle = (this->handedness_ == mcl::Handedness::kRightHanded) ? angles_[i] : -angles_[i];
      mcl::Point relative_position = mcl::QuatRotate(orientation,
                                                     mcl::Subtract(positions[i],
                                                                   position),
                                                     this->handedness_);
      this->microphones_[i]->SetPosition(mcl::Sum(relative_position, position));
      
      mcl::Quaternion q = mcl::QuatMultiply(orientation, mcl::AxAng2Quat(0.0, 0.0, 1.0, mic_angle));
      this->microphones_[i]->SetOrientation(q);
    }
  }

private:
  Length radius_;
  std::vector<Angle> angles_;

  static std::vector<mcl::Point> GetPositions(const mcl::Point& position,
                                              const Length radius,
                                              const std::vector<Angle>& angles) {
    std::vector<mcl::Point> positions(angles.size());
    for (Int i=0; i<(Int)angles.size(); ++i) {
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
class StereoMic : public CircularArray<T> {
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
