/*
 directivityspherical.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "sofamic.h"
#include "salconstants.h"
#include "vectorop.h"
#include <cmath>
#include "transformop.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {

SofaMic::SofaMic(const Point position,
                 const Quaternion orientation,
                 const std::string sofa_filename,
                 const Time sampling_frequency,
                 const Int update_length) :
          BinauralMic(position, orientation, update_length),
          hrtf_(NULL) {
            int filter_length;
            int err;
            struct MYSOFA_EASY *hrtf = NULL;

            hrtf = mysofa_open("/Users/enzodesena/repos/delete3/deleteme/Kemar_HRTF_sofa.sofa", 48000, &filter_length, &err);
            if(hrtf==NULL) {
              mysofa_close(hrtf);
              std::cout<<"Exiting with error:"<<err<<std::endl;
              ASSERT(false);
            }
//  int err;
//
//  hrtf_ = mysofa_open(sofa_filename.c_str(), (int) sampling_frequency, &filter_length_, &err);
//            
//  if(hrtf_ == NULL) {
//    mysofa_close(hrtf_);
//    std::cout<<"Exiting with error:"<<err<<std::endl;
//    ASSERT(false);
//  }
//  std::cout<<"Loaded SOFA file."<<std::endl;
}
  
  
  
Signal SofaMic::GetBrir(const Ear ear, const Point& point) noexcept {
  short left_impulse_response[filter_length_];
  short right_impulse_response[filter_length_];
  int left_delay;          // unit is samples
  int right_delay;         // unit is samples
  
  mysofa_getfilter_short(hrtf_, point.x(), point.y(), point.z(), left_impulse_response, right_impulse_response, &left_delay, &right_delay);
  
  ASSERT(left_delay == 0);
  ASSERT(right_delay == 0);
  
  // Create a vector of doubles
  Signal output_impulse_response(filter_length_);
  
  if (ear==kLeftEar) {
    std::transform(left_impulse_response, left_impulse_response + filter_length_,
                   output_impulse_response.begin(),
                   [](short value) { return static_cast<double>(value); });
  } else {
    std::transform(right_impulse_response, right_impulse_response + filter_length_,
                   output_impulse_response.begin(),
                   [](short value) { return static_cast<double>(value); });
  }
  
  return output_impulse_response;
}

SofaMic::~SofaMic() {
  mysofa_close(hrtf_);
}

} // namespace sal
