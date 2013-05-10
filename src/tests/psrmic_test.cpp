/*
 directivitypsr_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 93 $
 Last changed date:  $Date: 2012-06-07 09:58:54 +0100 (Thu, 07 Jun 2012) $
 */

#include "mcl.h"
#include "microphone.h"
#include "monomics.h"
#include "psrmic.h"
#include "stream.h"

namespace sat {
  
bool PSRMic::Test() {
  using mcl::IsEqual;
  Length sound_speed(343.0);
  
  PSRMic mic_a(Point(0.0,0.0,0.0), PI/2.0, 0.0, 0.0,
               0.0, 2.0*PI/5.0, sound_speed);
  MonoStream* stream_a = mic_a.stream();
  
  mic_a.RecordPlaneWave(0.5, Point(-1.0,0.0,0.0));
  assert(IsEqual(stream_a->Pull(), 0.0));
  mic_a.RecordPlaneWave(0.5, Point(0.0,1.0,0.0));
  assert(IsEqual(stream_a->Pull(), 0.0));
  
  mic_a.RecordPlaneWave(0.5, Point(cos(2.0*PI/5.0-VERY_SMALL),
                                   sin(2.0*PI/5.0-VERY_SMALL),
                                   0.0));
  assert(stream_a->Pull() >= 0.0);
  
  mic_a.RecordPlaneWave(0.5, Point(cos(2.0*PI/5.0-VERY_SMALL),
                                   -sin(2.0*PI/5.0-VERY_SMALL),
                                   0.0));
  assert(stream_a->Pull() >= 0.0);
  
  mic_a.RecordPlaneWave(0.5, Point(cos(2.0*PI/5.0+VERY_SMALL),
                                   sin(2.0*PI/5.0+VERY_SMALL),
                                   0.0));
  assert(IsEqual(stream_a->Pull(), 0));
  
  mic_a.RecordPlaneWave(0.5, Point(cos(2.0*PI/5.0+VERY_SMALL),
                                   -sin(2.0*PI/5.0+VERY_SMALL),
                                   0.0));
  assert(IsEqual(stream_a->Pull(), 0));
  
  Time max_ictd;
  Angle theta, phim, phimp1, beta;
  Sample eta_db, eta, phi, gamma;
  
  
  max_ictd = 0.0;
  phim = 0.0;
  phimp1 = 2.0*PI/5.0;
  eta_db = 13.0-16.2/(1.0+exp(-(max_ictd*1000-1.62)));
  eta = pow(10.0, -eta_db/20.0);
  beta = atan((eta*sin(phimp1-phim))/(1-eta*cos(phimp1-phim)));
  
  theta = 0.0;
  phi = sin(theta-(phim-beta))/sin((phimp1+beta)-theta);
  gamma = pow(1.0+pow(phi,2.0),-0.5);
  
  mic_a.RecordPlaneWave(1.0, Point(1.0, 0.0, 0.0));
  assert(IsEqual(stream_a->Pull(), gamma));
  
  mic_a.RecordPlaneWave(1.0, Point(1.0, 0.0, 0.0));
  assert(IsEqual(stream_a->Pull(), 0.956590742925106));
  
  theta = PI/10.0;
  phi = sin(theta-(phim-beta))/sin((phimp1+beta)-theta);
  gamma = pow(1.0+pow(phi,2.0),-0.5);
  
  mic_a.RecordPlaneWave(1.0, Point(cos(PI/10.0), sin(PI/10.0), 0.0));
  assert(IsEqual(stream_a->Pull(), gamma));
  
  mic_a.RecordPlaneWave(1.0, Point(cos(PI/10.0), sin(PI/10.0), 0.0));
  assert(IsEqual(stream_a->Pull(), 0.851784642051529));
  
  mic_a.RecordPlaneWave(0.5, Point(1.0, 0.0, 0.0));
  Sample sample_a = stream_a->Pull();
  mic_a.RecordPlaneWave(0.5, Point(cos(PI/5.0), sin(PI/5.0), 0.0));
  Sample sample_b = stream_a->Pull();
  assert(sample_a > sample_b);
  
  Length array_radius(0.155);
  PSRMic mic_b(Point(0.0,0.0,0.0), PI/2.0, 0.0, 0.0,
               array_radius, 2.0*PI/5.0, 343.0);
  MonoStream* stream_b = mic_b.stream();
  
  max_ictd = 2*array_radius/sound_speed * pow(sin(PI/5.0),2);
  eta_db = 13.0-16.2/(1.0+exp(-(max_ictd*1000-1.62)));
  eta = pow(10.0, -eta_db/20.0);
  beta = atan((eta*sin(phimp1-phim))/(1-eta*cos(phimp1-phim)));
  
  theta = 0.0;
  phi = sin(theta-(phim-beta))/sin((phimp1+beta)-theta);
  gamma = pow(1.0+pow(phi,2.0),-0.5);
  
  mic_b.RecordPlaneWave(1.0, Point(1.0, 0.0, 0.0));
  Sample sample = stream_b->Pull();
  assert(IsEqual(sample, gamma));
  
  mic_b.RecordPlaneWave(1.0, Point(1.0, 0.0, 0.0));
  assert(IsEqual(stream_b->Pull(), 0.948783470983758));
  
  theta = PI/10.0;
  phi = sin(theta-(phim-beta))/sin((phimp1+beta)-theta);
  gamma = pow(1.0+pow(phi,2.0),-0.5);
  
  mic_b.RecordPlaneWave(1.0, Point(cos(PI/10.0), sin(PI/10.0), 0.0));
  assert(IsEqual(stream_b->Pull(), gamma));
  
  mic_b.RecordPlaneWave(1.0, Point(cos(PI/10.0), sin(PI/10.0), 0.0));
  assert(IsEqual(stream_b->Pull(), 0.844459307421576));
  
  mic_b.RecordPlaneWave(1.0, Point(cos(PI/10.0), -sin(PI/10.0), 0.0));
  assert(IsEqual(stream_b->Pull(), 0.844459307421576));
  
  return true;
}
  
} // namespace sat

