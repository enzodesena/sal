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

#include "array.h"
#include "firfilter.h"
#include "microphone.h"
#include "salconstants.h"
#include "saltypes.h"

namespace sal {

enum class HeadRefOrientation {
  standard,  // Head facing positive x-axis; positive z-axis passing through jaw
             // and then scalp
  y_z  // Head facing positive y-axis; positive z-axis passing through jaw and
       // then scalp
};

class BinauralMicInstance;

class BinauralMic : public StereoMicrophone {
 public:
  BinauralMic(const dsp::Point& position, const dsp::Quaternion orientation,
              const Int update_length,
              const HeadRefOrientation reference_orientation =
                  HeadRefOrientation::standard);

  void SetUpdateLength(Int update_length) noexcept {
    update_length_ = update_length;
  }

  /** When bypass_ is true, the signals will not be filtered by the HRTF */
  void SetBypass(bool bypass) noexcept;

  virtual void Reset() noexcept;

  bool IsCoincident() const noexcept { return true; }

  Int num_channels() const noexcept { return 2; }

  virtual void AddPlaneWaveRelative(std::span<const Sample> input_data,
                                    const dsp::Point& point,
                                    const size_t wave_id,
                                    Buffer& output_buffer) noexcept;

 private:
  /** Retrieves the BRIR for a source in position `point`.
   The head is assumed to be positioned lying on the z-axis and facing
   the positive x-direction. E.g. a point on the positive x-axis
   is facing directly ahead of the head. */
  virtual Signal GetBrir(const Ear ear, const dsp::Point& point) noexcept = 0;

  void CreateInstanceIfNotExist(const size_t wave_id) noexcept;

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
  DatabaseBinauralMic(const dsp::Point& position,
                      const dsp::Quaternion orientation,
                      const Int update_length,
                      const HeadRefOrientation reference_orientation =
                          HeadRefOrientation::standard);

  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(dsp::Filter& filter);

 protected:
  // Database
  std::vector<std::vector<Signal> > hrtf_database_right_;
  std::vector<std::vector<Signal> > hrtf_database_left_;
};

class BinauralMicInstance {
 private:
  BinauralMicInstance(BinauralMic* base_mic, sal::Int update_length,
                      const HeadRefOrientation reference_orientation =
                          HeadRefOrientation::standard,
                      const size_t max_input_size = 1 << 14)
      : previous_point_(dsp::Point(NAN, NAN, NAN)),
        base_mic_(base_mic),
        filter_left_(dsp::FirFilter::GainFilter(1.0)),
        filter_right_(dsp::FirFilter::GainFilter(1.0)),
        update_length_(update_length),
        reference_orientation_(reference_orientation),
        scratch_vector_(max_input_size, 0.0) {}

  void AddPlaneWaveRelative(std::span<const Sample> input_data,
                            const dsp::Point& point,
                            Buffer& output_buffer) noexcept;

  void UpdateFilter(const dsp::Point& point) noexcept;

  /**
   The microphone object is called for every sample, while the position
   of SDN's elements is changed once in a while. Hence, these angles are
   stored so that we don't need to update the filter coefficients at every
   sample, but only when something changes.
   */
  dsp::Point previous_point_;

  BinauralMic* base_mic_;
  dsp::FirFilter filter_left_;
  dsp::FirFilter filter_right_;
  sal::Int update_length_;
  HeadRefOrientation reference_orientation_;

  std::vector<Sample> scratch_vector_;

  friend class BinauralMic;
};

}  // namespace sal

#endif
