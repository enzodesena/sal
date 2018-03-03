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
class SAL_API MicrophoneArray : public Microphone {
public:
  MicrophoneArray(const sal::MonoMic& mic_prototype,
                  const mcl::UInt num_microphones,
                  const mcl::Point position,
                  const mcl::Quaternion orientation,
                  const mcl::Handedness handedness = mcl::right_handed) :
          Microphone(position, orientation, handedness) {
    
    std::vector<MonoStream*> streams(num_microphones);
    microphones_.reserve(num_microphones);
    for (UInt i=0; i<num_microphones; ++i) {
      microphones_.push_back(mic_prototype);
      microphones_[i].set_position(position);
      microphones_[i].set_orientation(orientation);
      microphones_[i].set_handedness(handedness);
      streams[i] = microphone_pointers_[i]->stream();
    }
    
    stream_ = MultichannelStream(streams);
  }
  
  virtual void Tick();
  
  /**
   This method will move all the internal microphones to a new position.
   The relative positions of the different microphones will stay unchanged.
   */
  virtual void set_position(const mcl::Point&);
  
  // TODO: Implement this method
  virtual void set_orientation(const mcl::Quaternion&) { assert(false); }
  
  /** 
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  virtual bool IsCoincident();
  
  MultichannelStream* stream() { return &stream_; }
  
  std::vector<MonoMic*> microphones() { return microphone_pointers_; }
  
  static bool Test();
protected:
  std::vector<MonoMic> microphones_;
  
private:
  /**
   Simulates the output of the microphone array to a source in the direction
   of source.position() and with input signal `source.signal()`.
   This does not include attenuation nor delay due to propagation. These
   are in fact included in the `FreeFieldSimulation` in SAL.
   */
  virtual void RecordPlaneWaveRelative(const Sample& sample, const mcl::Point& point,
                                       const UInt& wave_id);
  
  MultichannelStream stream_;
protected:
  std::vector<MonoMic*> microphone_pointers_;
};

/**
 This generates a microphone array centered in position, with radius
 `radius`, `num_microphones` is the number of microphones,
 first_element_heading is the heading of first microphone - e.g.
 first_element_heading=0, means that the first microphone is
 at (position.x() + radius, position.y(), position.z()). Finally,
 directivity_pattern is the directivity pattern of each microphone.
 A span_angle == 0 will position the microphones uniformly around 2PI
 */
class SAL_API CircularArray : public MicrophoneArray {
public:
  CircularArray(const sal::MonoMic& mic_prototype,
                const UInt num_microphones,
                const Length radius,
                const Angle span_angle,
                const mcl::Point& position,
                const mcl::Quaternion& orientation,
                const mcl::Handedness& handedness = mcl::right_handed);
  
private:
  static std::vector<Angle> GetAngles(const UInt num_microphones,
                                      const Angle first_element_heading,
                                      const Angle span_angle);
  
  static std::vector<mcl::Point> GetPositions(const mcl::Point& position,
                                              const Length radius,
                                              const UInt num_microphones,
                                              const Angle first_element_heading,
                                              const Angle span_angle);
};
  
  
/**
 This generates a stereophonic microphone array centered in position,
 with radius `radius`. For identity orientation, the first microphone
 element is at angle 0, i.e.
 at (position.x() + radius, position.y(), position.z()).
 */
class SAL_API StereoMic : public CircularArray {
public:
  StereoMic(const sal::MonoMic& mic_prototype,
            const UInt num_microphones,
            const Length radius,
            const Angle base_angle,
            const mcl::Point& position,
            const mcl::Quaternion& orientation,
            const mcl::Handedness& handedness = mcl::right_handed) :
  CircularArray(mic_prototype, 2, radius, base_angle,
                position, orientation, handedness) {}
};


} // namespace sal
  
#endif
