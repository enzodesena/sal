/*
 iirfilter.h
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 153 $
 Last changed date:  $Date: 2012-10-06 21:39:09 +0100 (Sat, 06 Oct 2012) $
 */

#ifndef MCL_IIRFILTER_H
#define MCL_IIRFILTER_H

#include "digitalfilter.h"



namespace mcl {

enum WallType {
  // 6 mm pile carpet bonded to open-cell foam underlay
  // alpha = [0.03,0.09,0.20,0.54,0.70,0.72,0.72];
  // f = [125, 250, 500, 1000, 2000, 4000, 8000];
  carpet_pile,
  
  // Cotton carpet
  // alpha = [0.07, 0.31, 0.49, 0.81, 0.66, 0.54, 0.48];
  // f = [125, 250, 500, 1000, 2000, 4000, 8000];
  carpet_cotton,
  
  // Walls, hard surfaces average (brick walls, plaster, hard floors, etc.)
  // alpha = [0.02, 0.02, 0.03, 0.03, 0.04, 0.05, 0.05];
  // f = [125, 250, 500, 1000, 2000, 4000, 8000];
  wall_bricks,
  
  // Fissured ceiling tile
  // alpha = [0.49,0.53,0.53,0.75,0.92,0.99];
  // f = [125, 250, 500, 1000, 2000, 4000];
  ceiling_tile
};

class IirFilter : public DigitalFilter {
public:
  // Constructs an IIR filter (II-type direct implementation). B and A are numerator
  // and denominator of the filter, respectively. hello
  IirFilter(std::vector<Real> B,std::vector<Real> A);
  
  // Returns the output of the filter for an input equal to `input`. 
  // For example, if B=1, A=1, output will be equal to input. 
  // As a second example, if B=[0,1], A=[1], you will have 
  // (1) Filter(0.5)==0 and then
  // (2) Filter(0.0)==0.5
  virtual Real Filter(const Real input);
  
  // Returns the output of the filter for an input signal equal to `input`.
  virtual std::vector<Real> Filter(const std::vector<Real>& input);
  
  // Updates the filter coefficients. May cause articafts if the coefficients are
  // updated too rapidly.
  void UpdateFilter(std::vector<Real> B,std::vector<Real> A);
  
  // Constructs a filter for which output==input always.
  static IirFilter IdenticalFilter();
  
  // Constructs a filter for which output==gain*input always.
  static IirFilter GainFilter(const Real gain);
  
  // Get wall filters of type wall_type and for FS given by sampling_frequency
  static IirFilter WallFilter(WallType wall_type, Real sampling_frequency);
  
  std::vector<Real> B();
  
  std::vector<Real> A();
  
  static bool Test();
  
  IirFilter & operator= (const IirFilter &);
  
  IirFilter (const IirFilter&);
  
  virtual ~IirFilter();
  
private:
  std::vector<Real> B_;
  std::vector<Real> A_;
  
  // By storing A[0] before normalisation we can output B() and A() before
  // normalisation while keeping the internal representation normalised
  Real A0_;
  
  Real* state_;
};
  
} // namespace mcl

#endif
