/*
 directivityhrtf.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_BINAURALMIC_H
#define SAL_BINAURALMIC_H

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
  
class BinauralMicInstance;

class BinauralMic : public StereoMicrophone {
public:
  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  BinauralMic(Point position, Angle theta, Angle phi, Angle psi);
  
  virtual void Tick();
  
  virtual void Reset();
  
  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(mcl::DigitalFilter* filter);
  
  virtual ~BinauralMic() {}
  
protected:
  
  // Database
  Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS> hrtf_database_right_;
  Array<Array<Signal, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS> hrtf_database_left_;
  
  
private:
  
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt& wave_id);
  
  /**
   Returns the elevation index for the database for elevation in azimuth.
   The index departs from 0.
   */
  virtual UInt FindElevationIndex(Angle elevation) = 0;
  
  
  /**
   Returns the azimuthal index for the database for azimuth in grad.
   The index departs from 0.
   */
  virtual UInt FindAzimuthIndex(Angle azimuth, UInt elevation_index) = 0;
  
  
  std::map<UInt, BinauralMicInstance> instances_left_;
  std::map<UInt, BinauralMicInstance> instances_right_;
  
  
  friend class BinauralMicInstance;
};

class BinauralMicInstance {
private:
  BinauralMicInstance(BinauralMic* base_mic, Ear ear) :
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
  
  BinauralMic* base_mic_;
  
  friend class BinauralMic;
};
  
  
} // namespace sal

#endif
