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
#include "monomics.h"
#include "point.h"
#include "salconstants.h"
#include "saltypes.h"
#include "source.h"

namespace sal {

/** This is a microhone array, containing pointers to other microphones. A call
 to AddPlaneWave will call the AddPlaneWave of each microphone.
 You can then access the streams of each microphone by first extracting
 pointers to the microphone with the microphones() method and then accessing
 each one's stream. Alternatively, the object also has a multichannel stream
 object, which contains pointers to the individual streams. */
class MicrophoneArray : public Microphone {
 public:
  MicrophoneArray(const dsp::Point& position,
                  const dsp::Quaternion& orientation,
                  std::vector<std::unique_ptr<MonoMic>>&& microphones)
      : Microphone(position, orientation),
        microphones_(std::move(microphones)) {}

  /**
   This method will move all the internal microphones to a new position.
   The relative positions of the different microphones will stay unchanged.
   */
  virtual void SetPosition(const dsp::Point& position) noexcept {
    dsp::Point position_delta(position.x() - this->position().x(),
                              position.y() - this->position().y(),
                              position.z() - this->position().z());
    for (size_t i = 0; i < microphones_.size(); ++i) {
      dsp::Point old_mic_position = microphones_[i]->position();
      dsp::Point new_mic_position(old_mic_position.x() + position_delta.x(),
                                  old_mic_position.y() + position_delta.y(),
                                  old_mic_position.z() + position_delta.z());
      microphones_[i]->SetPosition(new_mic_position);
    }

    Microphone::SetPosition(position);
  }

  void SetOrientation(const dsp::Quaternion& orientation) noexcept {
    for (Int i = 0; i < (Int)microphones_.size(); ++i) {
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

    if (num_microphones == 0 || num_microphones == 1) {
      return true;
    }

    dsp::Point position(microphones_[0]->position());
    for (Int i = 1; i < num_microphones; ++i) {
      if (!IsEqual(microphones_[i]->position(), position)) {
        return false;
      }
    }
    return true;
  }

  Int num_channels() const noexcept { return microphones_.size(); }

  const MonoMic& GetConstMicrophoneRef(
      const size_t microphone_id) const noexcept {
    return *microphones_[microphone_id];
  }

  MonoMic& GetMicrophoneRef(const size_t microphone_id) noexcept {
    return *microphones_[microphone_id];
  }

  Int num_microphones() const noexcept { return microphones_.size(); }

  static bool Test();

  /**
   Simulates the output of the microphone array to a source in the direction
   of source.position() and with input signal `source.signal()`.
   This does not include attenuation nor delay due to propagation.
   If you wish to use this method, the buffer has to be a multichannel buffer,
   and all the underlying microphones need to be monophonic. If that is not
   the case, then you need to use the AddPlaneWave of the underlying microphone
   objects directly.
   */
  virtual void AddPlaneWave(std::span<const Sample> input_data,
                            const dsp::Point& point, const size_t wave_id,
                            Buffer& output_buffer) noexcept {
    for (size_t i = 0; i < microphones_.size(); ++i) {
      // Each microphone will push in its own mono stream. The multichannel
      // stream is merely a vector of pointers to the individual mono streams

      std::span<Sample> output_data = output_buffer.GetWriteView(i);

      microphones_[i]->AddPlaneWave(input_data, point, wave_id, output_data);
    }
  }

  virtual void AddPlaneWaveRelative(std::span<const Sample> input_data,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    Buffer& output_buffer) noexcept {
    // This method can't be called directly (should call AddPlaneWave instead.
    // That's because the relative point needs to be calculated with respect
    // to each microphone individually. If you could call
    // AddPlaneWaveRelative directly, on the other hand, it would carry out
    // the same rotation for all of them.
    ASSERT(false);
  }

 protected:
  std::vector<std::unique_ptr<MonoMic>> microphones_;
};

/** This object creates a microphone array based on a microphone prototype.
 So, for instance, you can take an omnimic, and create an array of N omni mics.
 This class is a parent for more specific classes, e.g. a circular array etc.
 This class simply creates N copies of the mic_prototype and repositions them
 in the set `position`, with given `orientation` and `handedness`. */
template <class T>
class UniformArray : public MicrophoneArray {
 public:
  UniformArray(const dsp::Point& position, const dsp::Quaternion& orientation,
               const T& mic_prototype, const dsp::Int num_microphones)
      // Directly initialize the base class in the initializer list
      : MicrophoneArray(position, orientation,
                        MicrophoneFactory(mic_prototype, num_microphones)) {}

 private:
  std::vector<std::unique_ptr<MonoMic>> MicrophoneFactory(
      const T& mic_prototype, const size_t num_microphones) {
    std::vector<std::unique_ptr<MonoMic>> output;
    output.reserve(num_microphones);
    for (size_t i = 0; i < num_microphones; ++i) {
      output.emplace_back(std::make_unique<T>(mic_prototype));
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
template <class T>
class CircularArray : public UniformArray<T> {
 public:
  CircularArray(const dsp::Point& position, const dsp::Quaternion& orientation,
                const T& mic_prototype, const Length radius,
                const std::vector<Angle>& angles)
      : UniformArray<T>(position, orientation, mic_prototype, angles.size()),
        radius_(radius),
        angles_(angles) {
    SetOrientation(orientation);
  }

  virtual void SetOrientation(const dsp::Quaternion& orientation) noexcept {
    dsp::Point position(this->position());
    std::vector<dsp::Point> positions =
        GetPositions(position, radius_, angles_);

    for (dsp::Int i = 0; i < (Int)angles_.size(); ++i) {
      Angle mic_angle = (this->handedness_ == dsp::Handedness::kRightHanded)
                            ? angles_[i]
                            : -angles_[i];
      dsp::Point relative_position =
          dsp::QuatRotate(orientation, dsp::Subtract(positions[i], position),
                          this->handedness_);
      this->microphones_[i]->SetPosition(dsp::Sum(relative_position, position));

      dsp::Quaternion q = dsp::QuatMultiply(
          orientation, dsp::AxAng2Quat(0.0, 0.0, 1.0, mic_angle));
      this->microphones_[i]->SetOrientation(q);
    }
  }

 private:
  Length radius_;
  std::vector<Angle> angles_;

  static std::vector<dsp::Point> GetPositions(
      const dsp::Point& position, const Length radius,
      const std::vector<Angle>& angles) {
    std::vector<dsp::Point> positions(angles.size());
    for (Int i = 0; i < (Int)angles.size(); ++i) {
      positions[i] =
          dsp::Point(radius * cos(angles[i]) + position.x(),
                     radius * sin(angles[i]) + position.y(), position.z());
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
template <class T>
class StereoMic : public CircularArray<T> {
 public:
  StereoMic(const dsp::Point& position, const dsp::Quaternion& orientation,
            const T& mic_prototype, const Length radius, const Angle base_angle,
            const Angle center_angle)
      : CircularArray<T>(position, orientation, mic_prototype, radius,
                         StereoAngles(base_angle, center_angle)) {}

 private:
  static std::vector<Angle> StereoAngles(const Angle base_angle,
                                         const Angle center_angle) {
    std::vector<Angle> angles(2);
    angles[0] = center_angle - base_angle / 2.0;
    angles[1] = center_angle + base_angle / 2.0;
    return angles;
  }
};

bool MicrophoneArrayTest();

}  // namespace sal

#endif
