/*
 source.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_SOURCE_H
#define SAL_SOURCE_H

#include <vector>

#include "point.h"
#include "salconstants.h"
#include "saltypes.h"
#include "salutilities.h"

namespace sal {

class Source {
 public:
  Source(const dsp::Point&,
         const dsp::Quaternion& orientation = dsp::Quaternion::Identity(),
         const dsp::Handedness handedness = dsp::Handedness::kRightHanded) noexcept;

  dsp::Point position() const noexcept;

  void SetPosition(const dsp::Point& position) noexcept;
  
  /** Returns current orientation of the microphone */
  dsp::Quaternion orientation() const noexcept;

  /** Set microphone orientation */
  void SetOrientation(const dsp::Quaternion& orientation) noexcept;

  void SetHandedness(const dsp::Handedness handedness) noexcept;
  
  /**
   We need to keep track of the `wave_id` because some sources need
   to do some filtering on the signal longer than one sample.
   The source objects need to keep track of each plane wave. Hence,
   the first time it sees a new `wave_id`, it will allocate a new filter
   for it. This method does not consider 1/r attenuation.
   */
  void ProcessAudio(std::span<const Sample> input_data,
                    const dsp::Point& observation_point,
                    const size_t wave_id,
                    std::span<Sample> output_data) noexcept {
    ProcessAudioRelative(input_data,
                         GetRelativePoint(observation_point),
                         wave_id,
                         output_data);
  }
  
  virtual void ProcessAudioRelative(std::span<const Sample> input_data,
                                    const dsp::Point& observation_point_relative_to_source_look_direction,
                                    const size_t wave_id,
                                    std::span<Sample> output_data) noexcept = 0;
  
  static bool Test();
 private:
  dsp::Point GetRelativePoint(const dsp::Point& point) const noexcept;
  
  Triplet position_;
  dsp::Quaternion orientation_;
  dsp::Handedness handedness_;
};



class OmniSource : public Source {
public:
  using Source::Source;
  
private:
  void ProcessAudioRelative(std::span<const Sample> input_data,
                                    const dsp::Point& /* observation_point_relative_to_source_look_direction */,
                                    const size_t /* wave_id */,
                                    std::span<Sample> output_data) noexcept {
    ASSERT(output_data.size() >= input_data.size()); // As usual, it is the input vector to dictate the length
    std::copy(input_data.begin(), input_data.end(), output_data.begin());
  }
};



class MemorylessSource: public Source {
public:
  using Source::Source;
  
private:
  void ProcessAudioRelative(std::span<const Sample> input_data,
                                    const dsp::Point& observation_point_relative_to_source_look_direction,
                                    const size_t /* wave_id */,
                                    std::span<Sample> output_data) noexcept override {
    ASSERT(output_data.size() >= input_data.size());
    dsp::Multiply(input_data, GetDirectivity(observation_point_relative_to_source_look_direction), output_data);
  }
  
 virtual Sample GetDirectivity(const dsp::Point& point) noexcept = 0;
};

class TrigSource : public MemorylessSource {
public:
  TrigSource(const std::vector<Sample>& coefficients,
             const dsp::Point& position,
             const dsp::Quaternion& orientation,
             dsp::Handedness handedness = dsp::Handedness::kRightHanded)
      : MemorylessSource(position, orientation, handedness),
        coefficients_(coefficients) {}
private:
  Sample GetDirectivity(const dsp::Point& point) noexcept override {
    Angle phi = AngleBetweenPoints(point, dsp::Point(1.0, 0.0, 0.0));
    return dsp::EvaluateTrigPolynomial(phi, coefficients_);
  }
  
  std::vector<Sample> coefficients_;
};

}  // namespace sal

#endif
