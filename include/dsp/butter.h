/*
 butter.h
 MCL
 
 Authors: user261002 @ stackoverflow.com
 
 Adapted from "http://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c"
 */


#ifndef MCL_BUTTER_H
#define MCL_BUTTER_H

#include <vector>
#include "iirfilter.h"

namespace mcl {
  
/**
 Construncts a bandpass butterworth filter. Equivalent to Matlab's
 butter(order, [w_low, w_high])
 */
IirFilter Butter(const Int order,
                 const Real w_low, const Real w_high);

/** Constructs a digital octave filter with given center frequency
 and sampling frequency.
 */
IirFilter OctaveFilter(const Int order,
                       const Real center_frequency,
                       const Real sampling_frequency);
  
IirFilterBank OctaveFilterBank(const Int order,
                               const Int num_bands,
                               const Real starting_frequency,
                               const Real sampling_frequency);
  
std::vector<double> ComputeLP(int FilterOrder);
std::vector<double> ComputeHP(int FilterOrder);
std::vector<double> TrinomialMultiply(int FilterOrder,
                                      std::vector<double> b,
                                      std::vector<double> c);

std::vector<double> ComputeNumCoeffs(int FilterOrder,
                                     double Lcutoff,
                                     double Ucutoff,
                                     std::vector<double> DenC);

std::vector<double> ComputeDenCoeffs(int FilterOrder,
                                     double Lcutoff,
                                     double Ucutoff );
  
} // namespace mcl
  
#endif
