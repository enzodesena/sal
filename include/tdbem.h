/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SAL_TDBEM_H
#define SAL_TDBEM_H


#include "cuboidroom.h"
#include "source.h"
#include "microphone.h"
#include "delayfilter.h"

#ifdef SAL_EXPORTS
  #define SAL_API __declspec(dllexport)
#else
  #define SAL_API
#endif

namespace sal {
  
class SAL_API TdBem {
private:
  Room* const room_;
  sal::Source* const source_;
  sal::Microphone* const microphone_;
  
  sal::Time sampling_frequency_;
  sal::Length spatial_sampling_period_;
  sal::Sample specific_acoustic_impedance_;
  
  std::vector<sal::Sample> rir_;
  
  
  std::vector<mcl::Point> points_; // Stores points on the bounduary
  sal::UInt num_elements_;
  std::vector<mcl::Point> normal_vectors_;
  std::vector<std::vector<sal::Length> > distances_;
  std::vector<std::vector<sal::Sample> > weights_current_;
  std::vector<std::vector<sal::Sample> > weights_previous_;
  
  std::vector<sal::Length> distances_mic_;
  std::vector<sal::Sample> weights_mic_current_;
  std::vector<sal::Sample> weights_mic_previous_;
  std::vector<sal::Sample> weights_source_;
  
  std::vector<sal::Length> distances_source_;
  sal::Length distance_los_;
  
  std::vector<sal::DelayFilter> pressures_;
  
  sal::DelayFilter source_delay_line_;
  
  bool log_;
  std::string log_file_name_;
  
  void CalculatePoints();
  
  static sal::Sample
  CalculateWeightCurrent(sal::Sample dr_dn,
                         sal::Length distance,
                         sal::Length sound_speed,
                         sal::Time sampling_frequency,
                         sal::Length spatial_sampling_period,
                         sal::Sample specific_acoustic_impedance);
  
  
  static sal::Sample
  CalculateWeightPrevious(sal::Sample dr_dn,
                          sal::Length distance,
                          sal::Length sound_speed,
                          sal::Time sampling_frequency,
                          sal::Length spatial_sampling_period,
                          sal::Sample specific_acoustic_impedance);
  
  static sal::Sample CalculateDrDn(mcl::Point point_x,
                                   mcl::Point point_y,
                                   mcl::Point normal_y);
  
public:
  TdBem(Room* const room,
        sal::Source* const source,
        sal::Microphone* const microphone,
        const sal::Time sampling_frequency,
        const sal::Length spatial_sampling_period,
        const sal::Sample specific_acoustic_impedance);
  
  
  void WriteOutPoints(const std::string file_name);
  
  void Run();
  
  sal::Signal rir() const { return rir_; }
  
  void Log(std::string file_name) {
    log_ = true;
    log_file_name_ = file_name;
  }
  
  static bool Test();
};
  
} // namespace sal


#endif
