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
using mcl::IsEqual;

namespace sal {
  
bool TripletHandler::Test() {

  TripletHandler speed_limiter(Point(0,0,0));
  speed_limiter.set_max_speed(1.0);
  speed_limiter.set_target_value(Point(1,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(1,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(2,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(2,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(0,0,0));
  speed_limiter.Update(2.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(0,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.set_target_value(Point(2,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(1,0,0)));
  ASSERT(!speed_limiter.HasReachedTarget());
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(2,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());
  
  speed_limiter.set_max_speed(std::numeric_limits<Speed>::infinity());
  speed_limiter.set_target_value(Point(0,0,0));
  ASSERT(!speed_limiter.HasReachedTarget());
  ASSERT(!IsEqual(speed_limiter.value(), Point(0,0,0)));
  speed_limiter.Update(0); // Even after 0 seconds,
  ASSERT(IsEqual(speed_limiter.value(), Point(0,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());
  
  // Test ramp smoother
  RampSmoother smoother(1.0,  // Initial value
                        1.0); // Sampling frequency
  assert(!smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 1.0));
  assert(IsEqual(smoother.GetNextValue(), 1.0));
  assert(IsEqual(smoother.GetNextValue(), 1.0));
  assert(!smoother.IsUpdating());
  
  smoother.set_target_value(3.0,
                            2.0);  // ramp time
  assert(smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 2.0));
  assert(smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 3.0));
  assert(!smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 3.0));
  
  smoother.set_target_value(1.0, 0.0); // Force to value
  assert(!smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 1.0));
  assert(!smoother.IsUpdating());
  
  Sample input_samples[4] = { 1.0, 1.0, 1.0, 1.0 };
  smoother.set_target_value(2.0,
                            2.0);  // ramp time
  Sample output_samples[5];
  Sample output_samples_cmp[5] = { 1.5, 2.0, 2.0, 2.0, };
  smoother.GetNextValuesAndMultiply(input_samples, 4, output_samples);
  assert(IsEqual(output_samples, output_samples_cmp, 4));
  
  return true;
}
  
}
