/*
 quaternion.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

// This file contains definitions of matrix operations and classes

#ifndef MCL_QUATERNION_H
#define MCL_QUATERNION_H

#include "mcltypes.h"
#include "point.h"
#include <cassert>

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {
  
/** Enum describing whether we are using a right handed or left handed
 reference system*/
enum Handedness {
  right_handed,
  left_handed
};
  
/** Enum describing the angles ordering convention for Euler angles */
enum EulerOrder {
  zxz, xyx, yzy, zyz, xzx, yxy,
  xyz, yzx, zxy, xzy, zyx, yxz
};
  
/** Quaternion class */
class MCL_API Quaternion {
public:
  /** Constructs a quaternion, with the first element being the scalar
   component and the following three forming the vector component */
  Quaternion(const Real w, const Real x, const Real y, const Real z) :
            w_(w), x_(x), y_(y), z_(z) {}
  
  Real w() const { return w_; }
  Real x() const { return x_; }
  Real y() const { return y_; }
  Real z() const { return z_; }
  
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
                      const Real angle);
  
Quaternion QuatConj(const Quaternion& q);

/** Returns the norm of a quaternion (defined the same as the Eucledian 
 norm in R^4) */
Real Norm(const Quaternion& q);
  
Point QuatRotate(const Quaternion& q, const Point& r,
                 const Handedness handedness = right_handed);

/** Implements the (Hamilton) quaternion multiplication **/
Quaternion QuatMultiply(const Quaternion& q, const Quaternion& r);
  
/** Converts Euler angles with a given convention to a Quaternion. 
 Each input angle corresponds to the associated ordering.
 E.g. for zyx convention (which is the default), the first angle is for a
 rotation around the z axis, second for y, etc.
 */
Quaternion Eul2Quat(const Real angle_1, const Real angle_2, const Real angle_3,
                    const EulerOrder order = zyx);
  
/** Returns the Euler angle around the x-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulX(const Quaternion q, const EulerOrder order = zyx);
  
/** Returns the Euler angle around the y-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulY(const Quaternion q, const EulerOrder order = zyx);
  
/** Returns the Euler angle around the z-axis associated to a given quaternion
 and for a given Euler rotation convention */
Real Quat2EulZ(const Quaternion q, const EulerOrder order = zyx);
  
Quaternion QuatInverse(const Quaternion q);
  
} // namespace mcl

#endif
