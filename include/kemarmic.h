/*
 directivityhrtf.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_KEMARMIC_H
#define SAL_KEMARMIC_H

#define NUM_ELEVATIONS 14
#define MAX_NUM_AZIMUTHS 72
#define NORMALISING_VALUE 30000.0

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
  
  static bool Test();
  
  virtual ~KemarMic() {}
private:
  
  
  static Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS>
          Load(const Ear ear, const std::string directory);
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  virtual UInt FindElevationIndex(Angle elevation);
  
  
  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  virtual UInt FindAzimuthIndex(Angle azimuth, UInt elevation_index);
  
};
  

  
} // namespace sal

#endif
