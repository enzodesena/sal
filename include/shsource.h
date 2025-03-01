/*
 ShSource.h
 Spatial Audio Library (SAL)
 Copyright (c) 2025, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 Josh Mannall
 */

#ifndef SAL_ShSource_H
#define SAL_ShSource_H

#include <map>
#include <vector>

#include "firfilter.h"
#include "source.h"
#include "saltypes.h"
#include "graphiceq.h"

namespace sal {

class ShSourceInstance;

class ShSource : public Source {
  public:
    ShSource(const dsp::Point& position,
             const dsp::Quaternion orientation,
             const std::vector<dsp::Real> centre_frequencies,
             const std::vector<std::vector<dsp::Complex>> sh_coefficients,
             const dsp::Real sampling_frequency) noexcept;

    void ProcessAudioRelative(std::span<const Sample> input_data,
                              const dsp::Point& observation_point_relative_to_source_look_direction,
                              const size_t wave_id,
                              std::span<Sample> output_data) noexcept;

  
    /**
    * @brief Calculate the directivity response for a given frequencies and direction
    *
    * @params frequencies The frequencies to calculate the directivity for
    * @params theta 0 to PI (0 to 180 degrees) 0 points along the forward axis, PI/2 perpendicular to the forward axis and PI is opposing the forward axis
    * @params phi 0 to 2PI (360 degrees) where 0 is the front, top or rear of the source and PI is the bottom of the source (rotates clockwise around the forward axis).
    *
    * @remark Uses front-pole orientation
    *
    * @return The directivity at the given frequency for a given direction
    */
    std::vector<dsp::Real> GetTransferFunction(const std::vector<dsp::Real>& frequencies, const dsp::Point& point) const;

    /**
    * @brief Calculate the directivity response for a given frequency and direction
    *
    * @params f The frequency to calculate the directivity for
    * @remark Uses front-pole orientation
    *
    * @return The directivity at the given frequency for a given direction */
    dsp::Real GetTransferFunction(dsp::Real f, const dsp::Point& point) const;
  
    void ResetFiltersState() noexcept;
  
    static bool Test();
  private:
    void CreateInstanceIfNotExist(const size_t wave_id) noexcept;

    
    
    std::vector<dsp::Real> GetGains(const dsp::Point& point) noexcept;
    
    dsp::GraphicEq GetGraphicEq() noexcept;
    
    std::map<UInt, ShSourceInstance> instances_;

    friend class ShSourceInstance;
    
    std::vector<dsp::Real> centre_frequencies_;                // Mid frequencies
    std::vector<std::vector<dsp::Complex>> sh_coefficients_;    // Spherical harmonics coefficients
    dsp::Real sampling_frequency_;
};

class ShSourceInstance {
 private:
  ShSourceInstance(ShSource* base_source)
      : previous_point_(dsp::Point(NAN, NAN, NAN)),
        base_source_(base_source),
      filter_(base_source->GetGraphicEq()) {}

  void ProcessAudioRelative(std::span<const Sample> input_data,
                            const dsp::Point& observation_point_relative_to_source_look_direction,
                            std::span<Sample> output_data) noexcept;
  
  void UpdateFilter(const dsp::Point& point) noexcept;
  
  /**
   The microphone object is called for every sample, while the position
   of SDN's elements is changed once in a while. Hence, these angles are
   stored so that we don't need to update the filter coefficients at every
   sample, but only when something changes.
   */
  dsp::Point previous_point_;

  ShSource* base_source_;
  dsp::GraphicEq filter_;

