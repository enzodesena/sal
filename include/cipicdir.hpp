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
#include "binauraldir.hpp"
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
} // namespace sal

#include "cipicdir.ipp"
