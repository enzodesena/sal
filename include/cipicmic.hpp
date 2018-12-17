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



#ifndef SAL_CIPICMIC_H
#define SAL_CIPICMIC_H

#include <map>
#include "vector.hpp"
#include "microphone.hpp"
#include "quaternion.hpp"
#include "saltypes.hpp"
#include "array.hpp"
#include "binauralmic.hpp"
#include "salconstants.hpp"

#define NUM_ELEVATIONS_CIPIC 50
#define LENGTH_BRIR_CIPIC 200
#define NORMALISING_VALUE_CIPIC 1.0

namespace sal
{
class CipicMic : public DatabaseBinauralMic
{
public:

  enum DataType
  {
    txt,
    wav
  };


  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  CipicMic(
    const mcl::Point& position,
    const mcl::Quaternion& orientation,
    const std::string& directory,
    DataType data_type,
    Int update_length = 0);

  using BinauralMic::IsCoincident;
  using BinauralMic::num_channels;

  static bool Test();


  ~CipicMic()
  {
  }


private:
  static std::vector<mcl::Vector<Signal>> Load(
    Ear ear,
    const std::string& directory,
    DataType data_type,
    const mcl::Vector<Angle>& azimuths);

  Signal GetBrir(
    Ear ear,
    const mcl::Point& point) noexcept override;

  mcl::Vector<Angle> azimuths_;
};
} // namespace sal

#endif
