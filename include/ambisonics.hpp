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
#include "matrix.hpp"
#include "salconstants.hpp"
#include "digitalfilter.hpp"

namespace sal
{
enum AmbisonicsConvention
{
  sqrt2,
  N3D
};

template<typename T>
class AmbisonicsDir : public DirectivityInterface<T>
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
  AmbisonicsDir(
    const size_t order,
    const AmbisonicsConvention convention = sqrt2)
    : order_(order)
    , convention_(convention)
  {
  }

  void ResetState() noexcept override
  {
  }

  static size_t GetNumChannels(
    const size_t order) noexcept;
  
  static mcl::Vector<T> HorizontalEncoding(
    const size_t order,
    const Angle theta);

  void ReceiveAdd(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output_buffer) noexcept override;
  
  static size_t GetChannelId(
    const Int degree,
    const size_t order) noexcept;
private:
  
  const size_t order_;
  AmbisonicsConvention convention_;
};


/** 
 Implements horizontal higher order ambisonics with regular loudspeakers
 configuration (e.g. pentagon for II-order etc..).
 */
template<typename T>
class AmbisonicsHorizDec
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
    size_t order,
    bool energy_decoding,
    Time cut_off_frequency,
    const mcl::Vector<Angle>& loudspeaker_angles,
    bool near_field_correction,
    Length loudspeakers_distance,
    Time sampling_frequency,
    Speed sound_speed);

  void Decode(
    const Buffer<T>& input_buffer,
    Buffer<T>& output_buffer);

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
  static mcl::DigitalFilter<T> NFCFilter(
    Int order,
    Length loudspeaker_distance,
    Time sampling_frequency,
    Length sound_speed);

  /**
   Produces the high-pass and low-pass crossover filter with cutoff
   frequency Fc and sampling frequency Fs as descibed by A. Heller et al.,
   "Is My Decoder Ambisonic?", in AES 125th Convention.
   */
  static mcl::DigitalFilter<T> CrossoverFilterLow(
    Time cut_off_frequency,
    Time sampling_frequency);
  static mcl::DigitalFilter<T> CrossoverFilterHigh(
    Time cut_off_frequency,
    Time sampling_frequency);

  /**
   amb_re_weights_matrix produces the diagonal matrix of weights for energy
   vector maximization. E.g. diag(g0,g1,g1,g2,g2) for the N=2 2D case.
   */
  static mcl::Matrix<T>
  MaxEnergyDec(
    const size_t order,
    const mcl::Vector<Angle>& loudspeaker_angles);


  /**
   Produces the weights for maximum energy vector for a regular polygon.
   N is ambisonics order. The function outputs g(0) = 1 which means that
   normalisation is such that pressure is preserved.
   */
  static T MaxEnergyDecWeight(
    Int index,
    size_t order)
  {
    return (Sample)cos(((Angle)index) * PI / (2.0 * ((Angle)order) + 2.0));
  }

  static mcl::Matrix<T>
  ModeMatchingDec(
    Int order,
    const mcl::Vector<Angle>& loudspeaker_angles);

private:


  static mcl::Vector<T> GetFrame(
    const size_t order,
    const size_t sample_id,
    const Buffer<T>& buffer);


  mcl::Vector<Angle> loudspeaker_angles_;
  size_t num_loudspeakers_;
  bool near_field_correction_;
  Length loudspeakers_distance_;
  bool energy_decoding_;
  // Ambisonics order
  size_t order_;

  // One filter per component
  mcl::Vector<mcl::DigitalFilter<T>> nfc_filters_;
  // One filter per loudspeaker
  mcl::Vector<mcl::DigitalFilter<T>> crossover_filters_high_;
  mcl::Vector<mcl::DigitalFilter<T>> crossover_filters_low_;

  // Cache the decoding matrix (mode-matching) for performance purposes.
  mcl::Matrix<T> mode_matching_matrix_;

  // Cache the decoding matrix (maximum energy) for performance purposes.
  mcl::Matrix<T> max_energy_matrix_;

  Time sampling_frequency_;
};
} // namespace sal

#include "ambisonics.ipp"
