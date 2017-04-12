/*
 propagationline_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "propagationline.h"
#include "saltypes.h"
#include "mcl.h"
#include "salconstants.h"

using mcl::IsEqual;
using sal::Length;
using sal::Time;
using mcl::Point;
using sal::Sample;


namespace sal {

bool PropagationLine::Test() {

  
  Time FS = 40000;
  Length distance = (Length) 3.0 * SOUND_SPEED/FS;
  
  Point point_a(0,0,0);
  Point point_b(distance,0,0);
  
  assert(IsEqual(Distance(point_a, point_b), distance));
  
  Sample attenuation = (Sample) (SOUND_SPEED/FS) / (distance);
  Time delay = distance / SOUND_SPEED;
  Time latency = delay * FS;
  
  assert(IsEqual(latency, 3.0));
  
  PropagationLine prop_line_a = PropagationLine(distance, FS);
  
  
  
  prop_line_a.Write(1.0);
  assert(IsEqual(prop_line_a.Read(),0.0));
  
  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  assert(IsEqual(prop_line_a.Read(),0.0));
  
  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  assert(IsEqual(prop_line_a.Read(),0.0));
  
  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  assert(IsEqual(prop_line_a.Read(),1.0*attenuation));
  
  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  assert(IsEqual(prop_line_a.Read(),2.0*attenuation));
  
  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  assert(IsEqual(prop_line_a.Read(),3.0*attenuation));
  
  prop_line_a.Tick();
  prop_line_a.Write(-1.0);
  assert(IsEqual(prop_line_a.Read(),-1.0*attenuation));

  //
  prop_line_a.Reset();
  prop_line_a.set_distance((Length) 2.0 * SOUND_SPEED/FS);
  attenuation = 1.0 / 2.0;
  
  prop_line_a.Tick();
  prop_line_a.Write(1.0);
  assert(IsEqual(prop_line_a.Read(),0.0));
  
  prop_line_a.Tick();
  prop_line_a.Write(2.0);
  assert(IsEqual(prop_line_a.Read(),0.0));
  
  prop_line_a.Tick();
  prop_line_a.Write(3.0);
  assert(IsEqual(prop_line_a.Read(),1.0*attenuation));
  
//  FS = 40000;
//  
//  std::vector<sal::Sample> output;
//  
//  PropagationLine prop_line_b = PropagationLine(4.3, FS, 100.0, 1000);
//  //prop_line_b.set_gain(1.0);
//  for (UInt i=0; i<round(FS/10.0); ++i) {
//    Time t = ((sal::Time) i)/FS;
//    if (mcl::Mod((Int) i+1, (Int) round(FS/40.0))==0) {
//      prop_line_b.set_distance(prop_line_b.distance()-0.8);
//      //prop_line_b.set_gain(1.0);
//    }
//    
//    prop_line_b.Write(cos(2.0*PI*100.0*t));
//    output.push_back(prop_line_b.Read());
//    prop_line_b.Tick();
//  }
//  
//  mcl::Save(output, "/Users/enzodesena/Documents/MATLAB/vector.txt");
  
  return true;
}

} // namespace sal
