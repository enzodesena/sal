///*
// kemarmic.cpp
// Spatial Audio Library (SAL)
// Copyright (c) 2015, Enzo De Sena
// All rights reserved.
// 
// Authors: Enzo De Sena, enzodesena@me.com
// 
// */
//
//#include "cipicmic.h"
//#include "mcl.h"
//#include "point.h"
//#include "salconstants.h"
//#include <string.h>
//
//namespace sal {
//
//
//CipicMic::CipicMic(Point position, Angle theta, Angle phi, Angle psi,
//                   const std::string directory) :
//BinauralMic(position, theta, phi, psi) {
//  
//  azimuths_ = std::vector<mcl::Int>({-80,-65,-55,-45,-40,-35,-30,-25,-20,-15,-10,-5,
//    0,5,10,15,20,25,30,35,40,45,55,65,80});
//  
//  hrtf_database_right_ = Load(right_ear, directory);
//  hrtf_database_left_ = Load(left_ear, directory);
//}
//
//std::vector<std::vector<Signal> >
//CipicMic::Load(const Ear ear, const std::string directory) {
//  std::vector<std::vector<Signal> > hrtf_database;
//  
//  for (UInt j=0; j<azimuths_.size(); ++j) {
//    // Initialise vector
//    hrtf_database.push_back(std::vector<Signal>(NUM_ELEVATIONS));
//    
//    Int azimuth = azimuths_[j];
//    
//    for (UInt i=0; i<NUM_ELEVATIONS; ++i) {
//      Angle angle = round(j * resolution);
//      
//      char file_name[1000];
//      char directory_name[1000];
//      char file_path[1000];
//      sprintf(directory_name, "/elev%d/", (int)elevation);
//      
//      sprintf(file_name, "H%de%03da.dat", (int)elevation, (int)angle);
//      
//      strcpy(file_path, directory.c_str());
//      strcat(file_path, directory_name);
//      strcat(file_path, file_name);
//      
//      std::ifstream file;
//      
//      file.open (file_path, std::ios::in | std::ios::binary | std::ios::ate);
//      if (! file.good()) { throw "Kemar lib not found."; }
//      long size = (long) file.tellg();
//      assert(sizeof(short) == 2);
//      short* data = new short[size/2];
//      
//      for (int k=0; k<size; k+=sizeof(short)) {
//        file.seekg (k, std::ios::beg);
//        short big_endian;
//        file.read ((char*)&big_endian, sizeof(short));
//        short little_endian(((big_endian & 0xff)<<8) |
//                            ((big_endian & 0xff00)>>8));
//        data[k/2] = little_endian;
//      }
//      
//      size = size / 2; // Length in number of samples
//      assert(size%2 == 0);
//      assert((size/2)%2 == 0);
//      
//      for (UInt k=0; k<size; k+=2) {
//        UInt ipsilateral_index = j;
//        UInt contralateral_index = (UInt)
//        ((((Int) num_measurements_[i]) -
//          ((Int) j)) % (Int) num_measurements_[i]);
//        
//        if (ear == right_ear) {
//          hrtf_database[i][ipsilateral_index].
//          push_back(data[k]/NORMALISING_VALUE);
//          // In the two cases for azimuth = 0, and azimuth = 180 the signals at
//          // left and right ears are equal.
//          if (ipsilateral_index != contralateral_index) {
//            hrtf_database[i][contralateral_index].
//            push_back(data[k+1]/NORMALISING_VALUE);
//          }
//        } else {
//          hrtf_database[i][ipsilateral_index].
//          push_back(data[k+1]/NORMALISING_VALUE);
//          if (ipsilateral_index != contralateral_index) {
//            hrtf_database[i][contralateral_index].
//            push_back(data[k]/NORMALISING_VALUE);
//          }
//        }
//      }
//      
//      delete[] data;
//    }
//  }
//  
//  return hrtf_database;
//}
//
//
//
//
//UInt CipicMic::FindElevationIndex(Angle elevation) {
//  Int elevation_index = round(elevation/10.0) + 4;
//  if (elevation_index < 0) {
//    return 0;
//  } else if (elevation_index > 13) {
//    return 13;
//  } else {
//    return (UInt) elevation_index;
//  }
//}
//
//UInt CipicMic::FindAzimuthIndex(Angle azimuth,
//                                UInt elevation_index) {
//  const UInt num_measurements[] =
//  {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
//  
//  Angle angular_resolution = 360.0 /
//  ((Angle) num_measurements[elevation_index]);
//  UInt azimuth_index = (UInt) round(azimuth/angular_resolution);
//  
//  if (azimuth_index == num_measurements[elevation_index]) { azimuth_index = 0; }
//  
//  return azimuth_index;
//}
//  
//  
//} // namespace sal
