/*
 ambisonics.h
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This include contains definition of classes and functions
 related to higer-order ambisonics.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#include "receiver.hpp"
#include "point.hpp"
#include "decoder.hpp"
#include "microphonearray.hpp"
#include "salconstants.hpp"
#include "digitalfilter.hpp"

namespace sal
{
enum AmbisonicsConvention
{
  sqrt2,
  N3D
};


class AmbisonicsMic : public Microphone
{
public:
  /**
   Constructs an Ambisonics microphone.
   `position` is the position of the microphone in the recording space.
   `order` is the HOA order
   The microphone is placed horizontaly (equivalent directivity patterns)
   if theta = 0.0. I am thinking of soundfield microphone, which is pointing
   upwards.
   */
  AmbisonicsMic(
    const mcl::Point& position,
    mcl::Quaternion orientation,
    Int order,
    AmbisonicsConvention convention = sqrt2)
    : Microphone(position, orientation)
    , order_(order)
    , convention_(convention)
  {
  }


  bool IsCoincident() const noexcept override
  {
    return true;
  }


  Int num_channels() const noexcept override
  {
    return BFormatBuffer::GetNumChannels(order_);
  }


  static mcl::Vector<mcl::Real> HorizontalEncoding(
    Int order,
    Angle theta);

  static bool Test();
  virtual void AddPlaneWaveRelative(
    const Sample* input_data,
    Int num_samples,
    const mcl::Point& point,
    Int wave_id,
    Buffer& output_buffer) noexcept;

private:

  const Int order_;
  AmbisonicsConvention convention_;
};


/** 
 Implements horizontal higher order ambisonics with regular loudspeakers
 configuration (e.g. pentagon for II-order etc..).
 */
class AmbisonicsHorizDec : public Decoder
{
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
  AmbisonicsHorizDec(
    Int order,
    bool energy_decoding,
    Time cut_off_frequency,
    const mcl::Vector<Angle>& loudspeaker_angles,
    bool near_field_correction,
    Length loudspeakers_distance,
    Time sampling_frequency,
    Speed sound_speed);

  void Decode(
    const Buffer& input_buffer,
    Buffer& output_buffer) override;

  static bool Test();


  virtual ~AmbisonicsHorizDec()
  {
  }


private:

  static mcl::Matrix<Sample>
  ModeMatchingDec(
    Int order,
    const mcl::Vector<Angle>& loudspeaker_angles);

  /**
   amb_re_weights_matrix produces the diagonal matrix of weights for energy
   vector maximization. E.g. diag(g0,g1,g1,g2,g2) for the N=2 2D case.
   */
  static mcl::Matrix<Sample>
  MaxEnergyDec(
    Int order,
    const mcl::Vector<Angle>& loudspeaker_angles);


  /**
   Produces the weights for maximum energy vector for a regular polygon.
   N is ambisonics order. The function outputs g(0) = 1 which means that
   normalisation is such that pressure is preserved.
   */
  static Sample MaxEnergyDecWeight(
    Int index,
    Int order)
  {
    return (Sample)cos(((Angle)index) * PI / (2.0 * ((Angle)order) + 2.0));
  }


  static mcl::Vector<Sample> GetFrame(
    Int order,
    Int sample_id,
    const Buffer& buffer);

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
  static mcl::IirFilter NFCFilter(
    Int order,
    Length loudspeaker_distance,
    Time sampling_frequency,
    Length sound_speed);

  /**
   Produces the high-pass and low-pass crossover filter with cutoff
   frequency Fc and sampling frequency Fs as descibed by A. Heller et al.,
   "Is My Decoder Ambisonic?", in AES 125th Convention.
   */
  static mcl::IirFilter CrossoverFilterLow(
    Time cut_off_frequency,
    Time sampling_frequency);
  static mcl::IirFilter CrossoverFilterHigh(
    Time cut_off_frequency,
    Time sampling_frequency);

  mcl::Vector<Angle> loudspeaker_angles_;
  Int num_loudspeakers_;
  bool near_field_correction_;
  Length loudspeakers_distance_;
  bool energy_decoding_;
  // Ambisonics order
  Int order_;

  // One filter per component
  mcl::Vector<mcl::IirFilter> nfc_filters_;
  // One filter per loudspeaker
  mcl::Vector<mcl::IirFilter> crossover_filters_high_;
  mcl::Vector<mcl::IirFilter> crossover_filters_low_;

  // Cache the decoding matrix (mode-matching) for performance purposes.
  mcl::Matrix<Sample> mode_matching_matrix_;

  // Cache the decoding matrix (maximum energy) for performance purposes.
  mcl::Matrix<Sample> max_energy_matrix_;

  Time sampling_frequency_;
};
} // namespace sal

#include "ambisonics.ipp"
