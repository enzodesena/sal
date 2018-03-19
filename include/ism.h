/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SAL_ISM_H
#define SAL_ISM_H

#include "cuboidroom.h"
#include "source.h"
#include "delayfilter.h"
#include "microphone.h"
#include "firfilter.h"

#ifdef SAL_EXPORTS
  #define SAL_API __declspec(dllexport)
#else
  #define SAL_API
#endif

namespace sal {
  
enum IsmInterpolation {
  none,
  peterson
};
  
class SAL_API Ism {
private:
  Room* const room_;
  sal::Source* const source_;
  sal::Microphone* const microphone_;
  
  sal::UInt rir_length_;
  IsmInterpolation interpolation_;
  sal::Time sampling_frequency_;
  sal::Length random_distance_;
  
  sal::Time peterson_window_;
  
  std::vector<sal::Sample> rir_;
  std::vector<sal::Time> images_delay_;
  std::vector<mcl::Point> images_position_;
  
  std::vector<sal::DelayFilter> images_int_delay_filter_;
  std::vector<mcl::FirFilter> images_frac_delay_filter_;
  
  bool modified_;
  
  void CalculateRir();
  
  void WriteSample(const sal::Time& delay_norm,
                          const sal::Sample& gid);
public:
  Ism(Room* const room,
      sal::Source* const source,
      sal::Microphone* const microphone,
      IsmInterpolation interpolation,
      sal::UInt rir_length,
      const sal::Time sampling_frequency);
  
  void Run();
  
  // Triggers a self-update of the network. Has to be called after microphone,
  // room or source (not including stream push) is updated.
  void Update();
  
  std::vector<sal::Sample> rir() { return rir_; }
  std::vector<sal::Time> images_delay() { return images_delay_; }
  
  void set_peterson_window(sal::Time duration) { peterson_window_ = duration; }
  
  void set_random_distance(sal::Length distance) { random_distance_ = distance; }
  
  static bool Test();
};
  
} // namespace sal

#endif
