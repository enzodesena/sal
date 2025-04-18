/*
 omnimicrophones.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#ifndef SAL_DIRECTIVITYFUNCTION_H
#define SAL_DIRECTIVITYFUNCTION_H

#include <span>
#include <type_traits>

#include "microphone.h"
#include "point.h"
#include "salconstants.h"
#include "vectorop.h"

namespace sal {

class MonoMic : virtual public Microphone {
 public:
  MonoMic(dsp::Point position, dsp::Quaternion orientation, const dsp::Handedness handedness = dsp::Handedness::kRightHanded)
      : Microphone(position, orientation, handedness) {}

  /** This method is used mostly for testing, as it is very slow.
   You should use AddPlaneWave instead. */
  Sample RecordPlaneWave(const Sample input_sample, const dsp::Point& point,
                         const Int& wave_id = 0) noexcept {
    MonoBuffer output_buffer(1);
    Microphone::AddPlaneWave(std::span<const Sample>(&input_sample, 1), point,
                             wave_id, output_buffer);
    return output_buffer.GetSample(0);
  }

  void AddPlaneWave(std::span<const Sample> input_data, const dsp::Point& point,
                    std::span<Sample> output_data) noexcept {
    this->AddPlaneWaveRelative(input_data, GetRelativePoint(point), 0, output_data);
  }
  
  void AddPlaneWave(std::span<const Sample> input_data, const dsp::Point& point,
                    const size_t wave_id,
                    std::span<Sample> output_data) noexcept {
    this->AddPlaneWaveRelative(input_data, GetRelativePoint(point), wave_id,
                               output_data);
  }

  // This needs to be implemented if you want to be able to use this as part of
  // a microphone array
  virtual void AddPlaneWaveRelative(std::span<const Sample> input_data,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    std::span<Sample> output_data) noexcept = 0;

  // This is provided so that you only need to implement the method above (i.e.
  // the one with an output span)
  void AddPlaneWaveRelative(std::span<const Sample> input_data,
                            const dsp::Point& point, const size_t wave_id,
                            Buffer& output_buffer) noexcept override {
    static_assert(std::is_base_of<Buffer, MonoBuffer>::value);
    AddPlaneWaveRelative(input_data, point, wave_id,
                         output_buffer.GetWriteView(Buffer::kMonoChannel));
  }

  bool IsCoincident() const noexcept override { return true; }

  Int num_channels() const noexcept override { return 1; }
};

class MemorylessMic : virtual public Microphone {
 public:
  MemorylessMic(dsp::Point position, dsp::Quaternion orientation, const dsp::Handedness handedness = dsp::Handedness::kRightHanded)
      : Microphone(position, orientation, handedness) {}

 private:
  virtual Sample GetDirectivity(const dsp::Point& point) = 0;
};

class MemorylessMonoMic : public MemorylessMic, public MonoMic {
 public:
  MemorylessMonoMic(const dsp::Point& position,
                    const dsp::Quaternion& orientation,
                    const dsp::Handedness handedness = dsp::Handedness::kRightHanded)
      : Microphone(position, orientation, handedness),
        MemorylessMic(position, orientation, handedness),
        MonoMic(position, orientation, handedness) {}

  virtual ~MemorylessMonoMic() {}

  void AddPlaneWaveRelative(std::span<const Sample> input_data,
                            const dsp::Point& point, const size_t /* wave_id */,
                            std::span<Sample> output_data) noexcept override {
    const size_t num_samples = input_data.size();
    ASSERT(output_data.size() >= num_samples);

    dsp::MultiplyAdd(input_data, GetDirectivity(point), output_data,
                     output_data);
  }

 private:
  virtual Sample GetDirectivity(const dsp::Point& point) override = 0;
};

class GainMic : public MemorylessMonoMic {
 public:
  GainMic(Sample gain, const dsp::Point& position)
      : Microphone(position, dsp::Quaternion::Identity()),
        MemorylessMonoMic(position, dsp::Quaternion::Identity()),
        gain_(gain) {}

  virtual bool IsOmni() const noexcept { return true; }

 private:
  virtual Sample GetDirectivity(const dsp::Point& /* point */) { return gain_; }

  Sample gain_;
};

class OmniMic : public GainMic {
 public:
  OmniMic(const dsp::Point& position)
      : Microphone(position, dsp::Quaternion::Identity()),
        GainMic((Sample)1.0, position) {}
};

/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TrigMic : public MemorylessMonoMic {
 public:
  TrigMic(const std::vector<Sample>& coefficients, const dsp::Point& position, const dsp::Quaternion& orientation,
          dsp::Handedness handedness = dsp::Handedness::kRightHanded)
      : Microphone(position, orientation, handedness),
        MemorylessMonoMic(position, orientation, handedness),
        coefficients_(coefficients) {}

 private:
  virtual Sample GetDirectivity(const dsp::Point& point) {
    Angle phi = AngleBetweenPoints(point, dsp::Point(1.0, 0.0, 0.0));
    return dsp::EvaluateTrigPolynomial(phi, coefficients_);
  }
  
  std::vector<Sample> coefficients_;
};

/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TanMic : public MemorylessMonoMic {
 public:
  TanMic(dsp::Point position, dsp::Quaternion orientation,
         sal::Sample base_angle)
      : Microphone(position, orientation),
        MemorylessMonoMic(position, orientation),
        base_angle_(base_angle) {}

 private:
  virtual Sample GetDirectivity(const dsp::Point& point) {
    Angle phi = AngleBetweenPoints(point, dsp::Point(1.0, 0.0, 0.0));

    sal::Angle phi_l = 0;
    sal::Angle phi_lp1 = base_angle_;  // PI/3.0;

    sal::Sample directivity;
    if (phi < phi_lp1) {
      directivity = 1.0 / sqrt(1 + pow(sin(phi - phi_l), 2.0) /
                                       pow(sin(phi_lp1 - phi), 2.0));
    } else {
      directivity = 0.0;
    }

    return directivity;
  }

  sal::Sample base_angle_;
};

}  // namespace sal

#endif
