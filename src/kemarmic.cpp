/*
 kemarmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "kemarmic.h"
#include "point.h"
#include "salconstants.h"
#include "vectorop.h"
#include <fstream>

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
                   const Int num_samples,
                   const Int update_length,
                   const HeadRefOrientation reference_orientation) :
          DatabaseBinauralMic(position, orientation, update_length,
                              reference_orientation) {
            
  num_measurements_ = GetNumMeasurements();
  elevations_ = GetElevations();
            
  if (directory.length() == 0) {
    hrtf_database_right_ = LoadEmbedded(kRightEar, num_samples);
    hrtf_database_left_ = LoadEmbedded(kLeftEar, num_samples);
  } else {
    hrtf_database_right_ = Load(kRightEar, directory, num_samples);
    hrtf_database_left_ = Load(kLeftEar, directory, num_samples);
  }
}
  
Array<mcl::Int, NUM_ELEVATIONS_KEMAR> KemarMic::GetNumMeasurements() noexcept {
  return {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
}

Array<mcl::Int, NUM_ELEVATIONS_KEMAR> KemarMic::GetElevations() noexcept {
  return {-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90};
}
  
bool KemarMic::IsDatabaseAvailable(const std::string directory) {
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations = GetElevations();
  
  for (Int i=0; i<NUM_ELEVATIONS_KEMAR; ++i) {
    
    Angle resolution = 360.0 / num_measurements[i];
    Angle elevation = elevations[i];
    Int num_measurement = (UInt) floor(((Angle) num_measurements[i])/2.0)+1;
    
    for (Int j=0; j<num_measurement; ++j) {
      Angle angle = (Int) round(j * resolution);
      
      std::ifstream file;
      file.open (GetFilePath(elevation, angle, directory),
                 std::ios::in | std::ios::binary | std::ios::ate);
      if (! file.good()) {
        return false;
      }
    }
  }
  return true;
}

std::string KemarMic::GetFilePath(const Angle elevation, const Angle angle,
                                  const std::string directory) noexcept {
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
  
  
  //  sal::KemarMic::PrintParsedDatabase(sal::kRightEar, "pss/sal/hrtfs/kemar",
  //                                     sal::KemarMic::kFullBrirLength, "h");
  //  sal::KemarMic::PrintParsedDatabase(sal::kRightEar, "pss/sal/hrtfs/kemar",
  //                      sal::KemarMic::kFullBrirLength, "hr");
  
void
KemarMic::PrintParsedDatabase(const Ear ear, const std::string directory,
                              const Int num_samples, std::string variable_name) {
  std::vector<std::vector<Signal> > hrtf_database = KemarMic::Load(ear, directory, num_samples);
  
//  exit(0);
//  for (Int i=0; i<hrtf_database.size(); ++i) {
//    for (Int j=0; j<hrtf_database[i].size(); ++j) {
//      for (Int sample_id=0; sample_id<hrtf_database[i][j].size(); ++sample_id) {
//        printf("%s[%d][%d][%d]=%.4E;\n", variable_name.c_str(),
//               (int) i, (int) j, (int) sample_id,
//               hrtf_database[i][j][sample_id]);
//      }
//    }
//  }
//    for (Int i=0; i<(Int)hrtf_database.size(); ++i) {
//      for (Int j=0; j<(Int)hrtf_database[i].size(); ++j) {
//        printf("%s[%d][%d] = {", variable_name.c_str(), (int) i, (int) j);
//        for (Int sample_id=0; sample_id<(Int)hrtf_database[i][j].size(); ++sample_id) {
//          printf("%.4E", hrtf_database[i][j][sample_id]);
//          if (sample_id < (Int)hrtf_database[i][j].size()-1) { printf(","); }
//        }
//        printf("}; \n");
//      }
//    }
  for (Int i=0; i<(Int)hrtf_database.size(); ++i) {
    for (Int j=0; j<(Int)hrtf_database[i].size(); ++j) {
      printf("{%d,%d,", (int) i, (int) j);
      for (Int sample_id=0; sample_id<(Int)hrtf_database[i][j].size(); ++sample_id) {
        printf("%.4E", hrtf_database[i][j][sample_id]);
        if (sample_id < (Int)hrtf_database[i][j].size()-1) { printf(","); }
      }
      printf("}, \n");
    }
  }
}
  
std::vector<std::vector<Signal> > KemarMic::LoadEmbedded(const Ear ear,
                                                         const Int num_samples) {
  std::vector<std::vector<Signal> > hrtf_database;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();
  
  for (Int i=0; i<NUM_ELEVATIONS_KEMAR; ++i) {
    // Initialise vector
    hrtf_database.push_back(std::vector<Signal>(num_measurements[i]));
    for (Int j=0; j<num_measurements[i]; ++j) {
      hrtf_database[i].push_back(Signal(FULL_LENGTH_KEMAR));
    }
  }
  
  LoadEmbeddedData(ear, hrtf_database);
  
  if (num_samples != kFullBrirLength) {
    for (Int i=0; i<NUM_ELEVATIONS_KEMAR; ++i) {
      for (Int j=0; j<num_measurements[i]; ++j) {
        hrtf_database[i][j] = mcl::Subset(hrtf_database[i][j], 0, num_samples);
      }
    }
  }
  return hrtf_database;
}
  
  
  
std::vector<std::vector<Signal> >
  KemarMic::Load(const Ear ear, const std::string directory,
                 const Int num_samples) {
  std::vector<std::vector<Signal> > hrtf_database;
  
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations = GetElevations();
  
  for (Int i=0; i<NUM_ELEVATIONS_KEMAR; ++i) {
    // Initialise vector
    hrtf_database.push_back(std::vector<Signal>(num_measurements[i]));
    
    Angle resolution = 360.0 / num_measurements[i];
    Angle elevation = elevations[i];
    Int num_measurement = (UInt) floor(((Angle) num_measurements[i])/2.0)+1;
    
    for (Int j=0; j<num_measurement; ++j) {
      Angle angle = (Int) round(j * resolution);
      
      std::ifstream file;
      file.open (GetFilePath(elevation, angle, directory),
                 std::ios::in | std::ios::binary | std::ios::ate);
      if (! file.good()) {
        mcl::Logger::GetInstance().LogErrorToCerr("Kemar lib not found.");
        throw("Kemar lib not found.");
      }
      long size = (long) file.tellg();
      ASSERT(sizeof(short) == 2);
      short* data = new short[size/2];
      
      for (int k=0; k<size; k+=sizeof(short)) {
        file.seekg (k, std::ios::beg);
        short big_endian;
        file.read ((char*)&big_endian, sizeof(short));
        short little_endian = (short) (((big_endian & 0xff)<<8) |
                                      ((big_endian & 0xff00)>>8));
        data[k/2] = little_endian;
      }
      
      file.close();
      
      size = size / 2; // Length in number of samples
      ASSERT(size%2 == 0);
      ASSERT((size/2)%2 == 0);
      
      if (num_samples != kFullBrirLength) {
        size = num_samples;
      }
      
      for (Int k=0; k<size; k+=2) {
        Int ipsilateral_index = j;
        Int contralateral_index = (UInt)
                ((((Int) num_measurements[i]) -
                  ((Int) j)) % (Int) num_measurements[i]);
        
        if (ear == kRightEar) {
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
  


  
Int KemarMic::FindElevationIndex(Angle elevation) {
  Int elevation_index = (Int) round(elevation/10.0) + 4;
  if (elevation_index < 0) {
    return 0;
  } else if (elevation_index > 13) {
    return 13; 
  } else {
    return (UInt) elevation_index;
  }
}

Int KemarMic::FindAzimuthIndex(Angle azimuth, Int elevation_index) {
  const Int num_measurements[] =
          {56,60,72,72,72,72,72,60,56,45,36,24,12,1};
  
  Angle angular_resolution = 360.0 /
          ((Angle) num_measurements[elevation_index]);
  Int azimuth_index = (UInt) round(azimuth/angular_resolution);
  
  if (azimuth_index == num_measurements[elevation_index]) { azimuth_index = 0; }
  
  return azimuth_index;
}
  

Signal KemarMic::GetBrir(const Ear ear, const Point& point) noexcept {
  // For forward looking direction, Azimuth = 0 and elevation =0
  Point norm_point = Normalized(point);
  Angle elevation = (asin((double) norm_point.z())) / PI * 180.0;
  
  Angle azimuth;
  
  switch (reference_orientation_) {
    case standard:
      azimuth = atan((double) norm_point.y()/norm_point.x())/PI*180.0;
      if (mcl::IsNan(azimuth)) { azimuth = 0.0; } // Conventionally, if x=y=0 then azimuth is taken as 0
      if (norm_point.x() < 0.0) { azimuth += 180.0; }
      break;
    case y_z:
      azimuth = -atan((double) norm_point.x()/norm_point.y())/PI*180.0;
      if (mcl::IsNan(azimuth)) { azimuth = 0.0; } // Conventionally, if x=y=0 then azimuth is taken as 0
      if (norm_point.y() < 0.0) { azimuth += 180.0; }
      break;
    default:
      ASSERT(false);
      break;
  }
  
  
  azimuth = mcl::Mod(azimuth, 360.0);
  
  if (std::isnan(elevation)) {
    elevation = 0.0;
    mcl::Logger::GetInstance().LogError("The elevation of a sound source "
          "appears to be NAN (possibily due to coincident sound source and "
          "observation point). Reverting to a %f elevation. ", elevation);
  }
  
  if (std::isnan(azimuth)) {
    azimuth = 0.0;
    mcl::Logger::GetInstance().LogError("The azimuth of a sound source "
          "appears to be NAN (possibily due to coincident sound source and "
          "observation point). Reverting to a %f azimuth.", azimuth);
  }
    
  ASSERT((elevation >= (-90.0-VERY_SMALL)) & (elevation <= (90.0+VERY_SMALL)));
  ASSERT((azimuth >= (0.0-VERY_SMALL)) & (azimuth <= (360.0+VERY_SMALL)));
  
  Int elevation_index = FindElevationIndex(elevation);
  Int azimuth_index = FindAzimuthIndex(azimuth, elevation_index);
  
  if (ear == kLeftEar) {
    return hrtf_database_left_[elevation_index][azimuth_index];
  } else {
    return hrtf_database_right_[elevation_index][azimuth_index];
  }
}
  
} // namespace sal
