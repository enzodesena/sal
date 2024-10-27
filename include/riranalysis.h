/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SDN_EDC_H
#define SDN_EDC_H

#include <vector>

#include "saltypes.h"

namespace sal {

class RirAnalysis {
 public:
  static std::vector<sal::Sample> EnergyDecayCurve(
      std::vector<sal::Sample> room_impulse_response);

  /**
   Returns the reverberation time RT60, i.e. the point where the EDC
   reaches -60 dB in [s]. Returns the length of the rir if the EDC doesn't
   fall below -60 dB.
   */
  static sal::Time Rt60(std::vector<sal::Sample> room_impulse_response,
                        sal::Time sampling_frequency);

  /**
   Returns the reverberation sample RT60, i.e. the point where the EDC
   reaches -60 dB in number of samples. Returns the length of the rir
   if the EDC doesn's fall below -60 dB.
   */
  static sal::Time Rs60(std::vector<sal::Sample> room_impulse_response);

  static sal::Time Rs60FromEdc(std::vector<sal::Sample> edc);

  static sal::Time Rs(std::vector<sal::Sample> room_impulse_response,
                      sal::Sample decay_value);

  static sal::Time RsFromEdc(std::vector<sal::Sample> edc,
                             sal::Sample decay_value);

  static bool Test();
};

}  // namespace sal

#endif
