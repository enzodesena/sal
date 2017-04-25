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
#include "salconstants.h"

namespace sal {
  
enum HeadRefOrientation {
  standard, // Head facing positive x-axis; positive z-axis passing through jaw and then scalp
  y_z  // Head facing positive y-axis; positive z-axis passing through jaw and then scalp
};
  
class BinauralMicInstance;

class SAL_API BinauralMic : public StereoMicrophone {
public:
  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  BinauralMic(const mcl::Point& position,
              const mcl::Quaternion orientation,
              const UInt update_length,
              const HeadRefOrientation reference_orientation = standard);
  
  void set_update_length(UInt update_length) { update_length_ = update_length; }
  
  /** When bypass_ is true, the signals will not be filtered by the HRTF */
  void set_bypass(bool bypass);
    
  virtual void Tick();
  
  virtual void Reset();
  
  virtual bool IsCoincident() { return true; }
  virtual bool IsFrameEnabled() { return true; }
  
  virtual ~BinauralMic() {}
  
private:
  
  /** Retrieves the BRIR for a source in position `point`.
   The head is assumed to be positioned lying on the z-axis and facing
   the positive x-direction. E.g. a point on the positive x-axis
   is facing directly ahead of the head. */
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) = 0;
  
  
  virtual void RecordPlaneWaveRelative(const Sample& sample, const mcl::Point& point,
                                       const UInt& wave_id);
  
  virtual void RecordPlaneWaveRelative(const Signal& signal, const mcl::Point& point,
                                       const UInt& wave_id);
  
  void CreateInstanceIfNotExist(const UInt& wave_id);
  
  std::map<UInt, BinauralMicInstance> instances_left_;
  std::map<UInt, BinauralMicInstance> instances_right_;
  
  /** How long it takes to update the underlying HRTF filter */
  UInt update_length_;
  
  /** When bypass_ is true, the signals will not be filtered by the HRTF */
  bool bypass_;
  
  friend class BinauralMicInstance;
  
protected:
  HeadRefOrientation reference_orientation_;
};


  
  
  
class BinauralMicInstance {
private:
  BinauralMicInstance(BinauralMic* base_mic, Ear ear, sal::UInt update_length,
                      const HeadRefOrientation reference_orientation = standard) :
  base_mic_(base_mic),
  filter_(mcl::FirFilter::GainFilter(1.0)),
  ear_(ear),
  previous_point_(mcl::Point(NAN, NAN, NAN)),
  update_length_(update_length),
  reference_orientation_(reference_orientation) {}
  
  Sample RecordPlaneWaveRelative(const Sample& sample, const mcl::Point& point);
  
  Signal RecordPlaneWaveRelative(const Signal& signal, const mcl::Point& point);
  
  /**
   The microphone object is called for every sample, while the position
   of SDN's elements is changed once in a while. Hence, these angles are
   stored so that we don't need to update the filter coefficients at every
   sample, but only when something changes.
   */
  mcl::Point previous_point_;
  
  void UpdateFilter(const mcl::Point& point);
  
  mcl::FirFilter filter_;
  
  Ear ear_;
  
  BinauralMic* base_mic_;
  
  sal::UInt update_length_;
  
  HeadRefOrientation reference_orientation_;
  
  friend class BinauralMic;
};
  
  
  
  
class SAL_API DatabaseBinauralMic : public BinauralMic {
public:
  DatabaseBinauralMic(const mcl::Point& position,
                      const mcl::Quaternion orientation,
                      const UInt update_length,
                      const HeadRefOrientation reference_orientation = standard);
  
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
