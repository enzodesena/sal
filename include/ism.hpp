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


class Ism
{
private:
  Room* const room_;
  Source* const source_;
  Microphone* const microphone_;

  Int rir_length_;
  IsmInterpolation interpolation_;
  Time sampling_frequency_;
  Length random_distance_;

  Time peterson_window_;

  mcl::Vector<Sample> rir_;
  mcl::Vector<Time> images_delay_;
  mcl::Vector<mcl::Point> images_position_;

  mcl::Vector<DelayFilter> images_int_delay_filter_;
  mcl::Vector<mcl::DigitalFilter> images_frac_delay_filter_;

  bool modified_;

  void CalculateRir();

  void WriteSample(
    const Time& delay_norm,
    const Sample& gid);
public:
  Ism(
    Room* room,
    Source* source,
    Microphone* microphone,
    IsmInterpolation interpolation,
    Int rir_length,
    Time sampling_frequency);

  void Run(
    const Sample* input_data,
    Int num_samples,
    Buffer& output_buffer);

  // Triggers a self-update of the network. Has to be called after microphone,
  // room or source (not including stream push) is updated.
  void Update();


  mcl::Vector<Sample> rir()
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
};



} // namespace sal

#include "ism_impl.hpp"
