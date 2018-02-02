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

  
/** An array of microphone each with monophonic outputs */
class SAL_API MicrophoneArray : public Microphone {
public:
  MicrophoneArray(mcl::Point position,
                  mcl::Quaternion orientation,
                  mcl::Handedness handedness = mcl::right_handed) :
          Microphone(position, orientation, handedness) {}
  
  virtual void Tick();
  
  virtual void set_position(const mcl::Point&);
  
  /** 
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  virtual bool IsCoincident();
  
  void InitStream();
  
  MultichannelStream* stream() {
    if (! stream_.initialised()) { InitStream(); }
    return &stream_;
  }
  
  std::vector<MonoMic*> microphones() { return microphone_pointers_; }
  
  static bool Test();
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
  CircularArray(const mcl::Point& position,
                const mcl::Quaternion& orientation,
                const Length radius,
                const UInt num_microphones,
                const Angle span_angle);
  
  static std::vector<Angle> GetAngles(const UInt num_microphones,
                                      const Angle first_element_heading,
                                      const Angle span_angle);
  
  static std::vector<mcl::Point> GetPositions(const mcl::Point& position,
                                         const Length radius,
                                         const UInt num_microphones,
                                         const Angle first_element_heading,
                                         const Angle span_angle);
protected:
  std::vector<Angle> angles_;
  std::vector<mcl::Point> positions_;
};
  

class SAL_API CircularTrig : public CircularArray {
public:
  CircularTrig(const mcl::Point& position,
               const mcl::Quaternion& orientation,
               const Length radius,
               const UInt num_microphones,
               const Angle span_angle,
               std::vector<Sample> coefficients);
private:
  std::vector<TrigMic> microphones_;
};
  
  
/**
 This generates a stereophonic microphone array centered in position,
 with radius `radius`. For identity orientation, the first microphone
 element is at angle 0, i.e.
 at (position.x() + radius, position.y(), position.z()).
 */
class SAL_API StereoMic : public CircularArray {
public:
  StereoMic(const mcl::Point& position,
            const mcl::Quaternion& orientation,
            const Length radius,
            const Angle base_angle) :
  CircularArray(position, orientation,
                radius, 2, base_angle) {}
};


} // namespace sal
  
#endif
