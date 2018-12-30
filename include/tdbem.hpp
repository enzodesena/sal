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
template<typename T>
class TdBem
{
private:
  Point source_position_;
  Point receiver_position_;
  
  Time sampling_frequency_;
  Length spatial_sampling_period_;
  T specific_acoustic_impedance_;

  mcl::Vector<T> rir_;

  mcl::Vector<Point> points_; // Stores points on the bounduary
  Int num_elements_;
  mcl::Vector<Point> normal_vectors_;
  std::vector<mcl::Vector<Length>> distances_;
  std::vector<mcl::Vector<T>> weights_current_;
  std::vector<mcl::Vector<T>> weights_previous_;

  mcl::Vector<Length> distances_mic_;
  mcl::Vector<T> weights_mic_current_;
  mcl::Vector<T> weights_mic_previous_;
  mcl::Vector<T> weights_source_;

  mcl::Vector<Length> distances_source_;
  Length distance_los_;

  mcl::Vector<DelayFilter<T>> pressures_;

  DelayFilter<T> source_delay_line_;

  bool log_;
  std::string log_file_name_;

  void CalculatePoints();

  static T
  CalculateWeightCurrent(
    T dr_dn,
    Length distance,
    Length sound_speed,
    Time sampling_frequency,
    Length spatial_sampling_period,
    T specific_acoustic_impedance);

  static T
  CalculateWeightPrevious(
    T dr_dn,
    Length distance,
    Length sound_speed,
    Time sampling_frequency,
    Length spatial_sampling_period,
    T specific_acoustic_impedance);

  static T CalculateDrDn(
    Point point_x,
    Point point_y,
    Point normal_y);

public:
  TdBem(
    const CuboidRoom<T>& room,
    const Source& source,
    const Receiver<T>& receiver,
    const Time sampling_frequency,
    const Length spatial_sampling_period,
    const T specific_acoustic_impedance);

  void WriteOutPoints(
    std::string file_name);

  void Run(
    const mcl::Vector<T>& input_buffer,
    Receiver<T>& receiver,
    Buffer<T>& output_buffer);


  Signal<T> rir() const
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

#include "tdbem.ipp"
