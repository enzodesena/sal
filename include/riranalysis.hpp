/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SDN_EDC_H
#define SDN_EDC_H

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


mcl::Vector<sal::Sample>
RirAnalysis::EnergyDecayCurve(
  mcl::Vector<sal::Sample> rir)
{
  // cs = cumsum(fliplr(h.^2));
  mcl::Vector<sal::Sample> cs = mcl::CumSum(mcl::Flip(mcl::Pow(rir, 2.0)));

  // edc = 10*log10(fliplr(cs./cs(end)));
  return mcl::Multiply<sal::Sample>
  (
    mcl::Log10(
      mcl::Flip(
        mcl::Multiply<sal::Sample>(cs, 1.0 / cs[cs.size() - 1]))), 10.0);
}


sal::Time RirAnalysis::Rt60(
  mcl::Vector<sal::Sample> rir,
  sal::Time sampling_frequency)
{
  return Rs60(rir) / sampling_frequency;
}


sal::Time RirAnalysis::Rs60(
  mcl::Vector<sal::Sample> rir)
{
  return Rs(rir, -60.0);
}


sal::Time RirAnalysis::Rs(
  mcl::Vector<sal::Sample> rir,
  sal::Sample decay_value)
{
  mcl::Vector<sal::Sample> edc = EnergyDecayCurve(rir);
  return Rs60FromEdc(edc);
}


sal::Time RirAnalysis::Rs60FromEdc(
  mcl::Vector<sal::Sample> edc)
{
  return RsFromEdc(edc, -60.0);
}


sal::Time RirAnalysis::RsFromEdc(
  mcl::Vector<sal::Sample> edc,
  sal::Sample decay_value)
{
  const sal::Int N = edc.size();
  for (sal::Int i = 0; i < N; ++i)
  {
    if (edc[i] < decay_value)
    {
      return i;
    }
  }
  return N;
}

} // namespace sal

#endif
