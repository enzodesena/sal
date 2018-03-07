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
  
bool MicrophoneArrayTest() {
  
  const UInt num_microphones(5);
  const Length array_radius(1.0);
  
  
  TrigMic mic_prototype(Point(0,0,0),
                        mcl::Quaternion::Identity(),
                        mcl::UnaryVector<Sample>(1.0));
  
  CircularArray<TrigMic> microphone_array_a(Point(0.0,0.0,1.5),
                                            mcl::Quaternion::Identity(),
                                            mic_prototype,
                                            array_radius,
                                            CircularArray<TrigMic>::UniformAngles(num_microphones, 0));

  assert(mcl::IsEqual(microphone_array_a.position(), Point(0.0,0.0,1.5)));
  
  std::vector<TrigMic*> microphones_a = microphone_array_a.microphones();
  
  assert(microphones_a.size() == num_microphones);

  assert(microphones_a[0]->position().Equals(Point(1.0,0.0,1.5)));
  assert(microphones_a[1]->position().Equals(Point(1.0*cos(2.0*PI/5.0),
                                                   1.0*sin(2.0*PI/5.0),
                                                   1.5)));

  microphone_array_a.set_position(Point(1.0,0.0,1.5));
  assert(microphones_a[0]->position().Equals(Point(2.0,0.0,1.5)));
  assert(microphones_a[1]->position().Equals(Point(1.0+1.0*cos(2.0*PI/5.0),
                                                   0.0+1.0*sin(2.0*PI/5.0),
                                                   1.5)));
  
  StereoMic<TrigMic> stereo_mic(Point(0.2,0.0,1.5),
                                mcl::Quaternion::Identity(),
                                mic_prototype,
                                1.0, PI/4.0, +PI/8.0);

  std::vector<TrigMic*> stereo_mics =  stereo_mic.microphones();
  
  assert(stereo_mics.size() == 2);
  assert(mcl::IsEqual(stereo_mics[0]->position(), Point(0.2 + 1.0*cos(0), 1.0*sin(0), 1.5)));
  assert(mcl::IsEqual(stereo_mics[1]->position(), Point(0.2 + 1.0*cos(PI/4.0), 1.0*sin(PI/4.0), 1.5)));
  
  return true;
}

} // namespace sal

