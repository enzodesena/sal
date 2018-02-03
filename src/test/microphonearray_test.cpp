/*
 microphonearray_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "microphonearray.h"
#include "salconstants.h"

namespace sal {

using mcl::Point;
  
bool MicrophoneArray::Test() {
  
  const UInt num_microphones(5);
  const Length array_radius(1.0);
  
  
  CircularTrig microphone_array_a(Point(0.0,0.0,1.5),
                                  mcl::Quaternion::Identity(),
                                  array_radius,
                                  num_microphones,
                                  0.0, // i.e. uniformly distributed in 2PI
                                  mcl::UnaryVector<Sample>(1.0));
  
  assert(mcl::IsEqual(microphone_array_a.position(), Point(0.0,0.0,1.5)));
  
  std::vector<MonoMic*> microphones_a = microphone_array_a.microphones();
  assert(microphones_a.size() == num_microphones);
  
  assert(microphones_a[0]->position().Equals(Point(1.0,0.0,1.5)));
  assert(microphones_a[1]->position().Equals(Point(1.0*cos(2*PI/5),
                                                   1.0*sin(2*PI/5),
                                                   1.5)));
  
  microphone_array_a.set_position(Point(1.0,0.0,1.5));
  assert(microphones_a[0]->position().Equals(Point(2.0,0.0,1.5)));
  assert(microphones_a[1]->position().Equals(Point(1.0+1.0*cos(2*PI/5),
                                                   0.0+1.0*sin(2*PI/5),
                                                   1.5)));
  
//  StereoMic stereo_mic(Point(0.2,0.0,1.5),
//                       mcl::Quaternion::Identity(),
//                       1.0, PI/4.0);
//
//  std::vector<MonoMic*> stereo_mics =  stereo_mic.microphones();
//  std::cout<<stereo_mics.size()<<std::endl;
//  assert(stereo_mics.size() == 2);
//  assert(mcl::IsEqual(stereo_mics[0]->position(), Point(0.2 + 1.0*cos(0), 1.0*sin(0), 1.5)));
//  assert(mcl::IsEqual(stereo_mics[1]->position(), Point(0.2 + 1.0*cos(PI/4.0), 1.0*sin(PI/4.0), 1.5)));
  
  return true;
}

} // namespace sal
