/*
 kemarmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "cipicmic.h"
#include "mcl.h"
#include "point.h"
#include "salconstants.h"
#include <string.h>

namespace sal {


CipicMic::CipicMic(Point position, Angle theta, Angle phi, Angle psi,
                   const std::string directory) :
BinauralMic(position, theta, phi, psi) {
  
  azimuths_ = std::vector<sal::Angle>({-80.0,-65.0,-55.0,-45.0,-40.0,-35.0,
    -30.0,-25.0,-20.0,-15.0,-10.0,-5.0, 0.0, 5.0, 10.0, 15.0, 20.0, 25.0,
    30.0, 35.0, 40.0, 45.0, 55.0, 65.0, 80.0});
  
  hrtf_database_right_ = Load(right_ear, directory);
  hrtf_database_left_ = Load(left_ear, directory);
}

std::vector<std::vector<Signal> >
CipicMic::Load(const Ear ear, const std::string directory) {
  std::vector<std::vector<Signal> > hrtf_database;
  
  for (UInt j=0; j<azimuths_.size(); ++j) {
    Int azimuth = azimuths_[j];
    
    std::string sign_text = (azimuth < 0) ? "neg" : "";
    std::string azimuth_text = std::to_string((azimuth > 0) ? azimuth:-azimuth);
    std::string ear_text = (ear == left_ear) ? "left" : "right";
    
    std::string file_name = sign_text + azimuth_text + "az" + ear_text + ".wav";
    std::string file_path = directory + "/" + file_name;
    
    std::ifstream file;
    
    file.open (file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (! file.good()) { throw "Cipic lib not found."; }
    file.close();
  
    std::vector<std::vector<sal::Sample> > brirs = WavHandler::Read(file_path);
    // For some reason I can't understand, the wav files contain the
    // BRIR across channels--there are 200 channels, one per sample;
    // there are 50 samples, one per elevation....
    mcl::Matrix<sal::Sample> brirs_matrix(brirs);
    mcl::Matrix<sal::Sample> transposed = mcl::Transpose(brirs_matrix);
    
    assert(transposed.data().size() == NUM_ELEVATIONS_CIPIC);
    
    hrtf_database.push_back(transposed.data());
  }
  
  return hrtf_database;
}


Signal CipicMic::GetBrir(const Ear ear, const Point& point) {
  // Calculate azimuth
  // For forward looking direction, Azimuth = 0 and elevation =0
  // "positive azimuth coresponds to moving right."
  Angle azimuth = (M_PI/2.0 -
            Point::AngleBetweenPoints(point, Point(0.0, -1.0, 0.0)))/M_PI*180.0;
  
  // Calculate elevation
  Point proj_xz = Point::Projection(point, Point(0.0, 1.0, 0.0));
  // "Positive evelation coresponds to moving up"
  Angle elevation;
  if (point.x() < 0.0) {
    elevation = proj_xz.theta()/M_PI*180.0;
  } else if (point.x() >= 0.0 && point.z() >= 0.0) {
    elevation = -proj_xz.theta()/M_PI*180.0;
  } else { // i.e. point.x() >= 0.0 && point.z() < 0.0
    elevation = 360.0 - proj_xz.theta()/M_PI*180.0;
  }
  
  if (isnan(elevation)) { elevation = 0.0; }
  if (isnan(azimuth)) { azimuth = 0.0; }
  
  assert(elevation >= (-90.0-VERY_SMALL) &
         elevation <= (270.0+VERY_SMALL));
  assert(azimuth >= (-90.0-VERY_SMALL) &
         azimuth <= (90.0+VERY_SMALL));
  
  UInt azimuth_index = mcl::MinIndex(mcl::Abs(mcl::Add(azimuths_,
                                                       -azimuth)));
  
  UInt elevation_index;
  if (elevation < -45.0) {
    elevation_index = 0;
  } else if (elevation > -45.0+360.0/64.0*49.0) {
    elevation_index = 49;
  } else {
    elevation_index = round((elevation + 45.0) * 64.0 / 360.0);
  }
  
  assert(azimuth_index >= 0 & azimuth_index < azimuths_.size());
  assert(elevation_index >= 0 & elevation_index <= 49);
  
  return (ear == left_ear) ?
          hrtf_database_left_[azimuth_index][elevation_index] :
          hrtf_database_right_[azimuth_index][elevation_index];
}
  
  
} // namespace sal
