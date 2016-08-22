/*
 butter.h
 MCL
 
 Authors: user261002 @ stackoverflow.com
 
 Adapted from "http://stackoverflow.com/questions/10373184/bandpass-butterworth-filter-implementation-in-c"
 */


#ifndef MCL_BUTTER_H
#define MCL_BUTTER_H

#include <vector>

namespace mcl {
  
  
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
