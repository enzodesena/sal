/*
 binauralmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_BINAURALMIC_H
#define SAL_BINAURALMIC_H

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
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~BinauralMic() {}
  
private:
  
  /** Retrieves the BRIR for a source in position `point`.
   The head is assumed to be positioned lying on the x-axis and facing
   the positive z-direction. E.g. a point on the positive z-axis
   is facing directly ahead of the head. */
  virtual Signal GetBrir(const Ear ear, const Point& point) = 0;
  
  
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt& wave_id);
  
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
  
  
  
  
class DatabaseBinauralMic : public BinauralMic {
public:
  DatabaseBinauralMic(Point position, Angle theta, Angle phi, Angle psi);
  
  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(mcl::DigitalFilter* filter);
  
protected:
  // Database
  std::vector<std::vector<Signal> > hrtf_database_right_;
  std::vector<std::vector<Signal> > hrtf_database_left_;
};
} // namespace sal

#endif
