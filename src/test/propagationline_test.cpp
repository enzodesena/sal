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
  
  return true;
}

} // namespace sal
