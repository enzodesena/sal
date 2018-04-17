/*
 ambisonics.h
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This include contains definition of classes and functions
 related to higer-order ambisonics.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_HOA_H
#define SAL_HOA_H


#include "microphone.h"
#include "point.h"
#include "stream.h"
#include "decoder.h"
#include "microphonearray.h"
#include "salconstants.h"
#include "iirfilter.h"

namespace sal {

enum AmbisonicsConvention {
  sqrt2,
  N3D
};

class SAL_API AmbisonicsMic : public Microphone {
public:
  /**
   Constructs an Ambisonics microphone.
   `position` is the position of the microphone in the recording space.
   `order` is the HOA order
   The microphone is placed horizontaly (equivalent directivity patterns)
   if theta = 0.0. I am thinking of soundfield microphone, which is pointing
   upwards.
   */
  AmbisonicsMic(const mcl::Point& position, mcl::Quaternion orientation,
                UInt order, AmbisonicsConvention convention = sqrt2) :
          Microphone(position, orientation),
          order_(order), convention_(convention) {}
  
  virtual void Tick() noexcept {
    stream_.Tick();
  }
  
  virtual bool IsCoincident() { return true; }
  
  /** Returns the BFormat stream of the microphone */
  BFormatStream* stream() { return &stream_; }
  
  static std::vector<mcl::Real> HorizontalEncoding(UInt order, Angle theta);
  
  static bool Test();
private:
  virtual void AddPlaneWaveRelative(const Signal& signal, const mcl::Point& point,
                                       const Int wave_id) noexcept;
  
  const Int order_;
  BFormatStream stream_;
  AmbisonicsConvention convention_;
};

  
/** 
 Implements horizontal higher order ambisonics with regular loudspeakers
 configuration (e.g. pentagon for II-order etc..).
 */
class SAL_API AmbisonicsHorizDec : public Decoder {
public:
  /**
   `position` is the position of the microphone in the recording space.
   `order` is the HOA order `num_loudspeakers` is the number of loudspeakers
   forming the regular polygon in the reproduction space.
   Exact reconstruction of the 2*`order`+1 circular harmonics is
   achieved if num_loudspeakers = 2*`order`+1 (e.g. penthagon loudspeaker
   for a second-order ambisonics).
   All loudspeakers are at polar angle is PI/2.0, i.e. are on an horizontal
   plane relative to the reference system. In other words all loudspeakers
   lie on a plane that is parallel with the x-y plane. The first loudspeaker
   is on the x-axis (azimuthal angle is 0).
   If `energy_decoding` is true a maximum energy decoding will be used for
   frequencies higher than `cut_off_frequency`.
   If `near_field_correction` is true, the signals will be processed using
   J. Daniel's near-field correction.
   */
  AmbisonicsHorizDec(const Int order,
                     const bool energy_decoding,
                     const Time cut_off_frequency,
                     const std::vector<Angle>& loudspeaker_angles,
                     const bool near_field_correction,
                     const Length loudspeakers_distance,
                     const Time sampling_frequency,
                     const Speed sound_speed,
                     BFormatStream* input_stream);
  
  virtual void Decode();
  
  
  static bool Test();

  virtual ~AmbisonicsHorizDec() {}
  
private:
  
  static mcl::Matrix<Sample>
  ModeMatchingDec(UInt order, const std::vector<Angle>& loudspeaker_angles);
  
  /**
   amb_re_weights_matrix produces the diagonal matrix of weights for energy
   vector maximization. E.g. diag(g0,g1,g1,g2,g2) for the N=2 2D case.
   */
  static mcl::Matrix<Sample>
  MaxEnergyDec(UInt order, const std::vector<Angle>& loudspeaker_angles);
  
  /**
   Produces the weights for maximum energy vector for a regular polygon.
   N is ambisonics order. The function outputs g(0) = 1 which means that
   normalisation is such that pressure is preserved.
   */
  static Sample MaxEnergyDecWeight(UInt index, UInt order) {
    return (Sample) cos(((Angle) index)*PI/(2.0*((Angle) order)+2.0));
  }
  
  static std::vector<Sample> PullFrame(UInt order, BFormatStream* stream);
  
  /**
   Produces the near field correction
   filters as described in "Spatial Sound Encoding Including Near Field
   Effect: Introducing Length Coding Filters and a Viable, New Ambisonic
   Format" by J. Daniel, AES 23rd, Int. Conf., 2003.
   `order` is the spherical component order. `loudspeaker_distance`
   is the loudspeaker distance from the centre.
   `sampling_frequency` is the sampling frequency (in Hz) and `sound_speed`
   the speed of sound (in m/s).
   */
  static mcl::IirFilter NFCFilter(const Int order,
                                  const Length loudspeaker_distance,
                                  const Time sampling_frequency,
                                  const Length sound_speed);

  
  /**
   Produces the high-pass and low-pass crossover filter with cutoff
   frequency Fc and sampling frequency Fs as descibed by A. Heller et al.,
   "Is My Decoder Ambisonic?", in AES 125th Convention.
   */
  static mcl::IirFilter CrossoverFilterLow(const Time cut_off_frequency,
                                           const Time sampling_frequency);
  static mcl::IirFilter CrossoverFilterHigh(const Time cut_off_frequency,
                                            const Time sampling_frequency);
  
  std::vector<Angle> loudspeaker_angles_;
  UInt num_loudspeakers_;
  bool near_field_correction_;
  Length loudspeakers_distance_;
  bool energy_decoding_;
  // Ambisonics order
  UInt order_;
  
  // One filter per component
  std::vector<mcl::IirFilter> nfc_filters_;
  // One filter per loudspeaker
  std::vector<mcl::IirFilter> crossover_filters_high_;
  std::vector<mcl::IirFilter> crossover_filters_low_;
  
  // Cache the decoding matrix (mode-matching) for performance purposes.
  mcl::Matrix<Sample> mode_matching_matrix_;
  
  // Cache the decoding matrix (maximum energy) for performance purposes.
  mcl::Matrix<Sample> max_energy_matrix_;
  
  Time sampling_frequency_;
  BFormatStream* input_stream_;
};
  
  
} // namespace sal

#endif

