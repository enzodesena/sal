/*
 directivityhrtf.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 106 $
 Last changed date:  $Date: 2012-06-22 03:31:02 +0100 (Fri, 22 Jun 2012) $
 */

#ifndef SAT_DIRECTIVITYHRTF_H
#define SAT_DIRECTIVITYHRTF_H

#include <vector>
#include "microphone.h"
#include "mcl.h"
#include "sattypes.h"
#include "stream.h"
#include "array.h"


namespace sat {


class DirectivityKemar : public Microphone {
public:
  // directory must contain the hrtf database.
  DirectivityKemar(Point position, AngleType theta, AngleType phi, AngleType psi,
                   const char * directory, StereoStream* stream);
  
  static bool Test();
  
  virtual ~DirectivityKemar() {};
  
private:
  
  virtual SampleType Filter(SampleType sample,  Point point);
  
  // Returns the elevation index for kemar database for elevation in azimuth. 
  // The index departs from 0.
  UIntType FindElevationIndex(AngleType elevation);
  
  
  // Returns the azimuthal index for kemar database for azimuth in grad. 
  // The index departs from 0.
  UIntType FindAzimuthIndex(AngleType azimuth, UIntType elevation_index);
  
  
//  virtual std::vector<SampleType> ImpulseResponse(Point point) const {
//    // TODO: IMPLEMENT
//    return mcl::UnaryVector<SampleType>(1.0);
//  }
  
  // The microphone object is called for every sample, while the position of SDN's 
  // elements is changed once in a while. Hence, these angles are stored so that we don't
  // need to update the filter coefficients at every sample, but only when something 
  // changes.
  Point previous_point_;
  
  
  // Database
  Array<Array<std::vector<SampleType>, 72>, 14> hrtf_database_;
  
  // All entries are divided by this value to make it acceptable amplitude.
  const SampleType normalising_value_;
  mcl::FirFilter filter_;
  
  StereoStream* stream_;
};
  
  
} // namespace sat

#endif
