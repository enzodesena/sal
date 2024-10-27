/*
 microphone.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_MICROPHONE_H
#define SAL_MICROPHONE_H

#include <assert.h>

#include <map>

#include "audiobuffer.h"
#include "point.h"
#include "quaternion.h"
#include "salconstants.h"
#include "source.h"

namespace sal {

class Microphone {
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
   dsp::AxAng2Quat(0,0,1,angle) with angle in radians.

   Methods with wave_id as a parameter imply that the user should
   explicitly tell the mic to `Tick`, i.e. to inform it that we are working
   on a new sample. Methods without wave_id (i.e. assuming there
   is a single plane wave incoming) do this automatically.
   */
  Microphone(dsp::Point position,
             dsp::Quaternion orientation = dsp::Quaternion::Identity());

  /** Returns current position of the microphone */
  dsp::Point position() const noexcept;

  /** Set microphone position */
  virtual void SetPosition(const dsp::Point& position) noexcept;

  /** Returns current orientation of the microphone */
  dsp::Quaternion orientation() const noexcept;

  /** Set microphone orientation */
  virtual void SetOrientation(const dsp::Quaternion& orientation) noexcept;

  /** Set handedness of reference system */
  void SetHandedness(const dsp::Handedness handedness) noexcept;

  /**
   We do not implement directly the case of a single plane wave because in
   most situations the microphone is recording many plane waves at the same
   time. This method should only be called in case of a single plane wave
   impinging on the microphone. For multiple plane waves, you need to
   explicitly specify the wave_id.
   */
  void AddPlaneWave(const MonoBuffer& signal, const dsp::Point& point,
                    Buffer& output_buffer) noexcept;

  void AddPlaneWave(std::span<const Sample> input_data, const dsp::Point& point,
                    Buffer& output_buffer) noexcept;

  void AddPlaneWave(const Sample input_sample, const dsp::Point& point,
                    Buffer& output_buffer) noexcept;

  /**
   We need to
   keep track of the wave_id because some microphones (e.g. kemar) need
   to do some filtering on the signal longer than one sample.
   The function Tick tells the microphone to advance in time.
   The microphone objects need to keep track of each plane wave. Hence,
   the first time it sees a new wave_id, it will allocate a new filter
   for it.
   */
  void AddPlaneWave(const MonoBuffer& input_buffer, const dsp::Point& point,
                    const size_t wave_id, Buffer& output_buffer) noexcept;

  void AddPlaneWave(const Sample input_sample, const dsp::Point& point,
                    const size_t wave_id, Buffer& output_buffer) noexcept;

  virtual void AddPlaneWave(std::span<const Sample> input_data,
                            const dsp::Point& point, const size_t wave_id,
                            Buffer& output_buffer) noexcept;

  virtual bool IsCoincident() const noexcept = 0;

  virtual Int num_channels() const noexcept = 0;

  virtual bool IsOmni() const noexcept { return false; }

  /** This method translates `point` in the reference system of the mic. */
  dsp::Point GetRelativePoint(const dsp::Point& point) const noexcept;

  /** Resets the state of the microphone (if any). */
  virtual void Reset() noexcept {}

  static bool Test();

  virtual void AddPlaneWaveRelative(const MonoBuffer& signal,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    Buffer& output_buffer) noexcept;

  /**
   This is implemented by the specific type of microphones. `dsp::Point` in this
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
  virtual void AddPlaneWaveRelative(std::span<const Sample> input_data,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    Buffer& output_buffer) noexcept = 0;

  virtual ~Microphone() {}

 private:
  dsp::Triplet position_;
  dsp::Quaternion orientation_;

  friend class MicrophoneArray;

 protected:
  dsp::Handedness handedness_;
};

class StereoMicrophone : public Microphone {
 public:
  StereoMicrophone(dsp::Point position, dsp::Quaternion orientation)
      : Microphone(position, orientation) {}
};

/** This microphone copies whatever is coming in to respective channels. */
class BypassMic final : public sal::Microphone {
 public:
  /**
   Constructs a bypass microphone object.

   @param[in] position The position in space of the microphone.
   */
  BypassMic(dsp::Point position, sal::Int num_channels) noexcept;

  /** Returns whether or not the microphone is a coincident microphone. */
  virtual bool IsCoincident() const noexcept { return true; }

  virtual sal::Int num_channels() const noexcept { return num_channels_; }

  virtual void AddPlaneWaveRelative(std::span<const Sample> input_data,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    sal::Buffer& output_buffer) noexcept;

 private:
  sal::Int num_channels_;
};

}  // namespace sal

#endif
