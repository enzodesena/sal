/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "salutilities.h"
#include "comparisonop.h"
#include <assert.h>

using mcl::Point;

namespace sal {
  
bool TripletHandler::Test() {

  TripletHandler speed_limiter(Point(0,0,0));
  speed_limiter.set_max_speed(1.0);
  speed_limiter.set_target_value(Point(1,0,0));
  speed_limiter.Update(1.0);
  assert(IsEqual(speed_limiter.value(), Point(1,0,0)));
  assert(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(2,0,0));
  speed_limiter.Update(1.0);
  assert(IsEqual(speed_limiter.value(), Point(2,0,0)));
  assert(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(0,0,0));
  speed_limiter.Update(2.0);
  assert(IsEqual(speed_limiter.value(), Point(0,0,0)));
  assert(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(2,0,0));
  speed_limiter.Update(1.0);
  assert(IsEqual(speed_limiter.value(), Point(1,0,0)));
  assert(!speed_limiter.HasReachedTarget());
  speed_limiter.Update(1.0);
  assert(IsEqual(speed_limiter.value(), Point(2,0,0)));
  assert(speed_limiter.HasReachedTarget());
  
  speed_limiter.set_max_speed(std::numeric_limits<Speed>::infinity());
  speed_limiter.set_target_value(Point(0,0,0));
  assert(!speed_limiter.HasReachedTarget());
  assert(!IsEqual(speed_limiter.value(), Point(0,0,0)));
  speed_limiter.Update(0); // Even after 0 seconds,
  assert(IsEqual(speed_limiter.value(), Point(0,0,0)));
  assert(speed_limiter.HasReachedTarget());
  
  
  
  
  
  return true;
}
  
}
