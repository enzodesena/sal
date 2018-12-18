/*
 cipicmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

// To convert the CIPIC database to text, please run the following in Matlab
// in the directory where you see the various 'subject_021' etc

//azimuths = [-80,-65,-55,-45:5:45, 55, 65, 80];
//listing = dir;
//for i = 1:numel(listing)
//listing_name = listing(i).name;
//if listing_name == '.'
//continue
//end
//if ~isempty(strfind(listing_name, 'show_data')) | ...
//~isempty(strfind(listing_name, 'convertToText.m')) | ...
//~isempty(strfind(listing_name, '.DS_Store'))
//continue
//end
//
//load(strcat(listing_name, '/hrir_final.mat'));
//
//for j=1:25
//azimuth = azimuths(j);
//if azimuth < 0; sign_text = 'neg'; else sign_text = ''; end
//
//dlmwrite(strcat(listing_name, '/', sign_text, num2str(abs(azimuth)), 'azleft.txt'), ...
//         squeeze(hrir_l(j, :, :)), ...
//         'delimiter','\t');
//dlmwrite(strcat(listing_name, '/', sign_text, num2str(abs(azimuth)), 'azright.txt'), ...
//         squeeze(hrir_r(j, :, :)), ...
//         'delimiter','\t');
//end
//end



#pragma once

#include <map>
#include "vector.hpp"
#include "receiver.hpp"
#include "quaternion.hpp"
#include "saltypes.hpp"
#include "saltypes.hpp"
#include "binauralmic.hpp"
#include "salconstants.hpp"
#include "salutilities.hpp"
#include "matrixop.hpp"

#ifndef NO_WAV_HANDLER
#include "wavhandler.hpp"
#endif

#define NUM_ELEVATIONS_CIPIC 50
#define NUM_AZIMUTHS_CIPIC 25
#define LENGTH_BRIR_CIPIC 200
#define NORMALISING_VALUE_CIPIC 1.0

namespace sal
{
template<typename T>
class CipicDirectivity : public DatabaseBinauralDirectivity<T>
{
public:

  enum class DataType
  {
    kTxt,
    kWav
  };


  /**
   Constructs a Cipic microphone opject.
   `directory` contains the hrtf database.
   */
  CipicDirectivity(
    const std::string& directory,
    const DataType data_type,
    const size_t update_length = 0,
    const HeadRefOrientation reference_orientation = HeadRefOrientation::x_facing);


private:
  static std::vector<mcl::Vector<Signal<T>>> Load(
    Ear ear,
    const std::string& directory,
    DataType data_type,
    const mcl::Vector<Angle>& azimuths);

  Signal<T> GetBrir(
    const Ear ear,
    const Point& point) noexcept override;

  mcl::Vector<Angle> azimuths_ = {
      -80.0, -65.0, -55.0, -45.0, -40.0, -35.0,
      -30.0, -25.0, -20.0, -15.0, -10.0, -5.0, 0.0, 5.0, 10.0, 15.0, 20.0, 25.0,
      30.0, 35.0, 40.0, 45.0, 55.0, 65.0, 80.0};
  
  static std::vector<mcl::Vector<Signal<T>>> hrir_left_;
  static std::vector<mcl::Vector<Signal<T>>> hrir_right_;
};

// Definitions
template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::hrir_left_;
template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::hrir_right_;

template<typename T>
CipicDirectivity<T>::CipicDirectivity(
  const std::string& directory,
  const DataType data_type,
  const size_t update_length,
  const HeadRefOrientation reference_orientation)
  : DatabaseBinauralDirectivity<T>(update_length, reference_orientation)
{
  if (hrir_left_.size() == 0)
  {
    hrir_right_ = Load(Ear::kRight, directory, data_type, azimuths_);
    hrir_left_ = Load(Ear::kLeft, directory, data_type, azimuths_);
  }
}


