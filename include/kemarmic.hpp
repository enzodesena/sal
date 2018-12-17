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
#include "binauralmic.hpp"
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
  
  
  mcl::Vector<T>& GetBrir(
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

// Definition of static 
template<typename T>
typename KemarDirectivity<T>::KemarDatabase KemarDirectivity<T>::global_db_;


template<typename T>
KemarDirectivity<T>::KemarDirectivity(
  const Time sampling_frequency,
  const std::string directory,
  const size_t num_samples,
  const size_t update_length,
  const HeadRefOrientation reference_orientation) noexcept
  : DatabaseBinauralDirectivity<T>(reference_orientation, update_length)
{
  Time employed_sampling_frequency =
    (sampling_frequency > 33075.0)
      ? 44100.0
      : 22050.0;
  
  if (! mcl::IsApproximatelyEqual(sampling_frequency, 22050.0)
    && ! mcl::IsApproximatelyEqual(sampling_frequency, 44100.0))
  {
    mcl::Logger::GetInstance().LogError
    (
      "The sampling frequency (%f) is not supported for "
      "the Kemar mic. Using %f instead.",
      sampling_frequency,
      employed_sampling_frequency);
  }
  
  // Check caching
  if (
    global_db_.hrir_left.size() == 0)
  {
    LoadDatabase(directory, sampling_frequency, num_samples, global_db_);
    db_ptr_ = &global_db_;
  }
  else if( // i.e. if the global_db_ has been populated
    global_db_.sampling_frequency == employed_sampling_frequency &&
    global_db_.hrir_length == num_samples)
  {
    db_ptr_ = &global_db_;
  }
  else
  {
    LoadDatabase(directory, sampling_frequency, num_samples, local_db_);
    db_ptr_ = &local_db_;
  }
}
  

template<typename T>
Angle KemarDirectivity<T>::GetElevation(
  const Point& point) const noexcept
{
  Angle elevation = (asin((double)point.z())) / PI * 180.0;
  if (mcl::IsNan(elevation))
  {
    elevation = 0.0;
    mcl::Logger::GetInstance().LogError
    (
      "The elevation of a sound source "
      "appears to be NAN (possibily due to coincident sound source and "
      "observation point). Reverting to a %f elevation. ",
      elevation);
  }
  
  ASSERT((elevation >= (-90.0 - VERY_SMALL)) & (elevation <= (90.0 + VERY_SMALL)));
  
  return elevation;
}


template<typename T>
Angle KemarDirectivity<T>::GetAzimuth(
  const Point& point) const noexcept
{
  Angle azimuth;

  switch (BinauralDirectivity<T>::reference_orientation_)
  {
  case HeadRefOrientation::x_facing:
    azimuth = atan((double)point.y() / point.x()) / PI * 180.0;
    if (mcl::IsNan(azimuth))
    {
      azimuth = 0.0;
    } // Conventionally, if x=y=0 then azimuth is taken as 0
    if (point.x() < 0.0)
    {
      azimuth += 180.0;
    }
    break;
  case HeadRefOrientation::y_facing:
    azimuth = -atan((double)point.x() / point.y()) / PI * 180.0;
    if (mcl::IsNan(azimuth))
    {
      azimuth = 0.0;
    } // Conventionally, if x=y=0 then azimuth is taken as 0
    if (point.y() < 0.0)
    {
      azimuth += 180.0;
    }
    break;
  default:
    ASSERT(false);
    break;
  }

  azimuth = mcl::Mod(azimuth, 360.0);

  if (mcl::IsNan(azimuth))
  {
    azimuth = 0.0;
    mcl::Logger::GetInstance().LogError
    (
      "The azimuth of a sound source "
      "appears to be NAN (possibily due to coincident sound source and "
      "observation point). Reverting to a %f azimuth.",
      azimuth);
  }
  ASSERT((azimuth >= (0.0 - VERY_SMALL)) & (azimuth <= (360.0 + VERY_SMALL)));
  return azimuth;
}


template<typename T>
size_t KemarDirectivity<T>::GetElevationIndex(
  const Angle elevation) const noexcept
{
  Int elevation_index = mcl::RoundToInt(elevation / 10.0) + 4;
  return std::max<Int>(std::min<Int>(elevation_index, 13), 0);
}


template<typename T>
size_t KemarDirectivity<T>::GetAzimuthIndex(
  const Angle azimuth,
  const size_t elevation_index) const noexcept
{
  Angle angular_resolution = 360.0 /
    ((Angle)num_measurements_[elevation_index]);
  size_t azimuth_index = mcl::RoundToInt(azimuth / angular_resolution);

  return (azimuth_index == num_measurements_[elevation_index])
    ? 0
    : azimuth_index;
}


template<typename T>
mcl::Vector<T>& KemarDirectivity<T>::GetBrir(
  const Ear ear,
  const Point& point) noexcept
{
  // For forward looking direction, Azimuth = 0 and elevation =0
  Point norm_point = Normalized(point);
  size_t elevation_index = GetElevationIndex(GetElevation(norm_point));
  size_t azimuth_index = GetAzimuthIndex(GetAzimuth(norm_point), elevation_index);
  return (ear == Ear::kLeft)
    ? db_ptr_->hrir_left[elevation_index][azimuth_index]
    : db_ptr_->hrir_right[elevation_index][azimuth_index];
}


template<typename T>
mcl::Vector<mcl::Vector<Signal<T>>> KemarDirectivity<T>::Load(
  Ear ear,
  std::string directory)
{
  mcl::Vector<mcl::Vector<Signal<T>>> hrtf_database(NUM_ELEVATIONS_KEMAR);

  for (size_t i = 0; i < NUM_ELEVATIONS_KEMAR; ++i)
  {
    // Initialise vector
    hrtf_database[i] = mcl::Vector<Signal<T>>(num_measurements_[i]);

    Angle resolution = 360.0 / num_measurements_[i];
    Angle elevation = elevations_[i];
    size_t num_measurement = (size_t)floor(((Angle)num_measurements_[i]) / 2.0) + 1;

    for (size_t j = 0; j < num_measurement; ++j)
    {
      Angle angle = mcl::RoundToInt(j * resolution);

      std::ifstream file;
      file.open
      (
        GetFilePath(elevation, angle, directory),
        std::ios::in | std::ios::binary | std::ios::ate);
      if (! file.good())
      {
        mcl::Logger::GetInstance().LogErrorToCerr("Kemar lib not found.");
        ASSERT(false);
      }
      size_t size = (size_t)file.tellg();
      ASSERT(sizeof(short) == 2);
      short* data = new short[size / 2];

      for (size_t k = 0; k < size; k += sizeof(short))
      {
        file.seekg(k, std::ios::beg);
        short big_endian;
        file.read((char*)&big_endian, sizeof(short));
        short little_endian = (short)(((big_endian & 0xff) << 8) |
          ((big_endian & 0xff00) >> 8));
        data[k / 2] = little_endian;
      }

      file.close();

      size = size / 2; // Length in number of samples
      ASSERT(size % 2 == 0);
      ASSERT((size / 2) % 2 == 0);

      hrtf_database[i][j] = mcl::Vector<Sample>(size/2);
      
      for (size_t k = 0; k < size; k += 2)
      {
        size_t ipsilateral_index = j;
        size_t contralateral_index =
          (((Int)num_measurements_[i]) -
            ((Int)j)) % (Int)num_measurements_[i];
        
        if (ear == Ear::kRight)
        {
          std::swap(ipsilateral_index, contralateral_index);
        }
        
        hrtf_database[i][ipsilateral_index][k/2] = data[k] / NORMALISING_VALUE_KEMAR;
        hrtf_database[i][contralateral_index][k/2] = data[k + 1] / NORMALISING_VALUE_KEMAR;
      }
      delete[] data;
    }
  }

  return hrtf_database;
}


template<typename T>
void KemarDirectivity<T>::LoadDatabase(
  const std::string directory,
  const Time sampling_frequency,
  const size_t num_samples,
  KemarDatabase& kemar_db) noexcept
{
  kemar_db.sampling_frequency = sampling_frequency;
  kemar_db.hrir_length = num_samples;
  if (directory.length() == 0)
  { // Load embedded
    kemar_db.hrir_right = LoadEmbeddedData(Ear::kRight);
    kemar_db.hrir_left = LoadEmbeddedData(Ear::kLeft);
  }
  else
  {
    kemar_db.hrir_right = Load(Ear::kRight, directory);
    kemar_db.hrir_left = Load(Ear::kLeft, directory);
  }

  if (mcl::IsApproximatelyEqual(sampling_frequency, 22050.0))
  {
    DatabaseBinauralDirectivity<T>::FilterAll(mcl::Butter<T>(10, 0.001, 0.45), kemar_db.hrir_right, kemar_db.hrir_left);
    for (size_t i = 0; i < NUM_ELEVATIONS_KEMAR; ++i)
    {
      for (size_t j = 0; j < num_measurements_[i]; ++j)
      {
        kemar_db.hrir_right[i][j] = mcl::Downsample(kemar_db.hrir_right[i][j], 2);
        kemar_db.hrir_left[i][j] = mcl::Downsample(kemar_db.hrir_left[i][j], 2);
      }
    }
  }

  if (num_samples != kFullBrirLength)
  {
    for (size_t i = 0; i < NUM_ELEVATIONS_KEMAR; ++i)
    {
      for (size_t j = 0; j < num_measurements_[i]; ++j)
      {
        kemar_db.hrir_right[i][j] = mcl::ZeroPad<T>(kemar_db.hrir_right[i][j], num_samples);
        kemar_db.hrir_left[i][j] = mcl::ZeroPad<T>(kemar_db.hrir_left[i][j], num_samples);
      }
    }
  }
}


template<typename T>
std::string KemarDirectivity<T>::GetFilePath(
  Angle elevation,
  Angle angle,
  std::string directory) noexcept
{
  char file_name[1000];
  char directory_name[1000];
  char file_path[1000];
  sprintf(directory_name, "/elev%d/", (int)elevation);
  sprintf(file_name, "H%de%03da.dat", (int)elevation, (int)angle);
  strcpy(file_path, directory.c_str());
  strcat(file_path, directory_name);
  strcat(file_path, file_name);
  return std::string(file_path);
}


template<typename T>
bool KemarDirectivity<T>::IsDatabaseAvailable(
  std::string directory) noexcept
{
  for (size_t i = 0; i < NUM_ELEVATIONS_KEMAR; ++i)
  {
    Angle resolution = 360.0 / num_measurements_[i];
    Angle elevation = elevations_[i];
    size_t num_measurement = (size_t)floor(((Angle)num_measurements_[i]) / 2.0) + 1;

    for (size_t j = 0; j < num_measurement; ++j)
    {
      Angle angle = mcl::RoundToInt(j * resolution);
      std::ifstream file;
      file.open
      (
        GetFilePath(elevation, angle, directory),
        std::ios::in | std::ios::binary | std::ios::ate);
      if (! file.good())
      {
        return false;
      }
    }
  }
  return true;
}


template<typename T>
void KemarDirectivity<T>::PrintParsedDatabase(
  Ear ear,
  std::string directory,
  Int num_samples,
  std::string variable_name)
{
  std::vector<mcl::Vector<Signal<T>>> hrtf_database = Load(ear, directory);
  for (Int i = 0; i < (Int)hrtf_database.size(); ++i)
  {
    for (Int j = 0; j < (Int)hrtf_database[i].size(); ++j)
    {
      printf("{%d,%d,", (int)i, (int)j);
      for (Int sample_id = 0; sample_id < (Int)hrtf_database[i][j].size(); ++
           sample_id)
      {
        printf("%.4E", hrtf_database[i][j][sample_id]);
        if (sample_id < (Int)hrtf_database[i][j].size() - 1)
        {
          printf(",");
        }
      }
      printf("}, \n");
    }
  }
}


} // namespace sal

#include "kemarmicdata.hpp"

