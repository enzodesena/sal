/*
 sofamic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2024, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_SOFAMIC_H
#define SAL_SOFAMIC_H

#include "microphone.h"
#include "binauralmic.h"
#include "saltypes.h"
#include "salconstants.h"
#include "mysofa.h"

namespace sal {

/**
 This object implements the spherical head model proposed by Duda et al in the
 article "Range dependence of the response of a spherical head model"
 */
class SofaMic : public BinauralMic {
public:
  SofaMic(const mcl::Point position,
          const mcl::Quaternion orientation,
          const std::string sofa_filename,
          const Time sampling_frequency,
          const Int update_length = 0,
          const HeadRefOrientation reference_orientation = HeadRefOrientation::standard);
  
  static bool Test();
  static bool Test(const mcl::Point& source_point, const Signal& cmp_left, const Signal& cmp_right);
  virtual ~SofaMic();
private:
  
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept;
  
  int filter_length_;
  
  struct ::MYSOFA_EASY* hrtf_;
};
  
} // namespace sal

#endif
