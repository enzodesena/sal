/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "salutilities.h"
#include "comparisonop.h"
#include <assert.h>

using mcl::Point;
using mcl::IsEqual;

namespace sal {
  
bool TripletHandler::Test() {

  TripletHandler speed_limiter(Point(0,0,0));
  speed_limiter = Triplet(0,0,0);
  
  speed_limiter.SetMaxSpeed(1.0);
  speed_limiter.SetTargetValue(Point(1,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(1,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(2,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(2,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(0,0,0));
  speed_limiter.Update(2.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(0,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(2,0,0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(1,0,0)));
  ASSERT(!speed_limiter.HasReachedTarget());
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(2,0,0)));
  ASSERT(speed_limiter.HasReachedTarget());
  
  speed_limiter.SetMaxSpeed(std::numeric_limits<Speed>::infinity());
  speed_limiter.SetTargetValue(Point(0,0,0));
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
  
  smoother.SetTargetValue(3.0, 2.0);  // ramp time
  assert(smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 2.0));
  assert(smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 3.0));
  assert(!smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 3.0));
  
  smoother.SetTargetValue(1.0, 0.0); // Force to value
  assert(!smoother.IsUpdating());
  assert(IsEqual(smoother.GetNextValue(), 1.0));
  assert(!smoother.IsUpdating());
  
  Sample input_samples[4] = { 1.0, 1.0, 1.0, 1.0 };
  smoother.SetTargetValue(2.0,
                            2.0);  // ramp time
  Sample output_samples[5];
  Sample output_samples_cmp[5] = { 1.5, 2.0, 2.0, 2.0, };
  smoother.GetNextValuesMultiply(input_samples, output_samples);
  assert(IsEqual(output_samples, output_samples_cmp, 4));
  
  
  smoother.SetTargetValue(15000.0, 6.0);  // ramp time
  RampSmoother smoother_copy(smoother);
  ASSERT(IsEqual(smoother.GetNextValue(), smoother_copy.GetNextValue(1)));
  smoother.GetNextValue();
  smoother.GetNextValue();
  ASSERT(IsEqual(smoother.GetNextValue(), smoother_copy.GetNextValue(3)));
  
  
  return true;
}
  
}
