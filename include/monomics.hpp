/*
 omnimicrophones.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_DIRECTIVITYFUNCTION_H
#define SAL_DIRECTIVITYFUNCTION_H

#include "point.hpp"
#include "salconstants.hpp"
#include "microphone.hpp"
#include "vectorop.hpp"

namespace sal
{
class MonoMic : virtual public Microphone
{
public:
  MonoMic(
    mcl::Point position,
    mcl::Quaternion orientation)
    : Microphone(position, orientation)
  {
  }


  /** This method is used mostly for testing, as it is very slow.
   You should use AddPlaneWave instead. */
  Sample RecordPlaneWave(
    const Sample input_sample,
    const mcl::Point& point,
    const Int& wave_id = 0) noexcept
  {
    MonoBuffer output_buffer(1);
    AddPlaneWave(&input_sample, 1, point, wave_id, output_buffer);
    return output_buffer.GetSample(0);
  }


  bool IsCoincident() const noexcept override
  {
    return true;
  }


  Int num_channels() const noexcept override
  {
    return 1;
  }


  virtual ~MonoMic()
  {
  }
};


class MemorylessMic : virtual public Microphone
{
public:
  MemorylessMic(
    mcl::Point position,
    mcl::Quaternion orientation)
    : Microphone(position, orientation)
  {
  }


  virtual ~MemorylessMic()
  {
  }


private:
  virtual Sample GetDirectivity(
    const mcl::Point& point) = 0;
};


class MemorylessMonoMic : public MemorylessMic, public MonoMic
{
public:
  MemorylessMonoMic(
    mcl::Point position,
    mcl::Quaternion orientation)
    : Microphone(position, orientation)
    , MemorylessMic(position, orientation)
    , MonoMic(position, orientation)
  {
  }


  virtual ~MemorylessMonoMic()
  {
  }


  virtual void AddPlaneWaveRelative(
    const Sample* input_data,
    const Int num_samples,
    const mcl::Point& point,
    const Int /* wave_id */,
    Buffer& output_buffer) noexcept
  {
    ASSERT(output_buffer.num_channels() >= 1);
    ASSERT(num_samples <= output_buffer.num_samples());

    mcl::MultiplyAdd
    (
      input_data,
      GetDirectivity(point),
      output_buffer.GetReadPointer(Buffer::Channel::kMono),
      num_samples,
      output_buffer.GetWritePointer(Buffer::Channel::kMono));
  }


private:

  Sample GetDirectivity(
    const mcl::Point& point) override = 0;
};


class GainMic : public MemorylessMonoMic
{
public:
  GainMic(
    mcl::Point position,
    Sample gain)
    : Microphone(position, mcl::Quaternion::Identity())
    , MemorylessMonoMic(position, mcl::Quaternion::Identity())
    , gain_(gain)
  {
  }


  bool IsOmni() const noexcept override
  {
    return true;
  }


  virtual ~GainMic()
  {
  }


private:
  Sample GetDirectivity(
    const mcl::Point& /* point */) override
  {
    return gain_;
  }


  Sample gain_;
};


class OmniMic : public GainMic
{
public:
  OmniMic(
    mcl::Point position)
    : Microphone(position, mcl::Quaternion::Identity())
    , GainMic(position, (Sample)1.0)
  {
  }


  virtual ~OmniMic()
  {
  }
};


/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TrigMic : public MemorylessMonoMic
{
public:
  TrigMic(
    mcl::Point position,
    mcl::Quaternion orientation,
    mcl::Vector<Sample> coefficients)
    : Microphone(position, orientation)
    , MemorylessMonoMic(position, orientation)
    , coefficients_(coefficients)
  {
  }


  virtual ~TrigMic()
  {
  }


private:
  Sample GetDirectivity(
    const mcl::Point& point) override
  {
    Angle phi = AngleBetweenPoints
    (
      point,
      mcl::Point(1.0, 0.0, 0.0));

    const Int N = coefficients_.size();
    Sample directivity(coefficients_[0]);
    for (Int i = 1; i < N; ++i)
    {
      directivity += coefficients_[i] * pow(cos(phi), i);
    }
    return directivity;
  }


  mcl::Vector<Sample> coefficients_;
};


/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TanMic : public MemorylessMonoMic
{
public:
  TanMic(
    mcl::Point position,
    mcl::Quaternion orientation,
    Sample base_angle)
    : Microphone(position, orientation)
    , MemorylessMonoMic(position, orientation)
    , base_angle_(base_angle)
  {
  }


  virtual ~TanMic()
  {
  }


private:
  Sample GetDirectivity(
    const mcl::Point& point) override
  {
    Angle phi = AngleBetweenPoints(point, mcl::Point(1.0, 0.0, 0.0));

    Angle phi_l = 0;
    Angle phi_lp1 = base_angle_; //PI/3.0;

    Sample directivity;
    if (phi < phi_lp1)
    {
      directivity =
        1.0 / sqrt(
          1 + pow(sin(phi - phi_l), 2.0) / pow(sin(phi_lp1 - phi), 2.0));
    }
    else
    {
      directivity = 0.0;
    }

    return directivity;
  }


  Sample base_angle_;
};
} // namespace sal

#endif
