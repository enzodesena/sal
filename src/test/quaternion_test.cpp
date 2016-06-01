/*
 quaternion_test.cpp
 MCL
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "quaternion.h"
#include "comparisonop.h"
#include "mcltypes.h"
#include "point.h"
#include "vectorop.h"
#include <vector>

namespace mcl {
bool Quaternion::Test() {
  
  Quaternion q(0.1, 0.2, 0.3, 0.4);
  assert(IsEqual(q.w(), 0.1));
  assert(IsEqual(q.x(), 0.2));
  assert(IsEqual(q.y(), 0.3));
  assert(IsEqual(q.z(), 0.4));
  assert(IsEqual(q, Quaternion(0.1, 0.2, 0.3, 0.4)));
  
  Quaternion p(1.0, 0.0, 1.0, 0.0);
  Quaternion r(1.0, 0.5, 0.5, 0.75);
  assert(IsEqual(QuatMultiply(p, r), Quaternion(0.5, 1.25, 1.5, 0.25)));
  assert(! IsEqual(QuatMultiply(r, p), Quaternion(0.5, 1.25, 1.5, 0.25)));
  Quaternion s(2.0, 1.0, 0.1, 0.1);
  assert(IsEqual(QuatMultiply(p, s), Quaternion(1.9, 1.1, 2.1, -0.9)));
  assert(! IsEqual(QuatMultiply(s, p), Quaternion(1.9, 1.1, 2.1, -0.9)));
  
  // Quaternion multiplication is distributive but not commutative
  assert(IsEqual(QuatMultiply(QuatMultiply(q, p), r), QuatMultiply(q, QuatMultiply(p, r))));
  
  assert(IsEqual(QuatRotate(Quaternion(0.5,0.5,0.5,0.5), Point(1.0, 2.0, 3.0), right_handed),
                 Point(3.0, 1.0, 2.0)));
  assert(IsEqual(QuatRotate(p, Point(1.0,1.0,1.0), left_handed), Point(-1.0,1.0,1.0)));
  assert(IsEqual(QuatRotate(p, Point(2.0,3.0,4.0), left_handed), Point(-4.0,3.0,2.0)));
  assert(IsEqual(QuatRotate(Quaternion(1.0, 0.5, 0.3, 0.1), Point(1.0,1.0,1.0), left_handed),
                 Point(0.8519,1.4741,0.3185), 1.0E-3));
  
  assert(IsEqual(AxAng2Quat(1,0,0,PI/2.0),
                 Quaternion(sqrt(2.0)/2.0,sqrt(2.0)/2.0,0.0,0.0)));
  
  assert(IsEqual(QuatRotate(AxAng2Quat(0,1,0,-PI/2.0), Point(1.0,0.0,0.0), right_handed),
                 Point(0,0,1)));
  assert(IsEqual(QuatRotate(QuatConj(AxAng2Quat(0,1,0,PI/2.0)), Point(1.0,0.0,0.0), right_handed),
                 Point(0,0,1)));
  assert(IsEqual(QuatRotate(AxAng2Quat(0,1,0,PI/2.0), Point(1.0,0.0,0.0), left_handed),
                 Point(0,0,1)));
  assert(IsEqual(QuatRotate(AxAng2Quat(0,1,0,PI/2.0), Point(-1.0,0.0,0.0), right_handed),
                 Point(0,0,1)));
  assert(IsEqual(QuatRotate(AxAng2Quat(0,1,0,-PI/2.0), Point(-1.0,0.0,0.0), left_handed),
                 Point(0,0,1)));
  
  // Test identity quaternion
  assert(IsEqual(QuatRotate(Quaternion::Identity(), Point(1.0, 2.0, 3.0)), Point(1.0, 2.0, 3.0)));
  assert(IsEqual(QuatMultiply(Quaternion::Identity(), p), p));
  assert(IsEqual(QuatMultiply(Quaternion::Identity(), r), r));
  assert(IsEqual(QuatMultiply(Quaternion::Identity(), s), s));
  
  // Test QuatInverse
  assert(IsEqual(QuatInverse(Quaternion(1,0,1,0)), Quaternion(0.5,0,-0.5,0)));
  
  return true;
}
  
} // namespace mcl