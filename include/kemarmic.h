/*
 directivityhrtf.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DIRECTIVITYHRTF_H
#define SAL_DIRECTIVITYHRTF_H

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


namespace sal {

class KemarMicInstance;
  
class KemarMic : public StereoMicrophone {
public:
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   */
  KemarMic(Point position, Angle theta, Angle phi, Angle psi,
           const std::string directory);
  
  virtual void Tick();
  
  virtual void Reset();
  
  static bool Test();
  
  /** 
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter 
   */
  void FilterAll(mcl::DigitalFilter* filter);
  
  virtual ~KemarMic() {}
private:
    
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt& wave_id);
  
  
  
  static Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS>
          Load(const Ear ear, const std::string directory);
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  static UInt FindElevationIndex(Angle elevation);
  
  
  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  static UInt FindAzimuthIndex(Angle azimuth, UInt elevation_index);
  
  
  std::map<UInt, KemarMicInstance> instances_left_;
  std::map<UInt, KemarMicInstance> instances_right_;
  
  // Database
  Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS> hrtf_database_right_;
  Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS> hrtf_database_left_;
  
  
  friend class KemarMicInstance;
};
  
class KemarMicInstance {
private:
  KemarMicInstance(KemarMic* base_mic, Ear ear) :
          base_mic_(base_mic),
          filter_(mcl::FirFilter::GainFilter(1.0)),
          ear_(ear),
          previous_point_(Point(NAN, NAN, NAN)) {}
  
  
  Sample RecordPlaneWaveRelative(const Sample& sample, const Point& point);
  
  
  /**
   The microphone object is called for every sample, while the position
   of SDN's elements is changed once in a while. Hence, these angles are
   stored so that we don't need to update the filter coefficients at every
   sample, but only when something changes.
   */
  Point previous_point_;
  
  mcl::FirFilter filter_;
  
  Ear ear_;
  
  KemarMic* base_mic_;
  
  friend class KemarMic;
};

  
} // namespace sal

#endif
