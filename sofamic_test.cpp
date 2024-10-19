/*
 directivityspherical_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "sofamic.h"
#include "microphone.h"
#include "salconstants.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
bool SofaMic::Test() {
  Time sampling_frequency(48000);
  Length distance(2.0);
  
  MonoBuffer impulse(6);
  impulse.SetSample(0, 1.0);
  
  SofaMic mic_a(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                "/Users/enzodesena/repos/delete3/deleteme/Kemar_HRTF_sofa.sofa",
                sampling_frequency);
  StereoBuffer stream_a(impulse.num_samples());
  
  // Point in front of face.
  Point point_front(distance, 0.0, 0.0);
  
  mic_a.AddPlaneWave(impulse, point_front, stream_a);
  ASSERT(mcl::IsEqual(stream_a.GetLeftReadPointer(),
                 stream_a.GetRightReadPointer(), impulse.num_samples()));
  
  
  std::vector<Sample> output_frontal(6);
  output_frontal[0] = 0.5249943567937;
  output_frontal[1] = -0.39587582734155;
  output_frontal[2] = -0.61842280953724;
  output_frontal[3] = -0.13938281540835;
  output_frontal[4] = 0.093428452743537;
  output_frontal[5] = -0.12911852945215;
  
  ASSERT(mcl::IsEqual(stream_a.GetLeftReadPointer(), output_frontal));
  
  ASSERT(false);
  
  return true;
}
  
} // namespace sal
