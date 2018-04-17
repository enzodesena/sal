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

using mcl::Point;
using mcl::Quaternion;

namespace sal {

bool Microphone::Test() {
  using mcl::IsEqual;
  
  //////////////////////////////////
  // OmniMic tests                //
  //////////////////////////////////
  
  OmniMic mic_a(Point(0.0,0.0,1.0));
  
  assert(mic_a.position().Equals(Point(0.0,0.0,1.0)));
  
  assert(IsEqual(mic_a.RecordPlaneWave(-1.0, Point(1.0,0.0,2.0)),-1.0));
  assert(IsEqual(mic_a.RecordPlaneWave(2.0, Point(-1.0,0.0,-2.0)), 2.0));
  
  
  //////////////////////////////////
  // TrigMic tests                //
  //////////////////////////////////
  std::vector<Sample> coefficients(3);
  coefficients[0] = 0.5;
  coefficients[1] = -1.0;
  coefficients[2] = -0.5;
  
  TrigMic mic_i(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/4.0), coefficients);
  
  const Angle angle_i = PI/4.0;
  assert(IsEqual(mic_i.RecordPlaneWave(0.5, Point(0.0,1.0,0.0)),
                 0.5*(0.5-1.0*cos(angle_i)-0.5*pow(cos(angle_i),2.0))));
  
  TrigMic mic_e(Point(1.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), coefficients);
  const Angle angle_e = PI/4.0;
  assert(IsEqual(mic_e.RecordPlaneWave(0.5, Point(0.0,1.0,0.0)),
                 0.5*(0.5-1.0*cos(angle_e)-0.5*pow(cos(angle_e),2.0))));
  
  TrigMic mic_f(Point(2.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), coefficients);
  const Angle angle_f= PI/2.0-acos(2.0/sqrt(5.0));
  assert(IsEqual(mic_f.RecordPlaneWave(0.5, Point(0.0,1.0,0.0)),
                 0.5*(0.5-1.0*cos(angle_f)-0.5*pow(cos(angle_f),2.0))));
  
  TrigMic mic_g(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(), coefficients);
  assert(IsEqual(mic_g.RecordPlaneWave(0.5, Point(1.0,0.0,0.0)),
                 0.5*(0.5-1.0-0.5)));
         
  TrigMic mic_h(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/5.0), coefficients);
  assert(IsEqual(mic_h.RecordPlaneWave(0.5, Point(2.0,2.0*tan(PI/5.0),0.0)),
                 0.5*(0.5-1.0-0.5)));
  
  
  TrigMic mic_p(Point(2.0,1.2,0.5), mcl::AxAng2Quat(0,0,1,0), coefficients);
  Sample sample = mic_p.RecordPlaneWave(0.5, Point(2.0+cos(0),1.2+sin(0),0.5+0.4));
  mic_p.set_orientation(mcl::AxAng2Quat(0,0,1,PI/10.0));
  assert(IsEqual(mic_p.RecordPlaneWave(0.5, Point(2.0+cos(PI/10.0),1.2+sin(PI/10.0),0.5+0.4)),
                 sample));
  
  //////////////////////////////////
  // GainMic tests                //
  //////////////////////////////////
  
  GainMic mic_v(Point(0.0,0.0,0.0), 0.5);
  assert(IsEqual(mic_v.RecordPlaneWave(-1.0,Point(1.0,0.0,2.0)),-0.5));
  assert(IsEqual(mic_v.RecordPlaneWave(-2.0,Point(1.0,3.0,2.0)),-1.0));
  
  return true;
}

} // namespace sal

