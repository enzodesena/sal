/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "point.h"
#include <iostream>
#include <cmath>
#include "comparisonop.h"

namespace mcl {

Triplet::Triplet() noexcept {}

Triplet::Triplet(Real x, Real y, Real z) noexcept :
        x_(x), y_(y), z_(z) {}
  
bool Triplet::Equals(const Triplet& other_point) const noexcept {
  return IsEqual(*this, other_point);
}

Real Triplet::norm() const noexcept {
  return sqrt(pow(x(),2) + pow(y(),2) + pow(z(),2));
}

Real Triplet::theta() const noexcept { return (Real) acos(z()/norm()); }

Real Triplet::phi() const noexcept { return (Real) atan2(y(), x()); }

void Triplet::Normalize() noexcept {
  Real vector_norm = norm();
  x_ = x_/vector_norm;
  y_ = y_/vector_norm;
  z_ = z_/vector_norm;
}


Point RotateAboutX(const Point& point, Real angle) noexcept {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x(),
               point.y()*cos_angle + point.z()*(-sin_angle),
               point.y()*(sin_angle) + point.z()*cos_angle);
}


Point RotateAboutY(const Point& point, Real angle) noexcept {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x()*cos_angle+point.z()*sin_angle,
               point.y(),
               point.x()*(-sin_angle) + point.z()*cos_angle);
}


Point RotateAboutZ(const Point& point, Real angle) noexcept {
  Real cos_angle = cos(angle);
  Real sin_angle = sin(angle);
  
  return Point(point.x()*cos_angle + point.y()*(-sin_angle),
               point.x()*sin_angle + point.y()*cos_angle,
               point.z());
}



Point Rotate(const Point& point, Real theta_1,
             Real theta_2, Real theta_3) noexcept {
  Point rotated_about_z = RotateAboutZ(point, theta_1);
  Point rotated_about_y = RotateAboutY(rotated_about_z, theta_2);
  Point rotated_about_x = RotateAboutX(rotated_about_y, theta_3);
  
  return rotated_about_x;
}


Real Distance(Point point_a, Point point_b) noexcept {
  Point point(point_a.x() - point_b.x(), 
              point_a.y() - point_b.y(), 
              point_a.z() - point_b.z());
  return point.norm();
}

// point_a is taken as the new centre of the reference system
Real Theta(Point point_a, Point point_b) noexcept {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.theta();
}

// point_a is taken as the new centre of the reference system
Real Phi(Point point_a, Point point_b) noexcept {
  Point point(point_b.x()-point_a.x(), point_b.y()-point_a.y(), point_b.z()-point_a.z());
  return point.phi();
}

Real DotProduct(Point point_a, Point point_b) noexcept {
  return point_a.x()*point_b.x()+point_a.y()*point_b.y()+point_a.z()*point_b.z();
}

// Returns the angle formed betwee two directions
Real AngleBetweenDirections(Real theta_a, Real phi_a,
                                    Real theta_b, Real phi_b) noexcept {
  Point point_a(sin(theta_a)*cos(phi_a), sin(theta_a)*sin(phi_a), cos(theta_a));
  Point point_b(sin(theta_b)*cos(phi_b), sin(theta_b)*sin(phi_b), cos(theta_b));
  
  return acos(DotProduct(point_a, point_b));
}

// Returns the angle (formed at the origin) between the two points
Real AngleBetweenPoints(Point point_a, Point point_b) noexcept {
  point_a.Normalize();
  point_b.Normalize();
  return acos(DotProduct(point_a, point_b));
}
  
Point PointSpherical(Real r, Real theta, Real phi) noexcept {
  Real x = r * cos(phi) * sin(theta);
  Real y = r * sin(phi) * sin(theta);
  Real z = r * cos(theta);
  return Point(x, y, z);
}
  
Point Sum(const Point point_a, const Point point_b) noexcept {
  return Point(point_a.x()+point_b.x(),
               point_a.y()+point_b.y(),
               point_a.z()+point_b.z());
}

Point Subtract(const Point point_a, const Point point_b) noexcept {
  return Point(point_a.x()-point_b.x(),
               point_a.y()-point_b.y(),
               point_a.z()-point_b.z());
}
  

Point Multiply(const Point point, const Real constant) noexcept {
  return Point(point.x() * constant,
               point.y() * constant,
               point.z() * constant);
}
  
Point PointOnLine(const Point point_a, const Point point_b, 
                         const Real distance) noexcept {
  Point point_centered = Subtract(point_b, point_a);
  Point out_point_centered = PointSpherical(distance, point_centered.theta(), 
                                                   point_centered.phi());
  
  return Sum(out_point_centered, point_a);
}
  
Point Normalized(Point point) noexcept {
  point.Normalize();
  return point;
}
  
Point Projection(const Point& vector,
                 const Point& plane_normal_vector) noexcept {
  Point normalised_plane_normal_vector = Normalized(plane_normal_vector);
  return Subtract(vector, Multiply(normalised_plane_normal_vector,
                                   DotProduct(vector,
                                              normalised_plane_normal_vector)));
}
  
bool IntersectionPlaneLineExists(const Point& line_point,
                                 const Point& line_direction,
                                 const Point& plane_point,
                                 const Point& plane_normal) noexcept {
  // TODO: find a way to avoid rewriting this calculation in the next function
  Real numerator = DotProduct(Subtract(plane_point, line_point),
                              plane_normal);
  Real denominator = DotProduct(line_direction, plane_normal);
  
  bool zero_numerator = IsEqual(numerator, 0.0);
  bool zero_denominator = IsEqual(denominator, 0.0);
  
  // If denominator = 0 then the line and plane are parallel.
  // There will be two cases:
  // if numerator = 0 then the line is contained in the plane, that is, the line
  // intersects the plane at each point of the line (and thus an intersection
  // still exists).
  // Otherwise, the line and plane have no intersection.
  // So, the case of no intersection is when the denominator = 0 and the
  // numerator is not = 0.
  return zero_denominator & ! zero_numerator;
}
  
Point IntersectionPlaneLine(const Point& line_point,
                            const Point& line_direction,
                            const Point& plane_point,
                            const Point& plane_normal) noexcept {
  if (! IntersectionPlaneLineExists(line_point, line_direction,
                                    plane_point, plane_normal)) {
    return Point(NAN, NAN, NAN);
  }
  
  Real d = DotProduct(Subtract(plane_point, line_point),
                      plane_normal) /
           DotProduct(line_direction, plane_normal);
  
  // if line and plane are parallel, and line is contained in plane
  if (IsNan(d)) {
    return line_point;
  } else {
    return Sum(Multiply(line_direction, d), line_point);
  }
}
  
} // namespace sal
