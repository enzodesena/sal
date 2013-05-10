/*
 propagationline_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 152 $
 Last changed date:  $Date: 2012-10-06 01:03:12 +0100 (Sat, 06 Oct 2012) $
 */


#include "propagationline.h"
#include "sattypes.h"
#include "mcl.h"
#include "satconstants.h"

using mcl::IsEqual;
using sat::Length;
using sat::Time;
using sat::Point;
using sat::Sample;


namespace sat {

bool PropagationLine::Test() {

  
  Time FS = 40000;
  Length distance = (Length) 3.0 * SOUND_SPEED/FS;
  
  Point point_a(0,0,0);
  Point point_b(distance,0,0);
  
  assert(IsEqual(Point::Distance(point_a, point_b), distance));
  
  Sample attenuation = (Sample) (SOUND_SPEED/FS) / (distance);
  Time delay = distance / SOUND_SPEED;
  Time latency = delay * FS;
  
  assert(IsEqual(latency, 3.0));
  
  PropagationLine propLine = PropagationLine(distance, FS);
  
  
  
  propLine.Write(1.0);
  assert(IsEqual(propLine.Read(),0.0));
  
  propLine.Tick();
  propLine.Write(2.0);
  assert(IsEqual(propLine.Read(),0.0));
  
  propLine.Tick();
  propLine.Write(3.0);
  assert(IsEqual(propLine.Read(),0.0));
  
  propLine.Tick();
  propLine.Write(-1.0);
  assert(IsEqual(propLine.Read(),1.0*attenuation));
  
  propLine.Tick();
  propLine.Write(-1.0);
  assert(IsEqual(propLine.Read(),2.0*attenuation));
  
  propLine.Tick();
  propLine.Write(-1.0);
  assert(IsEqual(propLine.Read(),3.0*attenuation));
  
  propLine.Tick();
  propLine.Write(-1.0);
  assert(IsEqual(propLine.Read(),-1.0*attenuation));

  return true;
}

} // namespace sat
