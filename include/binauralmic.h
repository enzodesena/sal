/*
 binauralmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_BINAURALMIC_H
#define SAL_BINAURALMIC_H

#include <map>
#include <vector>
#include "microphone.h"
#include "saltypes.h"
#include "array.h"
#include "salconstants.h"
#include "firfilter.h"

namespace sal {
  
enum HeadRefOrientation {
  standard, // Head facing positive x-axis; positive z-axis passing through jaw and then scalp
  y_z  // Head facing positive y-axis; positive z-axis passing through jaw and then scalp
};
  
class BinauralMicInstance;

class BinauralMic : public StereoMicrophone {
public:
  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  BinauralMic(const mcl::Point& position,
              const mcl::Quaternion orientation,
              const Int update_length,
              const HeadRefOrientation reference_orientation = standard);
  
  void set_update_length(Int update_length) noexcept { update_length_ = update_length; }
  
  /** When bypass_ is true, the signals will not be filtered by the HRTF */
  void set_bypass(bool bypass) noexcept;
  
  virtual void Reset() noexcept;
  
  virtual bool IsCoincident() const noexcept { return true; }
  
  virtual ~BinauralMic() {}
  
  
  virtual void AddPlaneWaveRelative(const Sample* signal,
                                    const Int num_samples,
                                    const mcl::Point& point,
                                    const Int wave_id,
                                    Buffer& output_buffer) noexcept;
  
private:
  
  /** Retrieves the BRIR for a source in position `point`.
   The head is assumed to be positioned lying on the z-axis and facing
   the positive x-direction. E.g. a point on the positive x-axis
   is facing directly ahead of the head. */
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept = 0;
  
  void CreateInstanceIfNotExist(const Int wave_id) noexcept;
  
  std::map<UInt, BinauralMicInstance> instances_;
  
  /** How long it takes to update the underlying HRTF filter */
  Int update_length_;
  
  /** When bypass_ is true, the signals will not be filtered by the HRTF */
  bool bypass_;
  
  friend class BinauralMicInstance;
  
protected:
  
  HeadRefOrientation reference_orientation_;
};


  
class DatabaseBinauralMic : public BinauralMic {
public:
  DatabaseBinauralMic(const mcl::Point& position,
                      const mcl::Quaternion orientation,
                      const Int update_length,
                      const HeadRefOrientation reference_orientation = standard);
  
  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(mcl::DigitalFilter* filter);
  
  virtual ~DatabaseBinauralMic() {}
protected:
  // Database
  std::vector<std::vector<Signal> > hrtf_database_right_;
  std::vector<std::vector<Signal> > hrtf_database_left_;
};
  
  
  
  
class BinauralMicInstance {
private:
  BinauralMicInstance(BinauralMic* base_mic, sal::Int update_length,
                      const HeadRefOrientation reference_orientation = standard) :
  previous_point_(mcl::Point(NAN, NAN, NAN)),
  base_mic_(base_mic),
  filter_left_(mcl::FirFilter::GainFilter(1.0)),
  filter_right_(mcl::FirFilter::GainFilter(1.0)),
  update_length_(update_length),
  reference_orientation_(reference_orientation) {}
  
  void AddPlaneWaveRelative(const Sample* input_data,
                            const Int num_samples,
                            const mcl::Point& point,
                            StereoBuffer& output_buffer) noexcept;
  
  void UpdateFilter(const mcl::Point& point) noexcept;
  
  /**
   The microphone object is called for every sample, while the position
   of SDN's elements is changed once in a while. Hence, these angles are
   stored so that we don't need to update the filter coefficients at every
   sample, but only when something changes.
   */
  mcl::Point previous_point_;
  
  BinauralMic* base_mic_;
  mcl::FirFilter filter_left_;
  mcl::FirFilter filter_right_;
  sal::Int update_length_;
  HeadRefOrientation reference_orientation_;
  
  friend class BinauralMic;
};
  
  
} // namespace sal

#endif
