/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "riranalysis.h"
#include "vectorop.h"

namespace sal {

std::vector<sal::Sample> RirAnalysis::EnergyDecayCurve(
    std::vector<sal::Sample> rir) {
  // cs = cumsum(fliplr(h.^2));
  std::vector<sal::Sample> cs = dsp::CumSum(dsp::Flip(dsp::Pow(rir, 2.0)));

  // edc = 10*log10(fliplr(cs./cs(end)));
  return dsp::Multiply<sal::Sample>(
      dsp::Log10(
          dsp::Flip(dsp::Multiply<sal::Sample>(cs, 1.0 / cs[cs.size() - 1]))),
      10.0);
}

sal::Time RirAnalysis::Rt60(std::vector<sal::Sample> rir,
                            sal::Time sampling_frequency) {
  return Rs60(rir) / sampling_frequency;
}

sal::Time RirAnalysis::Rs60(std::vector<sal::Sample> rir) {
  return Rs(rir, -60.0);
}

sal::Time RirAnalysis::Rs(std::vector<sal::Sample> rir,
                          sal::Sample decay_value) {
  std::vector<sal::Sample> edc = EnergyDecayCurve(rir);
  return Rs60FromEdc(edc);
}

sal::Time RirAnalysis::Rs60FromEdc(std::vector<sal::Sample> edc) {
  return RsFromEdc(edc, -60.0);
}

sal::Time RirAnalysis::RsFromEdc(std::vector<sal::Sample> edc,
                                 sal::Sample decay_value) {
  const sal::Int N = edc.size();
  for (sal::Int i = 0; i < N; ++i) {
    if (edc[i] < decay_value) {
      return i;
    }
  }
  return N;
}

}  // namespace sal
