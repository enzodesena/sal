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
#include "saltypes.h"
#include "array.h"
#include "binauralmic.h"
#include "salconstants.h"

namespace sal {
  

  
class SAL_API KemarMic : public DatabaseBinauralMic {
public:
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   With `num_samples` you can choose the length of the 
   BRIR. If set to zero yields the entire BRIR.
   */
  KemarMic(const mcl::Point& position,
           const mcl::Quaternion orientation,
           const std::string directory, const Int num_samples = 0,
           const Int update_length = 0,
           const HeadRefOrientation reference_orientation = standard);
  
  static bool Test();
private:
  
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept;
  
  std::vector<std::vector<Signal> > Load(const Ear ear,
                                         const std::string directory,
                                         const Int num_samples);
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  Int FindElevationIndex(Angle elevation);
  
  
  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  Int FindAzimuthIndex(Angle azimuth, Int elevation_index);
  
  
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements_;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations_;
};
  

  
} // namespace sal

#endif
