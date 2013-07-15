/*
 point.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "point.h"
#include "saltypes.h"
#include <cmath>
#include "equalityop.h"
#include "salconstants.h"

namespace sal {

Point::Point() {}

Point::Point(Length x, Length y, Length z) :
        x_(x), y_(y), z_(z) {}

  
bool Point::IsEqual(const Point& point_a, const Point& point_b,
                    const Length precision) {
  return mcl::IsEqual(point_a.x(), point_b.x(), precision) &&
         mcl::IsEqual(point_a.y(), point_b.y(), precision) &&
         mcl::IsEqual(point_a.z(), point_b.z(), precision);
}
  
bool Point::Equals(const Point& other_point) const {
  return Point::IsEqual(*this, other_point);
}

Length Point::norm() const { 
  return sqrt(pow(x(),2) + pow(y(),2) + pow(z(),2));
}

Angle Point::theta() const { return (Angle) acos(z()/norm()); }

Angle Point::phi() const { return (Angle) atan2(y(), x()); }

void Point::Normalize() {
  Length vector_norm = norm();
  x_ = x_/vector_norm;
  y_ = y_/vector_norm;
  z_ = z_/vector_norm;
}



Point Point::RotateAboutX(const Point& point, Angle angle) {
  Length cos_angle = cos(angle);
  Length sin_angle = sin(angle);
  
  return Point(point.x_, 
               point.y_*cos_angle + point.z_*(-sin_angle), 
               point.y_*(sin_angle) + point.z_*cos_angle);
}


Point Point::RotateAboutY(const Point& point, Angle angle) {
  Length cos_angle = cos(angle);
  Length sin_angle = sin(angle);
  
  return Point(point.x_*cos_angle+point.z_*sin_angle, 
               point.y_, 
               point.x_*(-sin_angle) + point.z_*cos_angle);
}


Point Point::RotateAboutZ(const Point& point, Angle angle) {
  Length cos_angle = cos(angle);
  Length sin_angle = sin(angle);
  
  return Point(point.x_*cos_angle + point.y_*(-sin_angle),
               point.x_*sin_angle + point.y_*cos_angle,
               point.z_);
}



Point Point::Rotate(const Point& point, Angle theta_1, Angle theta_2, Angle theta_3) {
  Point rotated_about_z = Point::RotateAboutZ(point, theta_1);
  Point rotated_about_y = Point::RotateAboutY(rotated_about_z, theta_2);
  Point rotated_about_x = Point::RotateAboutX(rotated_about_y, theta_3);
  
  return rotated_about_x;
}


Length Point::Distance(Point point_a, Point point_b) {
  Point point(point_a.x() - point_b.x(), 
              point_a.y() - point_b.y(), 
              point_a.z() - point_b.z());
  return point.norm();
}

// point_a is taken as the new centre of the reference system
Angle Point::Theta(Point point_a, Point point_b) {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.theta();
}

// point_a is taken as the new centre of the reference system
Angle Point::Phi(Point point_a, Point point_b) {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.phi();
}

Length Point::DotProduct(Point point_a, Point point_b) {
  return point_a.x()*point_b.x()+point_a.y()*point_b.y()+point_a.z()*point_b.z();
}

// Returns the angle formed betwee two directions
Angle Point::AngleBetweenDirections(Angle theta_a, Angle phi_a,
                                    Angle theta_b, Angle phi_b) {
  Point point_a(sin(theta_a)*cos(phi_a), sin(theta_a)*sin(phi_a), cos(theta_a));
  Point point_b(sin(theta_b)*cos(phi_b), sin(theta_b)*sin(phi_b), cos(theta_b));
  
  return acos(DotProduct(point_a, point_b));
}

// Returns the angle (formed at the origin) between the two points
Angle Point::AngleBetweenPoints(Point point_a, Point point_b) {
  point_a.Normalize();
  point_b.Normalize();
  return acos(DotProduct(point_a, point_b));
}
  
Point Point::PointSpherical(Length r, Angle theta, Angle phi) {
  Length x = r * cos(phi) * sin(theta);
  Length y = r * sin(phi) * sin(theta);
  Length z = r * cos(theta);
  return Point(x, y, z);
}
  
Point Point::Sum(const Point point_a, const Point point_b) {
  return Point(point_a.x()+point_b.x(),
               point_a.y()+point_b.y(),
               point_a.z()+point_b.z());
}

Point Point::Subtract(const Point point_a, const Point point_b) {
  return Point(point_a.x()-point_b.x(),
               point_a.y()-point_b.y(),
               point_a.z()-point_b.z());
}
  
Point Point::PointOnLine(const Point point_a, const Point point_b, 
                         const Length distance) {
  Point point_centered = Point::Subtract(point_b, point_a);
  Point out_point_centered = Point::PointSpherical(distance, point_centered.theta(), 
                                                   point_centered.phi());
  
  return Point::Sum(out_point_centered, point_a);
}
  
  
bool Point::IsEqual(std::vector<Point> points_a,
                    std::vector<Point> points_b) {
  const UInt num_points = points_a.size();
  if (num_points != points_b.size()) { return false; }
  for (UInt i=0; i<num_points; ++i) {
    if (! Point::IsEqual(points_a[i], points_b[i])) { return false; }
  }
  return true;
}

Point Point::Normalized(Point point) {
  point.Normalize();
  return point;
}
  
} // namespace sal
