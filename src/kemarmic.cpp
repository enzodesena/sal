/*
 directivityfunction.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "kemarmic.h"
#include "mcl.h"
#include "point.h"
#include "salconstants.h"
#include <string.h>

namespace sal {

  
KemarMic::KemarMic(Point position, Angle theta, Angle phi, Angle psi,
                   const std::string directory) :
          BinauralMic(position, theta, phi, psi) {
    
  hrtf_database_right_ = Load(right_ear, directory);
  hrtf_database_left_ = Load(left_ear, directory);
    
}
  

Array<Array<std::vector<Sample>, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS>
          KemarMic::Load(const Ear ear, const std::string directory) {
  
  const int num_elevations = NUM_ELEVATIONS;
  const Angle elevations[] = {-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90};
  const int num_measurements[] = {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
  
  Array<Array<std::vector<Sample>, MAX_NUM_AZIMUTHS>, NUM_ELEVATIONS>
          hrtf_database;
          
  for (UInt i=0; i<num_elevations; ++i) {
    Angle resolution = 360.0 / num_measurements[i];
    Angle elevation = elevations[i];
    UInt num_measurement = floor(num_measurements[i]/2.0)+1;
    
    for (UInt j=0; j<num_measurement; ++j) {
      Angle angle = round(j * resolution);
      
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
      
      size = size / 2; // Length in number of samples
      assert(size%2 == 0);
      assert((size/2)%2 == 0);
      
      for (UInt k=0; k<size; k+=2) {
        UInt ipsilateral_index = j;
        UInt contralateral_index = (UInt)
              ((((Int) num_measurements[i]) - ((Int) j)) % (Int) num_measurements[i]);
        
        if (ear == right_ear) {
          hrtf_database[i][ipsilateral_index].
                  push_back(data[k]/NORMALISING_VALUE);
          // In the two cases for azimuth = 0, and azimuth = 180 the signals at
          // left and right ears are equal.
          if (ipsilateral_index != contralateral_index) {
            hrtf_database[i][contralateral_index].
                    push_back(data[k+1]/NORMALISING_VALUE);
          }
        } else {
          hrtf_database[i][ipsilateral_index].
                    push_back(data[k+1]/NORMALISING_VALUE);
          if (ipsilateral_index != contralateral_index) {
            hrtf_database[i][contralateral_index].
                    push_back(data[k]/NORMALISING_VALUE);
          }
        }
      }
      
      delete[] data;
    }
  }
  
  return hrtf_database;
}
  


  
UInt KemarMic::FindElevationIndex(Angle elevation) {
  Int elevation_index = round(elevation/10) + 4;
  if (elevation_index < 0) {
    return 0;
  } else if (elevation_index > 13) {
    return 13; 
  } else {
    return (UInt) elevation_index;
  }
}

UInt KemarMic::FindAzimuthIndex(Angle azimuth,
                                    UInt elevation_index) {
  const UInt num_measurements[] =
          {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
  
  Angle angular_resolution = 360.0 /
          ((Angle) num_measurements[elevation_index]);
  UInt azimuth_index = (UInt) round(azimuth/angular_resolution);
  
  if (azimuth_index == num_measurements[elevation_index]) { azimuth_index = 0; }
  
  return azimuth_index;
}
  
  
} // namespace sal
