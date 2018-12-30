/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "salutilities.hpp"
#include "comparisonop.hpp"
#include <assert.h>

namespace sal
{
inline bool TripletHandlerTest()
{
  TripletHandler speed_limiter(Point(0, 0, 0));
  speed_limiter = Triplet(0, 0, 0);

  speed_limiter.SetMaxSpeed(1.0);
  speed_limiter.SetTargetValue(Point(1, 0, 0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(1, 0, 0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(2, 0, 0));
  speed_limiter.Update(1.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(2, 0, 0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(0, 0, 0));
  speed_limiter.Update(2.0);
  ASSERT(IsEqual(speed_limiter.value(), Point(0, 0, 0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetTargetValue(Point(2, 0, 0));
  speed_limiter.Update(1.0);
  ASSERT(mcl::IsApproximatelyEqual(speed_limiter.value(), Point(1, 0, 0)));
  ASSERT(!speed_limiter.HasReachedTarget());
  speed_limiter.Update(1.0);
  ASSERT(mcl::IsApproximatelyEqual(speed_limiter.value(), Point(2, 0, 0)));
  ASSERT(speed_limiter.HasReachedTarget());

  speed_limiter.SetMaxSpeed(std::numeric_limits<Speed>::infinity());
  speed_limiter.SetTargetValue(Point(0, 0, 0));
  ASSERT(!speed_limiter.HasReachedTarget());
  ASSERT(!mcl::IsApproximatelyEqual(speed_limiter.value(), Point(0, 0, 0)));
  speed_limiter.Update(0); // Even after 0 seconds,
  ASSERT(mcl::IsApproximatelyEqual(speed_limiter.value(), Point(0, 0, 0)));
  ASSERT(speed_limiter.HasReachedTarget());

  

  return true;
}
}
