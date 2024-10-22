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
                 const Int update_length,
                 const HeadRefOrientation reference_orientation) :
          BinauralMic(position, orientation, update_length, reference_orientation),
          hrtf_(NULL) {
  int err;
  // TODO: Still need to implement reference_orientation for orientations other than standard

  hrtf_ = mysofa_open(sofa_filename.c_str(), (int) sampling_frequency, &filter_length_, &err);
            
  if(hrtf_ == NULL) {
    mysofa_close(hrtf_);
    std::cout<<"Exiting with error:"<<err<<std::endl;
    ASSERT(false);
  }
}
  
  
  
Signal SofaMic::GetBrir(const Ear ear, const Point& point) noexcept {
  float left_impulse_response[filter_length_];
  float right_impulse_response[filter_length_];
  float left_delay;          // unit is samples
  float right_delay;         // unit is samples
  
  mysofa_getfilter_float(hrtf_,
                         (float) point.x(), (float) point.y(), (float) point.z(),
                         left_impulse_response, right_impulse_response,
                         &left_delay, &right_delay);
  
  ASSERT(left_delay == 0);
  ASSERT(right_delay == 0);
  
  // Create a vector of doubles
  Signal output_impulse_response(filter_length_);
  
  
  if (ear==kLeftEar) {
    std::transform(left_impulse_response, left_impulse_response + filter_length_,
                   output_impulse_response.begin(),
                   [](float value) { return static_cast<Sample>(value); });
  } else {
    std::transform(right_impulse_response, right_impulse_response + filter_length_,
                   output_impulse_response.begin(),
                   [](float value) { return static_cast<Sample>(value); });
  }
  
  return output_impulse_response;
}

SofaMic::~SofaMic() {
  mysofa_close(hrtf_);
}

} // namespace sal
