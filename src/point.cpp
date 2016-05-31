/*
 point.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "point.h"
#include <cmath>
#include "comparisonop.h"

namespace mcl {

Point::Point() {}

Point::Point(Real x, Real y, Real z) :
        x_(x), y_(y), z_(z) {}
  
bool Point::Equals(const Point& other_point) const {
  return IsEqual(*this, other_point);
}

Real Point::norm() const { 
  return sqrt(pow(x(),2) + pow(y(),2) + pow(z(),2));
}

Real Point::theta() const { return (Real) acos(z()/norm()); }

Real Point::phi() const { return (Real) atan2(y(), x()); }

void Point::Normalize() {
  Real vector_norm = norm();
  x_ = x_/vector_norm;
  y_ = y_/vector_norm;
  z_ = z_/vector_norm;
}


Point Point::RotateAboutX(const Point& point, Real angle) {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x_, 
               point.y_*cos_angle + point.z_*(-sin_angle), 
               point.y_*(sin_angle) + point.z_*cos_angle);
}


Point Point::RotateAboutY(const Point& point, Real angle) {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x_*cos_angle+point.z_*sin_angle, 
               point.y_, 
               point.x_*(-sin_angle) + point.z_*cos_angle);
}


Point Point::RotateAboutZ(const Point& point, Real angle) {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x_*cos_angle + point.y_*(-sin_angle),
               point.x_*sin_angle + point.y_*cos_angle,
               point.z_);
}



Point Point::Rotate(const Point& point, Real theta_1, Real theta_2, Real theta_3) {
  Point rotated_about_z = Point::RotateAboutZ(point, theta_1);
  Point rotated_about_y = Point::RotateAboutY(rotated_about_z, theta_2);
  Point rotated_about_x = Point::RotateAboutX(rotated_about_y, theta_3);
  
  return rotated_about_x;
}


Real Point::Distance(Point point_a, Point point_b) {
  Point point(point_a.x() - point_b.x(), 
              point_a.y() - point_b.y(), 
              point_a.z() - point_b.z());
  return point.norm();
}

// point_a is taken as the new centre of the reference system
Real Point::Theta(Point point_a, Point point_b) {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.theta();
}

// point_a is taken as the new centre of the reference system
Real Point::Phi(Point point_a, Point point_b) {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.phi();
}

Real Point::DotProduct(Point point_a, Point point_b) {
  return point_a.x()*point_b.x()+point_a.y()*point_b.y()+point_a.z()*point_b.z();
}

// Returns the angle formed betwee two directions
Real Point::AngleBetweenDirections(Real theta_a, Real phi_a,
                                    Real theta_b, Real phi_b) {
  Point point_a(sin(theta_a)*cos(phi_a), sin(theta_a)*sin(phi_a), cos(theta_a));
  Point point_b(sin(theta_b)*cos(phi_b), sin(theta_b)*sin(phi_b), cos(theta_b));
  
  return acos(DotProduct(point_a, point_b));
}

// Returns the angle (formed at the origin) between the two points
Real Point::AngleBetweenPoints(Point point_a, Point point_b) {
  point_a.Normalize();
  point_b.Normalize();
  return acos(DotProduct(point_a, point_b));
}
  
Point Point::PointSpherical(Real r, Real theta, Real phi) {
  Real x = r * cos(phi) * sin(theta);
  Real y = r * sin(phi) * sin(theta);
  Real z = r * cos(theta);
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
  

Point Point::Multiply(const Point point, const Real constant) {
  return Point(point.x() * constant,
               point.y() * constant,
               point.z() * constant);
}
  
Point Point::PointOnLine(const Point point_a, const Point point_b, 
                         const Real distance) {
  Point point_centered = Point::Subtract(point_b, point_a);
  Point out_point_centered = Point::PointSpherical(distance, point_centered.theta(), 
                                                   point_centered.phi());
  
  return Point::Sum(out_point_centered, point_a);
}
  
Point Point::Normalized(Point point) {
  point.Normalize();
  return point;
}
  
Point Point::Projection(const Point& vector, const Point& plane_normal_vector) {
  Point normalised_plane_normal_vector = Point::Normalized(plane_normal_vector);
  return Subtract(vector, Multiply(normalised_plane_normal_vector,
                                   DotProduct(vector,
                                              normalised_plane_normal_vector)));
}
  
} // namespace sal
