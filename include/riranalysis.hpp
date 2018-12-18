/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "vector.hpp"
#include "saltypes.hpp"

namespace sal
{
class RirAnalysis
{
public:
  static mcl::Vector<Sample>
  EnergyDecayCurve(
    mcl::Vector<Sample> room_impulse_response);

  /** 
   Returns the reverberation time RT60, i.e. the point where the EDC
   reaches -60 dB in [s]. Returns the length of the rir if the EDC doesn't
   fall below -60 dB. 
   */
  static Time Rt60(
    mcl::Vector<Sample> room_impulse_response,
    Time sampling_frequency);

  /**
   Returns the reverberation sample RT60, i.e. the point where the EDC
   reaches -60 dB in number of samples. Returns the length of the rir 
   if the EDC doesn's fall below -60 dB.
   */
  static Time Rs60(
    mcl::Vector<Sample> room_impulse_response);

  static Time Rs60FromEdc(
    mcl::Vector<Sample> edc);

  static Time Rs(
    mcl::Vector<Sample> room_impulse_response,
    Sample decay_value);

  static Time RsFromEdc(
    mcl::Vector<Sample> edc,
    Sample decay_value);

  static bool Test();
};



} // namespace sal


#include "riranalysis_impl.hpp"
