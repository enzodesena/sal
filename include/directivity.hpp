/*
 omnimicrophones.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "point.hpp"
#include "vectorop.hpp"

namespace sal
{


/**
   This is implemented by the specific type of microphones. `mcl::Point` in this
   case is relative to the microphone reference system.
   This filters the
   sample `sample` as a function of the position from where the sound is
   incoming `point`.
   Info for developer: the directivity should preferably have a maximum in
   the direction of the x-axis For anthropomorphic directivities,
   the facing direction is the x-axis. An arrow going from the jaw trough
   the skull and up should preferrably be in the direction of plus-z-axis.
   Other choices could be made, as long as the conventions are kept at
   higher levels.
   */
template<typename T>
class DirectivityInterface
{
public:
  virtual void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output) noexcept = 0;
  
  virtual void ResetState() noexcept = 0;
};



template<typename T>
class MemorylessMonoDirectivity: DirectivityInterface<T>
{
public:
  void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output) noexcept override
  {
    ASSERT(input.size() <= output.num_samples());
    
    mcl::MultiplyAdd
    (
      input.begin(),
      input.end(),
      GetDirectivity(point),
      output.begin(Channel::kMono),
      output.begin(Channel::kMono));
  }
  
  void ResetState() noexcept override
  {
  }


private:
  virtual T GetDirectivity(
    const Point& point) = 0;
};


template<typename T>
class OmniDirectivity : public MemorylessMonoDirectivity<T>
{
public:
  OmniDirectivity(T gain = T(1.0))
    : gain_(gain)
  {
  }

private:
  T GetDirectivity(
    const Point& /* point */) override
  {
    return gain_;
  }
  
  
  T gain_;
};


/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
template<typename T>
class TrigDirectivity : public MemorylessMonoDirectivity<T>
{
public:
  TrigDirectivity(
    const mcl::Vector<T>& coefficients)
    : coefficients_(coefficients)
  {
  }


private:
  T GetDirectivity(
    const mcl::Point<mcl::Real>& point) override
  {
    Angle phi = AngleBetweenPoints
    (
      point,
      mcl::Point<mcl::Real>(1.0, 0.0, 0.0));

    T directivity = coefficients_[0];
    for (size_t i = 1; i < coefficients_.size(); ++i)
    {
      directivity += coefficients_[i] * pow(cos(phi), i);
    }
    return directivity;
  }

  mcl::Vector<T> coefficients_;
};

template<typename T>
class TanDirectivity : public MemorylessMonoDirectivity<T>
{
public:
  TanDirectivity(Angle base_angle)
    : base_angle_(base_angle)
  {
  }

private:
  T GetDirectivity(
    const mcl::Point<mcl::Real>& point) override
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



template<typename T>
class Directivity
{
public:
  template<typename DirectivityType>
  Directivity(
    DirectivityType x)
    : self_(std::make_unique<DirectivityModel<DirectivityType>>(std::move(x)))
  {
  }


  Directivity(
    const Directivity& x)
    : self_(x.self_->copy_())
  {
  }


  Directivity(
    Directivity&& x) noexcept = default;


  Directivity& operator=(
    const Directivity& x) noexcept
  {
    Directivity tmp(x);
    *this = std::move(tmp); // Using move assignment operator
    return *this;
  }


  /** Move assignment operator */
  Directivity& operator=(
    Directivity&& x) noexcept = default;


  void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output) noexcept
  {
    self_->ReceiveAdd_(input, point, output);
  }


  void ResetState() noexcept
  {
    self_->Reset_();
  }
  
private:
  
  struct DirectivityConcept
  {
    virtual ~DirectivityConcept() = default;
    virtual void ReceiveAdd_(
      const mcl::Vector<T>& input,
      const Point& point,
      Buffer<T>& output) = 0;
    virtual void Reset_() = 0;
    virtual std::unique_ptr<DirectivityConcept> copy_() = 0;
  };


  template<typename DirectivityType>
  struct DirectivityModel final : DirectivityConcept
  {
    DirectivityType data_;

    DirectivityModel(
      DirectivityType x)
      : data_(std::move(x))
    {
    }


    std::unique_ptr<DirectivityConcept> copy_() override
    {
      return std::make_unique<DirectivityModel>(*this);
    }


    void ReceiveAdd_(
      const mcl::Vector<T>& input,
      const Point& point,
      Buffer<T>& output) noexcept override
    {
      data_.ReceiveAdd(input, point, output);
    }


    void Reset_() noexcept override
    {
      data_.ResetState();
    }
  };
  
  std::unique_ptr<DirectivityConcept> self_; // Concept is filterable object
};


} // namespace sal

