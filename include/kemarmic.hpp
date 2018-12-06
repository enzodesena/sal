/*
 kemarmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_KEMARMIC_H
#define SAL_KEMARMIC_H

#define FULL_LENGTH_KEMAR 128
#define NUM_ELEVATIONS_KEMAR 14
#define NORMALISING_VALUE_KEMAR 30000.0

#include <map>
#include "vector.hpp"
#include "microphone.hpp"
#include "saltypes.hpp"
#include "array.hpp"
#include "binauralmic.hpp"
#include "salconstants.hpp"

namespace sal
{
class KemarMic : public DatabaseBinauralMic
{
public:
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   With `num_samples` you can choose the length of the 
   BRIR. If set to zero yields the entire BRIR.
   */
  KemarMic(
    const mcl::Point& position,
    mcl::Quaternion orientation,
    std::string directory = "",
    Int num_samples = kFullBrirLength,
    Int update_length = 0,
    HeadRefOrientation reference_orientation = HeadRefOrientation::standard,
    Time sampling_frequency = 44100.0);

  static const int kFullBrirLength = -1;

  static bool IsDatabaseAvailable(
    std::string directory);

  static void PrintParsedDatabase(
    Ear ear,
    std::string directory,
    Int num_samples,
    std::string variable_name);

  static bool Test();
private:
  Signal GetBrir(
    Ear ear,
    const mcl::Point& point) noexcept override;

  static
  std::vector<mcl::Vector<Signal>> Load(
    Ear ear,
    std::string directory);

  static std::vector<mcl::Vector<Signal>> LoadEmbedded(
    Ear ear);

  static void LoadEmbeddedData(
    Ear ear,
    std::vector<mcl::Vector<Signal>>& data);

  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  Int FindElevationIndex(
    Angle elevation);

  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  Int FindAzimuthIndex(
    Angle azimuth,
    Int elevation_index);

  static Array<mcl::Int, NUM_ELEVATIONS_KEMAR> GetNumMeasurements() noexcept;
  static Array<mcl::Int, NUM_ELEVATIONS_KEMAR> GetElevations() noexcept;

  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements_;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations_;

  static std::string GetFilePath(
    Angle elevation,
    Angle angle,
    std::string directory) noexcept;
};
} // namespace sal

//#ifndef DO_NOT_LOAD_EMBEDDED_KEMAR
//  #include "kemarmicdata.hpp"
//#endif

#endif
