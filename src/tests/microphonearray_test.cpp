/*
 microphonearray_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "microphonearray.h"
#include "satconstants.h"

namespace sat {

  
bool MicrophoneArray::Test() {
  
  const UInt num_microphones = 5;
  
  CircularTrig microphone_array_a = CircularTrig(Point(0.0,0.0,1.5), 1.0,
                                     num_microphones,
                                     0.0,
                                     0.0, // i.e. uniformly distributed in 2PI
                                     mcl::UnaryVector<Sample>(1.0));
  
  std::vector<MonoMic*> microphones_a = microphone_array_a.microphones();
  assert(microphones_a.size() == num_microphones);
  
  assert(microphones_a[0]->position().Equals(Point(1.0,0.0,1.5)));
  assert(microphones_a[1]->position().Equals(Point(1.0*cos(2*PI/5),
                                                   1.0*sin(2*PI/5),
                                                   1.5)));
  
  
  CircularPSR microphone_array_b = CircularPSR(Point(1.0,0.0,1.5), 1.5,
                                     num_microphones,
                                     0.0,
                                     0.0, // i.e. uniformly distributed in 2PI
                                     343.0); // sound speed);
  
  std::vector<MonoMic*> microphones_b = microphone_array_b.microphones();
  assert(microphones_b[0]->position().Equals(Point(2.5,0.0,1.5)));
  assert(microphones_b[1]->position().Equals(Point(1.5*cos(2*PI/5)+1.0,
                                               1.5*sin(2*PI/5),
                                               1.5)));

  Point position(0.0,0.0,0.0);
  Length radius = 0.155;
  Angle midline_heading = 0.0;
  Angle base_angle = PI/3.0;
  Speed sound_speed = 343.0;
  StereoPSR microphone_array_c(position,
            radius,
            midline_heading,
            base_angle,
            sound_speed);
  //MultichannelStream* stream_c = (MultichannelStream*) microphone_array_c.stream();
    
  std::vector<MonoMic*> microphones_c = microphone_array_c.microphones();
  assert(microphones_c.size() == 2);
  // TODO: I had to reverse it. I am not sure why. Double check.
  assert(microphones_c[0]->position().Equals(Point(0.155*cos(PI/3.0/2.0),
                                                  -0.155*sin(PI/3.0/2.0),
                                                  0.0)));
  assert(microphones_c[1]->position().Equals(Point(0.155*cos(PI/3.0/2.0),
                                                  0.155*sin(PI/3.0/2.0),
                                                  0.0)));
  
  
  
  return true;
}

} // namespace sat
