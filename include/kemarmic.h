/*
 kemarmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_KEMARMIC_H
#define SAL_KEMARMIC_H

#define NUM_ELEVATIONS_KEMAR 14
#define NORMALISING_VALUE_KEMAR 30000.0

#include <map>
#include <vector>
#include "microphone.h"
#include "mcl.h"
#include "saltypes.h"
#include "stream.h"
#include "array.h"
#include "binauralmic.h"


namespace sal {
  
class KemarMic : public BinauralMic {
public:
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   */
  KemarMic(Point position, Angle theta, Angle phi, Angle psi,
           const std::string directory);
  
  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(mcl::DigitalFilter* filter);
  
  static bool Test();
  
  virtual ~KemarMic() {}
private:
  
  virtual Signal GetBrir(const Ear ear, const Point& point);
  
  std::vector<std::vector<Signal> > Load(const Ear ear,
                                         const std::string directory);
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  UInt FindElevationIndex(Angle elevation);
  
  
  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  UInt FindAzimuthIndex(Angle azimuth, UInt elevation_index);
  
  
  // Database
  std::vector<std::vector<Signal> > hrtf_database_right_;
  std::vector<std::vector<Signal> > hrtf_database_left_;
  
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements_;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations_;
};
  

  
} // namespace sal

#endif
