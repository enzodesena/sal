/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

// This file contains definitions of matrix operations and classes

#ifndef MCL_QUATERNION_H
#define MCL_QUATERNION_H

#include "mcltypes.h"
#include "point.h"
#include "constants.h"
#include <cassert>

namespace mcl {
  
  
/** Enum describing the angles ordering convention for Euler angles */
enum EulerOrder {
  zxz, xyx, yzy, zyz, xzx, yxy,
  xyz, yzx, zxy, xzy, zyx, yxz
};
  
struct AxAng {
  Real x;
  Real y;
  Real z;
  Real angle;
};
  
/** Quaternion class */
class Quaternion {
public:
  /** Constructs a quaternion, with the first element being the scalar
   component and the following three forming the vector component */
  Quaternion(const Real w, const Real x, const Real y, const Real z) noexcept :
            w_(w), x_(x), y_(y), z_(z) {}
  
  Real w() const noexcept { return w_; }
  Real x() const noexcept { return x_; }
  Real y() const noexcept { return y_; }
  Real z() const noexcept { return z_; }
  
  /** Constructs a quaternion that is neutral to rotations, 
   i.e. a multiplicative identity quaternion */
  static Quaternion Identity();
  
  static bool Test();
  
private:
  // q = w + x*i + y*j + z*k where i² = j² = k² = i*j*k = -1
  Real w_;
  Real x_;
  Real y_;
  Real z_;
};
  
/** Returns a Quaternion using a given axis-angle representation */
Quaternion AxAng2Quat(const Real x, const Real y, const Real z,
                              const Real angle) noexcept;
  
AxAng Quat2AxAng(const Quaternion& q) noexcept;
  
Quaternion QuatConj(const Quaternion& q) noexcept;

/** Returns the norm of a quaternion (defined the same as the Eucledian 
 norm in R^4) */
Real Norm(const Quaternion& q) noexcept;
  
Point QuatRotate(const Quaternion& q, const Point& r,
                 const Handedness handedness = kRightHanded) noexcept;

/** Implements the (Hamilton) quaternion multiplication **/
Quaternion QuatMultiply(const Quaternion& q, const Quaternion& r) noexcept;
  
/** Converts Euler angles with a given convention to a Quaternion. 
 Each input angle corresponds to the associated ordering.
 E.g. for zyx convention (which is the default), the first angle is for a
 rotation around the z axis, second for y, etc.
 */
Quaternion Eul2Quat(const Real angle_1, const Real angle_2, const Real angle_3,
                    const EulerOrder order = zyx) noexcept;
  
/** Returns the Euler angle around the x-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulX(const Quaternion q,
                       const EulerOrder order = zyx) noexcept;
  
/** Returns the Euler angle around the y-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulY(const Quaternion q,
                       const EulerOrder order = zyx) noexcept;
  
/** Returns the Euler angle around the z-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulZ(const Quaternion q,
                       const EulerOrder order = zyx) noexcept;
  
Quaternion QuatInverse(const Quaternion q) noexcept;
  
} // namespace mcl

#endif
