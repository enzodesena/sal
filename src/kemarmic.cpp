/*
 kemarmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "kemarmic.h"
#include "mcl.h"
#include "point.h"
#include "salconstants.h"

#ifdef _WIN32
  #define sprintf(...) sprintf_s(__VA_ARGS__)
  #define strcpy(...) strcpy_s(__VA_ARGS__)
  #define strcat(...) strcat_s(__VA_ARGS__)
#endif


using mcl::Point;
using mcl::Quaternion;

namespace sal {

KemarMic::KemarMic(const Point& position,
                   const Quaternion orientation,
                   const std::string directory,
                   const UInt num_samples,
                   const UInt update_length) :
          DatabaseBinauralMic(position, orientation, update_length) {
            
  num_measurements_ = {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
  elevations_ = {-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90};
            
  hrtf_database_right_ = Load(right_ear, directory, num_samples);
  hrtf_database_left_ = Load(left_ear, directory, num_samples);
}
  

std::vector<std::vector<Signal> >
  KemarMic::Load(const Ear ear, const std::string directory,
                 const UInt num_samples) {
  std::vector<std::vector<Signal> > hrtf_database;
          
  for (UInt i=0; i<NUM_ELEVATIONS_KEMAR; ++i) {
    // Initialise vector
    hrtf_database.push_back(std::vector<Signal>(num_measurements_[i]));
    
    Angle resolution = 360.0 / num_measurements_[i];
    Angle elevation = elevations_[i];
    UInt num_measurement = (UInt) floor(((Angle) num_measurements_[i])/2.0)+1;
    
    for (UInt j=0; j<num_measurement; ++j) {
      Angle angle = (Int) round(j * resolution);
      
      char file_name[1000];
      char directory_name[1000];
      char file_path[1000];
      sprintf(directory_name, "/elev%d/", (int)elevation);
      
      sprintf(file_name, "H%de%03da.dat", (int)elevation, (int)angle);
      
      strcpy(file_path, directory.c_str());
      strcat(file_path, directory_name);
      strcat(file_path, file_name);
      
      std::ifstream file;
      
      file.open (file_path, std::ios::in | std::ios::binary | std::ios::ate);
      if (! file.good()) { throw "Kemar lib not found."; }
      long size = (long) file.tellg();
      assert(sizeof(short) == 2);
      short* data = new short[size/2];
      
      for (int k=0; k<size; k+=sizeof(short)) {
        file.seekg (k, std::ios::beg);
        short big_endian;
        file.read ((char*)&big_endian, sizeof(short));
        short little_endian(((big_endian & 0xff)<<8) |
                            ((big_endian & 0xff00)>>8));
        data[k/2] = little_endian;
      }
      
      file.close();
      
      size = size / 2; // Length in number of samples
      assert(size%2 == 0);
      assert((size/2)%2 == 0);
      
      if (num_samples > 0) {
        size = num_samples;
      }
      
      for (Int k=0; k<size; k+=2) {
        UInt ipsilateral_index = j;
        UInt contralateral_index = (UInt)
                ((((Int) num_measurements_[i]) -
                  ((Int) j)) % (Int) num_measurements_[i]);
        
        if (ear == right_ear) {
          hrtf_database[i][ipsilateral_index].
                  push_back(data[k]/NORMALISING_VALUE_KEMAR);
          // In the two cases for azimuth = 0, and azimuth = 180 the signals at
          // left and right ears are equal.
          if (ipsilateral_index != contralateral_index) {
            hrtf_database[i][contralateral_index].
                    push_back(data[k+1]/NORMALISING_VALUE_KEMAR);
          }
        } else {
          hrtf_database[i][ipsilateral_index].
                    push_back(data[k+1]/NORMALISING_VALUE_KEMAR);
          if (ipsilateral_index != contralateral_index) {
            hrtf_database[i][contralateral_index].
                    push_back(data[k]/NORMALISING_VALUE_KEMAR);
          }
        }
      }
      
      delete[] data;
    }
  }
  
  return hrtf_database;
}
  


  
UInt KemarMic::FindElevationIndex(Angle elevation) {
  Int elevation_index = (Int) round(elevation/10.0) + 4;
  if (elevation_index < 0) {
    return 0;
  } else if (elevation_index > 13) {
    return 13; 
  } else {
    return (UInt) elevation_index;
  }
}

UInt KemarMic::FindAzimuthIndex(Angle azimuth, UInt elevation_index) {
  const UInt num_measurements[] =
          {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
  
  Angle angular_resolution = 360.0 /
          ((Angle) num_measurements[elevation_index]);
  UInt azimuth_index = (UInt) round(azimuth/angular_resolution);
  
  if (azimuth_index == num_measurements[elevation_index]) { azimuth_index = 0; }
  
  return azimuth_index;
}
  

Signal KemarMic::GetBrir(const Ear ear, const Point& point) {
  
  // For forward looking direction, Azimuth = 0 and elevation =0
  Point norm_point = Normalized(point);
  Angle elevation = (asin((double) norm_point.z())) / PI * 180.0;
  
  Angle azimuth;
  if (norm_point.z() >= 0.0) { azimuth = (asin((double) norm_point.y())) / PI * 180.0; }
  else azimuth = (acos((double) norm_point.y())+PI/2.0) / PI * 180.0;
      
  azimuth = mcl::Mod(azimuth, 360.0);
  
  assert((elevation >= (-90.0-VERY_SMALL)) & (elevation <= (90.0+VERY_SMALL)));
  assert((azimuth >= (0.0-VERY_SMALL)) & (azimuth <= (360.0+VERY_SMALL)));
  
  UInt elevation_index = FindElevationIndex(elevation);
  UInt azimuth_index = FindAzimuthIndex(azimuth, elevation_index);
  
  if (ear == left_ear) {
    return hrtf_database_left_[elevation_index][azimuth_index];
  } else {
    return hrtf_database_right_[elevation_index][azimuth_index];
  }
}
  
} // namespace sal
