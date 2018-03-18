/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "quaternion.h"
#include "exception.h"
#include "elementaryop.h"
#include "comparisonop.h"


namespace mcl {

Quaternion QuatConj(const Quaternion& q) {
  return Quaternion(q.w(), -q.x(), -q.y(), -q.z());
}

Real Norm(const Quaternion& q) {
  return sqrt(pow(q.w(),2.0)+pow(q.x(),2.0)+pow(q.y(),2.0)+pow(q.z(),2.0));
}

Point QuatRotate(const Quaternion& q, const Point& r,
                 const Handedness handedness) {
  Real norm = Norm(q);
  Quaternion q_norm(q.w()/norm, q.x()/norm, q.y()/norm, q.z()/norm);
  
  Quaternion p(0.0, r.x(), r.y(), r.z());
  
  if (handedness == right_handed) {
    Quaternion result = QuatMultiply(QuatMultiply(q_norm, p), QuatConj(q_norm));
    return Point(result.x(), result.y(), result.z());
  } else {
    Quaternion result = QuatMultiply(QuatMultiply(QuatConj(q_norm), p), q_norm);
    return Point(result.x(), result.y(), result.z());
  }
}
  
Quaternion QuatMultiply(const Quaternion& q, const Quaternion& r) {
  return Quaternion(r.w()*q.w()-r.x()*q.x()-r.y()*q.y()-r.z()*q.z(),
                    r.w()*q.x()+r.x()*q.w()-r.y()*q.z()+r.z()*q.y(),
                    r.w()*q.y()+r.x()*q.z()+r.y()*q.w()-r.z()*q.x(),
                    r.w()*q.z()-r.x()*q.y()+r.y()*q.x()+r.z()*q.w());
}

Quaternion AxAng2Quat(const Real x, const Real y, const Real z,
                      const Real angle) {
  Real norm = sqrt(pow(x, 2.0)+pow(y, 2.0)+pow(z, 2.0));
  return Quaternion(cos(angle/2.0),
                    sin(angle/2.0)*x/norm,
                    sin(angle/2.0)*y/norm,
                    sin(angle/2.0)*z/norm);
}
  
AxAng Quat2AxAng(const Quaternion& input) {
  AxAng output;
  
  Quaternion q = (Norm(input)>1.0) ? Quaternion(input.w()/Norm(input),
                                                input.x()/Norm(input),
                                                input.y()/Norm(input),
                                                input.z()/Norm(input)) : input;
  
  Point q_r(q.x(), q.y(), q.z());
  if (IsEqual(q_r.norm(), 0.0)) {
    output.x = 0;
    output.y = 0;
    output.z = 1;
    output.angle = 0;
    return output;
  }
  
  Real n_inv = 1.0/Sqrt(1.0-q.w()*q.w());
  output.angle = 2.0*acos(q.w());
  output.x = q.x()*n_inv;
  output.y = q.y()*n_inv;
  output.z = q.z()*n_inv;
  return output;
}
  
  
Quaternion Eul2Quat(const Real angle_1, const Real angle_2, const Real angle_3,
                    const EulerOrder order) {
  Quaternion rotation_1 = Quaternion::Identity();
  Quaternion rotation_2 = Quaternion::Identity();
  Quaternion rotation_3 = Quaternion::Identity();

  switch (order) {
    case xyx: case xzx: case xyz: case xzy:
      rotation_1 = Quaternion(cos(angle_1/2.0), sin(angle_1/2.0), 0.0, 0.0);
      break;
    case yzy: case yxy: case yzx: case yxz:
      rotation_1 = Quaternion(cos(angle_1/2.0), 0.0, sin(angle_1/2.0), 0.0);
      break;
    case zxz: case zyz: case zxy: case zyx:
      rotation_1 = Quaternion(cos(angle_1/2.0), 0.0, 0.0, sin(angle_1/2.0));
      break;
    default:
      assert(false);
      break;
  }
  
  switch (order) {
    case yxy: case yxz: case zxy: case zxz:
      rotation_2 = Quaternion(cos(angle_2/2.0), sin(angle_2/2.0), 0.0, 0.0);
      break;
    case zyz: case xyx: case xyz: case zyx:
      rotation_2 = Quaternion(cos(angle_2/2.0), 0.0, sin(angle_2/2.0), 0.0);
      break;
    case yzy: case yzx: case xzx: case xzy:
      rotation_2 = Quaternion(cos(angle_2/2.0), 0.0, 0.0, sin(angle_2/2.0));
      break;
    default:
      assert(false);
      break;
  }
  
  switch (order) {
    case xyx: case zyx: case yzx: case xzx:
      rotation_3 = Quaternion(cos(angle_3/2.0), sin(angle_3/2.0), 0.0, 0.0);
      break;
    case zxy: case yxy: case yzy: case xzy:
      rotation_3 = Quaternion(cos(angle_3/2.0), 0.0, sin(angle_3/2.0), 0.0);
      break;
    case yxz: case zxz: case zyz: case xyz:
      rotation_3 = Quaternion(cos(angle_3/2.0), 0.0, 0.0, sin(angle_3/2.0));
      break;
    default:
      assert(false);
      break;
  }
  
  return QuatMultiply(QuatMultiply(rotation_1, rotation_2), rotation_3);
}
  
Quaternion Quaternion::Identity() {
  return Quaternion(1.0, 0.0, 0.0, 0.0);
}
  
Real Quat2EulX(const Quaternion q, const EulerOrder order) {
  switch (order) {
    case zyx:
      return atan2(-2.0*q.y()*q.z()+2.0*q.w()*q.x(),
                   pow(q.w(),2.0)+pow(q.z(),2.0)-pow(q.y(),2.0)-pow(q.x(),2.0));
      break;
    default:
      throw_line("");
      return NAN;
      break;
  }
}
  
Real Quat2EulZ(const Quaternion q, const EulerOrder order) {
  switch (order) {
    case zyx:
      return atan2(-2.0*q.x()*q.y()+2.0*q.w()*q.z(),
                   pow(q.w(), 2.0)+pow(q.x(), 2.0)
                   -pow(q.y(), 2.0)-pow(q.z(), 2.0));
      break;
    default:
      throw_line("");
      return NAN;
      break;
  }
}

Real Quat2EulY(const Quaternion q, const EulerOrder order) {
  switch (order) {
    case zyx:
      return asin(2.0*q.x()*q.z()+2.0*q.w()*q.y());
      break;
    default:
      throw_line("");
      return NAN;
      break;
  }
}
  
Quaternion QuatInverse(const Quaternion q) {
  Real norm_sq = pow(Norm(q), 2.0);
  Quaternion conj = QuatConj(q);
  return Quaternion(conj.w()/norm_sq,
                    conj.x()/norm_sq, conj.y()/norm_sq, conj.z()/norm_sq);
}
  
} // namespace mcl
