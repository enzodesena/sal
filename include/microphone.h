/*
 microphone.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#ifndef SAL_MICROPHONE_H
#define SAL_MICROPHONE_H

#include "point.h"
#include "stream.h"
#include "source.h"
#include "quaternion.h"
#include <assert.h>
#include <map>
#include "salconstants.h"

namespace sal {

class SAL_API Microphone {
public:
  /**
   `position` is the position of the microphone, and orientation 
   is a quaternion describing the microphone rotation.
   A microphone generally has its acoustical axis on the x-axis.
   We are using a spherical coordinate system with the convention 
   that theta is the angle formed with the z-axis, 
   and phi is the angle formed on the projection on the x-y plane 
   with the x-axis.
   The x-axis corresponds to (r, theta, phi) = (r, pi/2, 0).
   
   An example on the use of this function is as follow:
   if you don't want to change its orientation, then you should use
   a Quaternion::Identical().
   Otherwise, if you want to rotate it around the z-axis (i.e. a
   rotation on the horizontal plane), then you should use 
   mcl::AxAng2Quat(0,0,1,angle) with angle in radians.
   
   Methods with wave_id as a parameter imply that the user should
   explicitly tell the mic to `Tick`, i.e. to inform it that we are working
   on a new sample. Methods without wave_id (i.e. assuming there
   is a single plane wave incoming) do this automatically.
   */
  Microphone(mcl::Point position,
             mcl::Quaternion orientation = mcl::Quaternion::Identity());
  
  /** Returns current position of the microphone */
  mcl::Point position() const;
  
  /** Set microphone position */
  void set_position(const mcl::Point& position);
  
  /** Returns current orientation of the microphone */
  mcl::Quaternion orientation() const;
  
  /** Set microphone orientation */
  void set_orientation(const mcl::Quaternion& orientation);
  
  /** Set handedness of reference system */
  void set_handedness(const mcl::Handedness handedness);
  
  /**
   We do not implement directly the case of a single plane wave because in
   most situations the microphone is recording many plane waves at the same
   time. This method should only be called in case of a single plane wave
   impinging on the microphone. For multiple plane waves, you need to
   explicitly specify the wave_id.
   */
  void RecordPlaneWave(const Sample& sample, const mcl::Point& point);
  
  /**
   This method should only be called in case of a single plane wave
   impinging on the microphone. For multiple plane waves, you need to
   explicitly specify the wave_id.
   */
  void RecordPlaneWave(const Signal& signal, const mcl::Point& point);
  
  /**
   This method should only be called in case of a single plane wave
   impinging on the microphone. For multiple plane waves, you need to
   explicitly specify the wave_id.
   */
  void RecordPlaneWave(Source source);
  
  /**
   We need to
   keep track of the wave_id because some microphones (e.g. kemar) need
   to do some filtering on the signal longer than one sample.
   The function Tick tells the microphone to advance in time.
   The microphone objects need to keep track of each plane wave. Hence,
   the first time it sees a new wave_id, it will allocate a new filter
   for it.
   */
  void RecordPlaneWave(const Sample& sample, const mcl::Point& point,
                       const UInt& wave_id);
  
  void RecordPlaneWave(const Signal& signal, const mcl::Point& point,
                       const UInt& wave_id);
  
  /**
   This function tells the microphone to advance by one `tick` in time.
   This is meant to inform the microphone that we are working on a
   new sample in time. Only need to use this when calling wave_ids explicitly.
   */
  virtual void Tick() = 0;
  
  virtual bool IsCoincident() = 0;
  
  /*
   This is true when the microphone can handle frames.
   */
  virtual bool IsFrameEnabled() { return true; }
  
  virtual bool IsOmni() { return false; }
  
  /** This method translates `point` in the reference system of the mic. */
  mcl::Point GetRelativePoint(const mcl::Point& point) const;
  
  /** Resets the state of the microphone (if any). */
  virtual void Reset() {}
  
  static bool Test();
  
  virtual ~Microphone();
private:
  /**
   Cache variables to speed up. Since most of the times two consecutive calls
   to `Filter()` are for the same `mcl::Point` in space, we save the result of the
   geometrical calculations.
   */
  std::map<UInt,mcl::Point> last_point_;
  
  /**
   This point is the most up-to-date point in the reference system of the mic.
   */
  std::map<UInt,mcl::Point> last_relative_point_;
  
  void CalculateRelativePoint(const mcl::Point& point, const UInt& wave_id);
  
  /**
   This is implemented by the specific type of microphones. `mcl::Point` in this
   case is relative to the microphone reference system.
   This is the most important function of this object. This filters the
   sample `sample` as a function of the position from where the sound is
   incoming `point`.
   Info for developer: the directivity should preferably have a maximum in
   the direction of the x-axis For anthropomorphic directivities,
   the facing direction is the x-axis. An arrow going from the jaw trough
   the skull and up should preferrably be in the direction of plus-z-axis.
   Other choices could be made, as long as the conventions are kept at
   higher levels.
   */
  virtual void RecordPlaneWaveRelative(const Sample& sample,
                                       const mcl::Point& point,
                                       const UInt& wave_id);
  
  virtual void RecordPlaneWaveRelative(const Signal& signal,
                                       const mcl::Point& point,
                                       const UInt& wave_id) = 0;
  
protected:
  mcl::Point position_;
  mcl::Quaternion orientation_;
  mcl::Handedness handedness_;
  
  template<typename T>
  friend class MicrophoneArray;
};

class SAL_API StereoMicrophone : public Microphone {
public:
  StereoMicrophone(mcl::Point position, mcl::Quaternion orientation) :
  Microphone(position, orientation) {}
  
  StereoStream* stream() { return &stream_; }
protected:
  StereoStream stream_;
};
  
} // namespace sal

#endif
