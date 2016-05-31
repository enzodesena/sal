/*
 point.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_POINT_H
#define MCL_POINT_H

#include "mcltypes.h"

namespace mcl {

class Point {
public:
  /** Constructs a `Point` with all coordinates set to zero. */
  Point();
  
  /** Constructor with explicit definition of all coordinates. */
  Point(Real x, Real y, Real z);
  
  bool Equals(const Point& other_point) const;
  
  // Getter methods.
  Real x() const { return x_; }
  Real y() const { return y_; }
  Real z() const { return z_; }
  
  void set_x(Real);
  void set_y(Real);
  void set_z(Real);
  
  /** Returns the norm of the vector, or, in other words, the distance
   of the point from the origin (0,0,0) */
  Real norm() const;
  
  /** Returns the angle formed with the z-axis */
  Real theta() const;
  
  /** Returns the angle formed between the projection on the x-y axis and
   the x-axis */
  Real phi() const;
  
  /** Modifies the point (i.e. vector) such that its norm is equal to 1. */
  void Normalize();

  /**
   Rotates the reference system about the x-axis with the right-hand rule.
   e.g. RotateAboutX(Point(0.0,1.0,0.0), pi/2) == Point(0.0,0.0,1.0)
   */
  static Point RotateAboutX(const Point&, Real);
  
  /**
   Rotates the reference system about the y-axis with the right-hand rule.
   e.g. RotateAboutY(Point(1.0,0.0,0.0), pi/2) == Point(0.0,0.0,-1.0)
   */
  static Point RotateAboutY(const Point&, Real);
  
  /** 
   Rotates the reference system about the z-axis with the right-hand rule.
   e.g. RotateAboutZ(Point(0.0,1.0,0.0), pi/2) == Point(-1.0,0.0,0.0)
   */
  static Point RotateAboutZ(const Point&, Real);
  
  /**
   Rotates the reference system with euler angles. Convention is ZYX with
   angles phi, theta and psi, respectively.
   */
  static Point Rotate(const Point&, Real phi, 
                      Real theta, Real psi);
  
  static Real DotProduct(Point, Point);
  static Real Distance(Point, Point);
  static Real Theta(Point, Point);
  static Real Phi(Point, Point);
  static Real AngleBetweenDirections(Real theta_a, Real phi_a,
                                      Real theta_b, Real phi_b);
  static Real AngleBetweenPoints(Point, Point);
  
  /**
   This returns the point on the line between `point_a` and `point_b` which
   has a distance of `distance` from `point_a`
   */
  static Point PointOnLine(const Point point_a, const Point point_b, 
                           const Real distance);
  
  /** Sums the coordinates of `point_a` and `point_b` */
  static Point Sum(const Point point_a, const Point point_b);
  
  /** Subtracts the coordinates of `point_a` from `point_b` (point_a-point_b) */
  static Point Subtract(const Point point_a, const Point point_b);
  
  /** 
   Multiplies all coordinates by given constant. Has the effect of changing
   of changing the length of the vector.
   */
  static Point Multiply(const Point point, const Real constant);
  
  /**
   Contructs a point from spherical coordinates, with (r, 0, 0) corresponding
   to the z-axis, and (r, pi/2, 0) corresponding to x-axis. Right-hand rule. 
   */
  static Point PointSpherical(Real r, Real theta, Real phi);
  
  /** 
   Constructs a vector that is the the projection of the input `vector`
   on the plane (passing through the origin) identified by the vector
   normal to the plane `plane_normal_vector`.
   */
  static Point Projection(const Point& vector, const Point& plane_normal_vector);
  
  /**
   Returns a new point that is a normalized (norm == 1) version of `point`.
   */
  static Point Normalized(Point point);
  
  static bool Test();
private:
  Real x_;
  Real y_;
  Real z_;
};

} // namespace mcl

#endif
