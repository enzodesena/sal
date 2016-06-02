/*
 microphone_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "microphone.h"
#include "monomics.h"
#include "signal.h"
#include "kemarmic.h"
#include "sphericalheadmic.h"
#include "mcl.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {

bool Microphone::Test() {
  using mcl::IsEqual;
  
  //////////////////////////////////
  // OmniMic tests                //
  //////////////////////////////////
  
  OmniMic mic_a(Point(0.0,0.0,1.0), Quaternion::Identity());
  MonoStream* stream_a = mic_a.stream();
  
  assert(mic_a.position().Equals(Point(0.0,0.0,1.0)));
  mic_a.RecordPlaneWave(-1.0,Point(1.0,0.0,2.0));
  
  assert(IsEqual(stream_a->Pull(),-1.0));
  mic_a.RecordPlaneWave(2.0,Point(-1.0,0.0,-2.0));
  assert(IsEqual(stream_a->Pull(),2.0));
  
  
  //////////////////////////////////
  // TrigMic tests                //
  //////////////////////////////////
  std::vector<Sample> coefficients(3);
  coefficients[0] = 0.5;
  coefficients[1] = -1.0;
  coefficients[2] = -0.5;
  
  TrigMic mic_i(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/4.0), coefficients);
  MonoStream* stream_i = mic_i.stream();
  const Angle angle_i = PI/4.0;
  mic_i.RecordPlaneWave(0.5, Point(0.0,1.0,0.0));
  assert(IsEqual(stream_i->Pull(),
                 0.5*(0.5-1.0*cos(angle_i)-0.5*pow(cos(angle_i),2.0))));
  
  TrigMic mic_e(Point(1.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), coefficients);
  MonoStream* stream_e = mic_e.stream();
  const Angle angle_e = PI/4.0;
  mic_e.RecordPlaneWave(0.5, Point(0.0,1.0,0.0));
  assert(IsEqual(stream_e->Pull(),
                 0.5*(0.5-1.0*cos(angle_e)-0.5*pow(cos(angle_e),2.0))));
  
  TrigMic mic_f(Point(2.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), coefficients);
  MonoStream* stream_f = mic_f.stream();
  const Angle angle_f= PI/2.0-acos(2.0/sqrt(5.0));
  mic_f.RecordPlaneWave(0.5, Point(0.0,1.0,0.0));
  assert(IsEqual(stream_f->Pull(),
                 0.5*(0.5-1.0*cos(angle_f)-0.5*pow(cos(angle_f),2.0))));
  
  TrigMic mic_g(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(), coefficients);
  MonoStream* stream_g = mic_g.stream();
  mic_g.RecordPlaneWave(0.5, Point(1.0,0.0,0.0));
  assert(IsEqual(stream_g->Pull(), 0.5*(0.5-1.0-0.5)));
  
  TrigMic mic_h(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/5.0), coefficients);
  MonoStream* stream_h = mic_h.stream();
  mic_h.RecordPlaneWave(0.5, Point(2.0,2.0*tan(PI/5.0),0.0));
  assert(IsEqual(stream_h->Pull(), 0.5*(0.5-1.0-0.5)));
    
  
  //////////////////////////////////
  // GainMic tests                //
  //////////////////////////////////
  
  GainMic mic_v(Point(0.0,0.0,0.0), Quaternion::Identity(), 0.5);
  MonoStream* stream_v = mic_v.stream();
  mic_v.RecordPlaneWave(-1.0,Point(1.0,0.0,2.0));
  assert(IsEqual(stream_v->Pull(),-0.5));
  mic_v.RecordPlaneWave(-2.0,Point(1.0,3.0,2.0));
  assert(IsEqual(stream_v->Pull(),-1.0));
 
  return true;
}

} // namespace sal

