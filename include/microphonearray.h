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
#include "psrmic.h"
#include "salconstants.h"

namespace sal {

  
/** An array of microphone each with monophonic outputs */
class SAL_API MicrophoneArray : public Microphone {
public:
  
  MicrophoneArray() :
  Microphone(mcl::Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity()) {}
  


  virtual void Tick();
  
  // TODO: implement these two virtual methods
  //  mcl::Point position() const;
  //  virtual void set_position(const mcl::Point&);
  
  /** 
   Returns true if the array is coincident. If there are 0 or 1 microphones
   the array is considered coincident.
   */
  virtual bool IsCoincident();
  
  void InitStream();
  
  MultichannelStream* stream() { return &stream_; }
  
  std::vector<MonoMic*> microphones() { return microphone_pointers_; }
  
  static bool Test();
private:
  /**
   Simulates the output of the microphone array to a source in the direction
   of source.position() and with input signal `source.signal()`.
   This does not include attenuation nor delay due to propagation. These
   are in fact included in the `FreeFieldSimulation` in SAT or are
   are implemented as delay lines in SDN.
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
                const Length radius,
                const UInt num_microphones,
                const Angle first_element_heading,
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
               const Length radius,
               const UInt num_microphones,
               const Angle first_element_heading,
               const Angle span_angle,
               std::vector<Sample> coefficients);
private:
  std::vector<TrigMic> microphones_;
};
  
  


class SAL_API CircularPSR : public CircularArray {
public:
  CircularPSR(const mcl::Point& position,
              const Length radius,
              const UInt num_microphones,
              const Angle first_element_heading,
              const Angle span_angle,
              const Length sound_speed);
  
private:
  std::vector<PSRMic> microphones_;
};

  
/**
 This generates a stereophonic microphone array centered in position,
 with radius `radius`,
 midline_heading is the heading of the midline microphone
 - e.g. first_element_heading=0, means that the first microphone is
 at (position.x() + radius, position.y(), position.z()).
 Finally, directivity_pattern
 is the directivity apttern of each microphone.
 */
class SAL_API StereoMic : public CircularArray {
public:
  StereoMic(const mcl::Point& position,
            const Length radius,
            const Angle midline_heading,
            const Angle base_angle) :
        CircularArray(position, radius, 2, midline_heading-base_angle/2.0,
                      base_angle) {}
};

  
class SAL_API StereoPSR : public StereoMic {
public:
  StereoPSR(const mcl::Point& position,
            const Length radius,
            const Angle midline_heading,
            const Angle base_angle,
            const Length sound_speed);
private:
  std::vector<PSRMic> microphones_;
};

} // namespace sal
  
#endif