  friend class ShSource;
};


/**
* @brief Genelec 8020c directivity data
*/
const std::vector<dsp::Real> GenelecDirectivityFreqs = { 62.5, 125.0, 250.0, 500.0, 1e3, 2e3, 4e3, 8e3, 16e3 };

/**
* @brief Genelec 8020c directivity data as spherical harmonics
*
* @remark Calculated from the BRAS Database directivity measurements
*/
const static std::vector<std::vector<dsp::Complex>> GenelecDirectivityShCoeffs = {
  { 2.766636223198812 },
  { 3.510204359731806 },
  { 3.370669764960976 },
  
  { 2.64325252062826,
  dsp::Complex(-0.0637887899933280, 0.00709043682858875),
  0.475502894948934,
  dsp::Complex(0.0637887899933280, 0.00709043682858875),
  dsp::Complex(-0.00766656882903998, 0.00502613096609965),
  dsp::Complex(0.00552595525197679, 0.00669867042959554),
  0.146385344721295,
  dsp::Complex(-0.00552595525197679, 0.00669867042959554),
  dsp::Complex(-0.00766656882903998, -0.00502613096609965) },

  { 1.97128124928243,
  dsp::Complex(0.0288955622920283, 0.0101887659330981),
  0.543116368361039,
  dsp::Complex(-0.0288955622920283, 0.0101887659330981),
  dsp::Complex(-0.0222949902191432, 0.00482095521649736),
  dsp::Complex(0.0143165526072852, 0.00317936142931662),
  0.144068751090373,
  dsp::Complex(-0.0143165526072852, 0.00317936142931662),
  dsp::Complex(-0.0222949902191432, -0.00482095521649736) },
  
  { 1.37100588209144,
  dsp::Complex(0.0146562216497114, 0.00515044599364579),
  0.720386999496995,
  dsp::Complex(-0.0146562216497114, 0.00515044599364579),
  dsp::Complex(-0.0650615377560616, 0.00397170007680051),
  dsp::Complex(0.0367209089864466, 0.00618685999609908),
  0.351953301197736,
  dsp::Complex(-0.0367209089864466, 0.00618685999609908),
  dsp::Complex(-0.0650615377560616, -0.00397170007680051),
  dsp::Complex(-0.0357384354633624, -0.00265237385647239),
  dsp::Complex(-0.0532304055163828, 0.00285715119698396),
  dsp::Complex(0.0496970488762515, 0.00389936245744877),
  0.121330122148826,
  dsp::Complex(-0.0496970488762515, 0.00389936245744877),
  dsp::Complex(-0.0532304055163828, -0.00285715119698396),
  dsp::Complex(0.0357384354633624, -0.00265237385647239) },

  { 1.07987502306489,
  dsp::Complex(-0.0159120139766602, 0.0352113761857916),
  0.664568633630202,
  dsp::Complex(0.0159120139766602, 0.0352113761857916),
  dsp::Complex(-0.0339235653998775, 0.00392905938423214),
  dsp::Complex(-0.0136105518477092, 0.0159126538918982),
  0.287751426442446,
  dsp::Complex(0.0136105518477092, 0.0159126538918982),
  dsp::Complex(-0.0339235653998775, -0.00392905938423214),
  dsp::Complex(-0.0394958780644998, -0.00110778262618036),
  dsp::Complex(-0.0375235021578686, 0.00291360976273746),
  dsp::Complex(0.0348035789199694, 0.00885765396282877),
  0.120961170905888,
  dsp::Complex(-0.0348035789199694, 0.00885765396282877),
  dsp::Complex(-0.0375235021578686, -0.00291360976273746),
  dsp::Complex(0.0394958780644998, -0.00110778262618036) },

  { 1.02095654461750,
  dsp::Complex(0.0241799203216629, 0.0362664242624599),
  0.771135355920134,
  dsp::Complex(-0.0241799203216629, 0.0362664242624599),
  dsp::Complex(-0.0167508380936951, 0.00257611255022035),
  dsp::Complex(0.000676587781659708, 0.0274680154532966),
  0.348051354619507,
  dsp::Complex(-0.000676587781659708, 0.0274680154532966),
  dsp::Complex(-0.0167508380936951, -0.00257611255022035),
  dsp::Complex(-0.00414471799601657, -0.00130780159524470),
  dsp::Complex(-0.0342853542828136, 0.00308769607090894),
  dsp::Complex(0.0152598563883195, 0.0152615542290605),
  0.106841233812882,
  dsp::Complex(-0.0152598563883195, 0.0152615542290605),
  dsp::Complex(-0.0342853542828136, -0.00308769607090894),
  dsp::Complex(0.00414471799601657, -0.00130780159524470), },

  { 1.07838488548815,
  dsp::Complex(-0.0394941970606158, -0.0388894927032113),
  0.998186478509121,
  dsp::Complex(0.0394941970606158, -0.0388894927032113),
  dsp::Complex(-0.0374103834948442, 0.00765545389631506),
  dsp::Complex(-0.0755795876724971, -0.0353076305014837),
  0.507222386717422,
  dsp::Complex(0.0755795876724971, -0.0353076305014837),
  dsp::Complex(-0.0374103834948442, -0.00765545389631506),
  dsp::Complex(0.0411095458969911, 0.0491546186817065),
  dsp::Complex(-0.0416257687194416, 0.00144233306080249),
  dsp::Complex(-0.0597357730591135, -0.0255126272371865),
  0.141051073362225,
  dsp::Complex(0.0597357730591135, -0.0255126272371865),
  dsp::Complex(-0.0416257687194416, -0.00144233306080249),
  dsp::Complex(-0.0411095458969911, 0.0491546186817065) }
};



}  // namespace sal

#endif
