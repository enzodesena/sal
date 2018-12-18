/*
 kemarmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#pragma once

#define FULL_LENGTH_KEMAR 128
#define NUM_ELEVATIONS_KEMAR 14
#define NORMALISING_VALUE_KEMAR 30000.0

#include <map>
#include "vector.hpp"
#include "receiver.hpp"
#include "saltypes.hpp"
#include "binauraldir.hpp"
#include "salconstants.hpp"
#include "elementaryop.hpp"
#include "butter.hpp"

#ifdef _WIN32
#define sprintf(...) sprintf_s(__VA_ARGS__)
#define strcpy(...) strcpy_s(__VA_ARGS__)
#define strcat(...) strcat_s(__VA_ARGS__)
#endif


namespace sal
{
template<typename T>
class KemarDirectivity : public DatabaseBinauralDirectivity<T>
{
public:
  /** Constant meant to inform the directivity to use the entire length
  of the Kemar HRIR. */
  static const size_t kFullBrirLength = std::numeric_limits<size_t>::max();
  
  
  /** 
   Constructs a Kemar microphone opject. 
   @param[in] directory contains the hrtf database.
   @param[in] num_samples you can choose the length of the
   BRIR. If set to `kFullBrirLength` yields the entire BRIR.
   @param[int] update_length tells the object how many calls to
   `ReceiveAndAddToBuffer` it takes to switch to the new 
   */
  KemarDirectivity(
    const Time sampling_frequency,
    const std::string directory = "",
    const size_t num_samples = kFullBrirLength,
    const size_t update_length = 0,
    const HeadRefOrientation reference_orientation = HeadRefOrientation::x_facing) noexcept;


  static bool IsDatabaseAvailable(
    std::string directory) noexcept;


  static void PrintParsedDatabase(
    Ear ear,
    std::string directory,
    Int num_samples,
    std::string variable_name);
  

private:
  struct KemarDatabase
  {
    mcl::Vector<mcl::Vector<Signal<T>>> hrir_left;
    mcl::Vector<mcl::Vector<Signal<T>>> hrir_right;
    Time sampling_frequency;
    size_t hrir_length;
  };


  static constexpr Array<size_t, NUM_ELEVATIONS_KEMAR> num_measurements_ =
    {56, 60, 72, 72, 72, 72, 72, 60, 56, 45, 36, 24, 12, 1};
  static constexpr Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations_ =
    {-40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
  
  
  static KemarDatabase global_db_;
  KemarDatabase local_db_;
  KemarDatabase* db_ptr_;
  
  
  static mcl::Vector<mcl::Vector<Signal<T>>> LoadEmbeddedData(
    const Ear ear) noexcept;
  
  
  Angle GetElevation(
    const Point& point) const noexcept;
  
  
  Angle GetAzimuth(
    const Point& point) const noexcept;
  
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  size_t GetElevationIndex(
    const Angle elevation) const noexcept;


  /**
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  size_t GetAzimuthIndex(
    const Angle azimuth,
    const size_t elevation_index) const noexcept;
  
  
  mcl::Vector<T> GetBrir(
    const Ear ear,
    const Point& point) noexcept override;


  static
  mcl::Vector<mcl::Vector<Signal<T>>> Load(
    Ear ear,
    std::string directory);


  /**
  @param[in] sampling_frequency is assumed to be either 44100.0 or 22050.0
  @param[out] kemar_db this is where it will be loading the database. */
  static void LoadDatabase(
    const std::string directory,
    const Time sampling_frequency,
    const size_t num_samples,
    KemarDatabase& kemar_db) noexcept;


  static std::string GetFilePath(
    Angle elevation,
    Angle angle,
    std::string directory) noexcept;
};




} // namespace sal

#include "kemardir_impl.hpp"
#include "kemardir_data.hpp"

