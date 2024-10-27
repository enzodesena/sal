/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_POINT_H
#define SAL_DSP_POINT_H

#include "mcltypes.h"

namespace sal {

namespace dsp {

class Triplet {
 public:
  /** Constructs a `Point` with all coordinates set to zero. */
  Triplet() noexcept;

  /** Constructor with explicit definition of all coordinates. */
  Triplet(Real x, Real y, Real z) noexcept;

  bool Equals(const Triplet& other_point) const noexcept;

  // Getter methods.
  Real x() const noexcept { return x_; }
  Real y() const noexcept { return y_; }
  Real z() const noexcept { return z_; }

  void SetX(const Real value) noexcept { x_ = value; }
  void SetY(const Real value) noexcept { y_ = value; }
  void SetZ(const Real value) noexcept { z_ = value; }

  /** Returns the norm of the vector, or, in other words, the distance
   of the point from the origin (0,0,0) */
  Real norm() const noexcept;

  /** Returns the angle formed with the z-axis */
  Real theta() const noexcept;

  /** Returns the angle formed between the projection on the x-y axis and
   the x-axis */
  Real phi() const noexcept;

  /** Modifies the point (i.e. vector) such that its norm is equal to 1. */
  void Normalize() noexcept;

 private:
  Real x_;
  Real y_;
  Real z_;
};

typedef Triplet Point;

/**
 Rotates the reference system about the x-axis with the right-hand rule.
 e.g. RotateAboutX(Point(0.0,1.0,0.0), pi/2) == Point(0.0,0.0,1.0)
 */
Point RotateAboutX(const Point&, Real) noexcept;

/**
 Rotates the reference system about the y-axis with the right-hand rule.
 e.g. RotateAboutY(Point(1.0,0.0,0.0), pi/2) == Point(0.0,0.0,-1.0)
 */
Point RotateAboutY(const Point&, Real) noexcept;

/**
 Rotates the reference system about the z-axis with the right-hand rule.
 e.g. RotateAboutZ(Point(0.0,1.0,0.0), pi/2) == Point(-1.0,0.0,0.0)
 */
Point RotateAboutZ(const Point&, Real) noexcept;

/**
 Rotates the reference system with euler angles. Convention is ZYX with
 angles phi, theta and psi, respectively.
 */
Point Rotate(const Point&, Real phi, Real theta, Real psi) noexcept;

Real DotProduct(Point, Point) noexcept;

/** Returns a Point (i.e. 3D vector) corresponding to the cross product
 of two Points (i.e. 3D vectors). Uses the right-hand rule. */
Point CrossProduct(Point, Point) noexcept;
Real Distance(Point, Point) noexcept;
Real Theta(Point, Point) noexcept;
Real Phi(Point, Point) noexcept;
Real AngleBetweenDirections(Real theta_a, Real phi_a, Real theta_b,
                            Real phi_b) noexcept;
Real AngleBetweenPoints(Point, Point) noexcept;

/**
 This returns the point on the line between `point_a` and `point_b` which
 has a distance of `distance` from `point_a`
 */
Point PointOnLine(const Point point_a, const Point point_b,
                  const Real distance) noexcept;

/** Sums the coordinates of `point_a` and `point_b` */
Point Sum(const Point point_a, const Point point_b) noexcept;

/** Subtracts the coordinates of `point_a` from `point_b` (point_a-point_b) */
Point Subtract(const Point point_a, const Point point_b) noexcept;

/**
 Multiplies all coordinates by given constant. Has the effect of changing
 of changing the length of the vector.
 */
Point Multiply(const Point point, const Real constant) noexcept;

/**
 Contructs a point from spherical coordinates, with (r, 0, 0) corresponding
 to the z-axis, and (r, pi/2, 0) corresponding to x-axis. Right-hand rule.
 */
Point PointSpherical(Real r, Real theta, Real phi) noexcept;

/**
 Constructs a vector that is the the projection of the input `vector`
 on the plane (passing through the origin) identified by the vector
 normal to the plane `plane_normal_vector`.
 */
Point Projection(const Point& vector,
                 const Point& plane_normal_vector) noexcept;

/**
 Returns a new point that is a normalized (norm == 1) version of `point`.
 */
Point Normalized(Point point) noexcept;

/**
 Returns the intersection point between a plane and a line. The line is
 identified by a point on a line, line_point, and the direction of the line,
 line_direction (every point on the line can be expressed as
 p=d line_point+line_direction, with d any scalar).
 The plane is identified as any point on the plane, plane_point, and
 the normal to the plane, plane_normal (every point on the plane can be
 expressed as (p-plane_point, plane_normal)=0 where (x,y) is scalar product).
 If the line is contained in the plane, the function returns line_point.
 If there is no intersection, then the function returns a point of NANs.
 The user should first check whether an intersection exists using
 IntersectionPlaneLineExists.
 */
Point IntersectionPlaneLine(const Point& line_point,
                            const Point& line_direction,
                            const Point& plane_point,
                            const Point& plane_normal) noexcept;

/**
 Returns whther or not an intersection between a plane and a line exists.
 The line is identified by a point on a line, line_point,
 and the direction of the line,
 line_direction (every point on the line can be expressed as
 p=d line_point+line_direction, with d any scalar).
 The plane is identified as any point on the plane, plane_point, and
 the normal to the plane, plane_normal (every point on the plane can be
 expressed as (p-plane_point, plane_normal)=0 where (x,y) is scalar product).
 */
bool IntersectionPlaneLineExists(const Point& line_point,
                                 const Point& line_direction,
                                 const Point& plane_point,
                                 const Point& plane_normal) noexcept;

bool PointTest();

} // namespace dsp

} // namespace sal

#endif
