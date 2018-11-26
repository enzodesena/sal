/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_ISM_H
#define SAL_ISM_H

#include "cuboidroom.hpp"
#include "source.hpp"
#include "delayfilter.hpp"
#include "microphone.hpp"
#include "firfilter.hpp"

namespace sal {
  
enum IsmInterpolation {
  none,
  peterson
};
  
class Ism {
private:
  Room* const room_;
  sal::Source* const source_;
  sal::Microphone* const microphone_;
  
  sal::Int rir_length_;
  IsmInterpolation interpolation_;
  sal::Time sampling_frequency_;
  sal::Length random_distance_;
  
  sal::Time peterson_window_;
  
  mcl::Vector<sal::Sample> rir_;
  mcl::Vector<sal::Time> images_delay_;
  mcl::Vector<mcl::Point> images_position_;
  
  mcl::Vector<sal::DelayFilter> images_int_delay_filter_;
  mcl::Vector<mcl::FirFilter> images_frac_delay_filter_;
  
  bool modified_;
  
  void CalculateRir();
  
  void WriteSample(const sal::Time& delay_norm,
                          const sal::Sample& gid);
public:
  Ism(Room* const room,
      sal::Source* const source,
      sal::Microphone* const microphone,
      IsmInterpolation interpolation,
      sal::Int rir_length,
      const sal::Time sampling_frequency);
  
  void Run(const Sample* input_data, const Int num_samples,
           Buffer& output_buffer);
  
  // Triggers a self-update of the network. Has to be called after microphone,
  // room or source (not including stream push) is updated.
  void Update();
  
  mcl::Vector<sal::Sample> rir() { return rir_; }
  mcl::Vector<sal::Time> images_delay() { return images_delay_; }
  
  void SetPetersonWindow(sal::Time duration) { peterson_window_ = duration; }
  
  void SetRandomDistance(sal::Length distance) { random_distance_ = distance; }
  
  static bool Test();
};
  
} // namespace sal

#endif