template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::Load(
  const Ear ear,
  const std::string& directory,
  const DataType data_type,
  const mcl::Vector<sal::Angle>& azimuths)
{
  std::vector<mcl::Vector<Signal<T>>> hrtf_database;

  for (Int j = 0; j < (Int)azimuths.size(); ++j)
  {
    Int azimuth = (Int)azimuths[j];

    std::string sign_text = (azimuth < 0) ? "neg" : "";
    std::string azimuth_text = ToString((azimuth > 0) ? azimuth : -azimuth);
    std::string ear_text = (ear == Ear::kLeft) ? "left" : "right";
    std::string data_type_text = (data_type == DataType::kWav) ? ".wav" : ".txt";
    std::string file_name =
      sign_text + azimuth_text + "az" + ear_text + data_type_text;
    std::string file_path = directory + "/" + file_name;

    std::ifstream file;
    file.open(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (! file.good())
    {
      mcl::Logger::GetInstance().LogErrorToCerr("Cipic lib not found.");
      ASSERT(false);
    }
    file.close();

    mcl::Vector<mcl::Vector<sal::Sample>> brirs;
    switch (data_type)
    {
#ifdef __x86_64__
#ifndef IOSARM
#ifndef NO_WAV_HANDLER
      case DataType::kWav:
        // For some reason I can't understand, the wav files contain the
      // BRIR across channels--there are 200 channels, one per sample;
        // there are 50 samples, one per elevation....
        brirs =
        mcl::Transpose(mcl::Matrix<sal::Sample>(WavHandler<T>::Read(file_path))).data();
        break;
#endif
#endif
#endif

    case DataType::kTxt:
      brirs = mcl::Matrix<sal::Sample>::Load(file_path).data();
      break;
    default:
      ASSERT(false);
      break;
    }

    ASSERT(brirs.size() == NUM_ELEVATIONS_CIPIC);
    ASSERT(brirs[0].size() == LENGTH_BRIR_CIPIC);
    hrtf_database.push_back(brirs);
  }

  return hrtf_database;
}


template<typename T>
Signal<T> CipicDirectivity<T>::GetBrir(
  const Ear ear,
  const Point& point) noexcept
{
  // Calculate azimuth
  // For forward looking direction, Azimuth = 0 and elevation =0
  // "positive azimuth coresponds to moving right."
  Angle azimuth = (PI / 2.0 -
    AngleBetweenPoints(point, Point(0.0, -1.0, 0.0))) / PI * 180.0;

  // Calculate elevation
  Point proj_xz = Projection(point, Point(0.0, 1.0, 0.0));
  // "Positive elevation coresponds to moving up"
  Angle elevation;
  if (point.x() >= 0.0)
  {
    elevation = 90.0 - proj_xz.theta() / PI * 180.0;
  }
  else
  {
    elevation = 90.0 + proj_xz.theta() / PI * 180.0;
  }

  if (isnan(elevation))
  {
    elevation = 0.0;
  }
  if (isnan(azimuth))
  {
    azimuth = 0.0;
  }

  ASSERT
  (
    (elevation >= (-90.0 - VERY_SMALL)) &
    (elevation <= (270.0 + VERY_SMALL)));
  ASSERT
  (
    (azimuth >= (-90.0 - VERY_SMALL)) &
    (azimuth <= (90.0 + VERY_SMALL)));

  size_t azimuth_index = mcl::MinIndex
  (
    mcl::Abs
    (
      mcl::Add
      (
        azimuths_,
        -azimuth)));

  Int elevation_index;
  if (elevation < -45.0)
  {
    elevation_index = 0;
  }
  else if (elevation > -45.0 + 360.0 / 64.0 * 49.0)
  {
    elevation_index = 49;
  }
  else
  {
    elevation_index = mcl::RoundToInt((elevation + 45.0) * 64.0 / 360.0);
  }

  ASSERT((azimuth_index >= 0) & (azimuth_index < azimuths_.size()));
  ASSERT((elevation_index >= 0) & (elevation_index <= 49));

  return (ear == Ear::kLeft) ?
           hrir_left_[azimuth_index][elevation_index] :
           hrir_right_[azimuth_index][elevation_index];
}




} // namespace sal
