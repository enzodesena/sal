/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "cuboidroom.hpp"
#include "source.hpp"
#include "receiver.hpp"
#include "delayfilter.hpp"

namespace sal
{
class TdBem
{
private:
  Room* const room_;
  Source* const source_;
  Microphone* const microphone_;

  Time sampling_frequency_;
  Length spatial_sampling_period_;
  Sample specific_acoustic_impedance_;

  mcl::Vector<Sample> rir_;

  mcl::Vector<mcl::Point> points_; // Stores points on the bounduary
  Int num_elements_;
  mcl::Vector<mcl::Point> normal_vectors_;
  std::vector<mcl::Vector<Length>> distances_;
  std::vector<mcl::Vector<Sample>> weights_current_;
  std::vector<mcl::Vector<Sample>> weights_previous_;

  mcl::Vector<Length> distances_mic_;
  mcl::Vector<Sample> weights_mic_current_;
  mcl::Vector<Sample> weights_mic_previous_;
  mcl::Vector<Sample> weights_source_;

  mcl::Vector<Length> distances_source_;
  Length distance_los_;

  mcl::Vector<DelayFilter> pressures_;

  DelayFilter source_delay_line_;

  bool log_;
  std::string log_file_name_;

  void CalculatePoints();

  static Sample
  CalculateWeightCurrent(
    Sample dr_dn,
    Length distance,
    Length sound_speed,
    Time sampling_frequency,
    Length spatial_sampling_period,
    Sample specific_acoustic_impedance);

  static Sample
  CalculateWeightPrevious(
    Sample dr_dn,
    Length distance,
    Length sound_speed,
    Time sampling_frequency,
    Length spatial_sampling_period,
    Sample specific_acoustic_impedance);

  static Sample CalculateDrDn(
    mcl::Point point_x,
    mcl::Point point_y,
    mcl::Point normal_y);

public:
  TdBem(
    Room* room,
    Source* source,
    Microphone* microphone,
    Time sampling_frequency,
    Length spatial_sampling_period,
    Sample specific_acoustic_impedance);

  void WriteOutPoints(
    std::string file_name);

  void Run(
    const MonoBuffer& input_buffer,
    Buffer& output_buffer);


  Signal rir() const
  {
    return rir_;
  }


  void Log(
    std::string file_name)
  {
    log_ = true;
    log_file_name_ = file_name;
  }


  static bool SimulationTime();
};
} // namespace sal

