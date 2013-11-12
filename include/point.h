/*
 point.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_POINT_H
#define SAL_POINT_H

#include "saltypes.h"
#include <cmath>
#include "comparisonop.h"

namespace sal {

class Point {
public:
  // Constructs a `Point` with all coordinates set to zero.
  Point();
  
  // Constructor with explicit definition of all coordinates.
  Point(Length x, Length y, Length z);
  
  bool Equals(const Point& other_point) const;
  
  // Getter methods.
  Length x() const { return x_; }
  Length y() const { return y_; }
  Length z() const { return z_; }
  
  void set_x(Length);
  void set_y(Length);
  void set_z(Length);
  
  Length norm() const;
  Angle theta() const;
  Angle phi() const;
  
  // Modifies the point (i.e. vector) such that its norm is equal to 1.
  void Normalize();

  // Rotate the reference system about the x-axis with the right-hand rule. 
  // e.g. RotateAboutX(Point(0.0,1.0,0.0), pi/2) == Point(0.0,0.0,1.0)
  static Point RotateAboutX(const Point&, Angle);
  
  // Rotate the reference system about the y-axis with the right-hand rule. 
  // e.g. RotateAboutY(Point(1.0,0.0,0.0), pi/2) == Point(0.0,0.0,-1.0)
  static Point RotateAboutY(const Point&, Angle);
  
  // Rotate the reference system about the z-axis with the right-hand rule. 
  // e.g. RotateAboutZ(Point(0.0,1.0,0.0), pi/2) == Point(-1.0,0.0,0.0)
  static Point RotateAboutZ(const Point&, Angle);
  
  // Rotate the reference system with euler angles. Convention is ZYX with 
  // angles phi, theta and psi, respectively.
  static Point Rotate(const Point&, Angle phi, 
                      Angle theta, Angle psi);
  
  static Length DotProduct(Point, Point);
  static Length Distance(Point, Point);
  static Angle Theta(Point, Point);
  static Angle Phi(Point, Point);
  static Angle AngleBetweenDirections(Angle, Angle, Angle, Angle);
  static Angle AngleBetweenPoints(Point, Point);
  
  // This returns the point on the line between `point_a` and `point_b` which 
  // has a distance of `distance` from `point_a`
  static Point PointOnLine(const Point point_a, const Point point_b, 
                           const Length distance);
  
  // Sums the coordinates of `point_a` and `point_b`
  static Point Sum(const Point point_a, const Point point_b);
  
  // Subtracts the coordinates of `point_a` from `point_b` (point_a-point_b)
  static Point Subtract(const Point point_a, const Point point_b);
  
  // Contructs a point from spherical coordinates, with (r, 0, 0) corresponding
  // to the z-axis, and (r, pi/2, 0) corresponding to x-axis. Right-hand rule. 
  static Point PointSpherical(Length r, Angle theta, Angle phi);
  
  static bool
  IsEqual(const Point& point_a, const Point& point_b,
          const Length precision = VERY_SMALL);
  
  static bool IsEqual(std::vector<Point> points_a,
                      std::vector<Point> points_b);
  
  // Returns a new point that is a normalized (norm == 1) version of `point`.
  static Point Normalized(Point point);
  
  static bool Test();
private:
  Length x_;
  Length y_;
  Length z_;
};

} // namespace sal

#endif
