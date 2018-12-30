/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "cuboidroom.hpp"
#include "source.hpp"
#include "delayfilter.hpp"
#include "receiver.hpp"
#include "digitalfilter.hpp"

namespace sal
{
enum IsmInterpolation
{
  none,
  peterson
};

template<typename T>
class Ism
{
public:
  Ism(
    const CuboidRoom<T>& room,
    const Source& source,
    const Receiver<T>& receiver,
    const IsmInterpolation interpolation,
    const size_t rir_length,
    const Time sampling_frequency);

  void Run(
    const mcl::Vector<T> input,
    Receiver<T>& receiver,
    Buffer<T>& output_buffer);

  // Triggers a self-update of the network. Has to be called after microphone,
  // room or source (not including stream push) is updated.
  void Update(
    const CuboidRoom<T>& room,
    const Source& source,
    const Receiver<T>& receiver);


  mcl::Vector<T> rir()
  {
    return rir_;
  }


  mcl::Vector<Time> images_delay()
  {
    return images_delay_;
  }


  void SetPetersonWindow(
    Time duration)
  {
    peterson_window_ = duration;
  }


  void SetRandomDistance(
    Length distance)
  {
    random_distance_ = distance;
  }

  static bool Test();
  
private:
  void CalculateRir(
    const CuboidRoom<T>& room,
    const Source& source,
    const Receiver<T>& receiver);


  void WriteSample(
    const Time delay_norm,
    const T gid);
  
  // Member variables
  IsmInterpolation interpolation_;
  Time sampling_frequency_;
  Length random_distance_;

  Time peterson_window_;

  mcl::Vector<T> rir_;
  mcl::Vector<Time> images_delay_;
  mcl::Vector<Point> images_position_;

  mcl::Vector<DelayFilter<T>> images_int_delay_filter_;
  mcl::Vector<mcl::DigitalFilter<T>> images_frac_delay_filter_;

};



} // namespace sal

#include "ism.ipp"
