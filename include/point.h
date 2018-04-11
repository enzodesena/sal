/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef MCL_POINT_H
#define MCL_POINT_H

#include "mcltypes.h"

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {

class MCL_API Point {
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
  
  void set_x(Real x) { x_ = x; }
  void set_y(Real y) { y_ = y; }
  void set_z(Real z) { z_ = z; }
  
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

private:
  Real x_;
  Real y_;
  Real z_;
};
  
/**
 Rotates the reference system about the x-axis with the right-hand rule.
 e.g. RotateAboutX(Point(0.0,1.0,0.0), pi/2) == Point(0.0,0.0,1.0)
 */
MCL_API Point RotateAboutX(const Point&, Real);

/**
 Rotates the reference system about the y-axis with the right-hand rule.
 e.g. RotateAboutY(Point(1.0,0.0,0.0), pi/2) == Point(0.0,0.0,-1.0)
 */
MCL_API Point RotateAboutY(const Point&, Real);

/**
 Rotates the reference system about the z-axis with the right-hand rule.
 e.g. RotateAboutZ(Point(0.0,1.0,0.0), pi/2) == Point(-1.0,0.0,0.0)
 */
MCL_API Point RotateAboutZ(const Point&, Real);

/**
 Rotates the reference system with euler angles. Convention is ZYX with
 angles phi, theta and psi, respectively.
 */
MCL_API Point Rotate(const Point&, Real phi,
                     Real theta, Real psi);

MCL_API Real DotProduct(Point, Point);
MCL_API Real Distance(Point, Point);
MCL_API Real Theta(Point, Point);
MCL_API Real Phi(Point, Point);
MCL_API Real AngleBetweenDirections(Real theta_a, Real phi_a,
                                   Real theta_b, Real phi_b);
MCL_API Real AngleBetweenPoints(Point, Point);

/**
 This returns the point on the line between `point_a` and `point_b` which
 has a distance of `distance` from `point_a`
 */
MCL_API Point PointOnLine(const Point point_a, const Point point_b,
                          const Real distance);

/** Sums the coordinates of `point_a` and `point_b` */
MCL_API Point Sum(const Point point_a, const Point point_b);

/** Subtracts the coordinates of `point_a` from `point_b` (point_a-point_b) */
MCL_API Point Subtract(const Point point_a, const Point point_b);

/**
 Multiplies all coordinates by given constant. Has the effect of changing
 of changing the length of the vector.
 */
MCL_API Point Multiply(const Point point, const Real constant);

/**
 Contructs a point from spherical coordinates, with (r, 0, 0) corresponding
 to the z-axis, and (r, pi/2, 0) corresponding to x-axis. Right-hand rule.
 */
MCL_API Point PointSpherical(Real r, Real theta, Real phi);

/**
 Constructs a vector that is the the projection of the input `vector`
 on the plane (passing through the origin) identified by the vector
 normal to the plane `plane_normal_vector`.
 */
MCL_API Point Projection(const Point& vector, const Point& plane_normal_vector);

/**
 Returns a new point that is a normalized (norm == 1) version of `point`.
 */
MCL_API Point Normalized(Point point);

/**
 Returns the intersection point between a plane and a line. The line is
 identified by a point on a line, line_point, and the direction of the line,
 line_direction (every point on the line can be expressed as
 p=d line_point+line_direction, with d any scalar).
 The plane is identified as any point on the plane, plane_point, and
 the normal to the plane, plane_normal (every point on the plane can be
 expressed as (p-plane_point, plane_normal)=0 where (x,y) is scalar product).
 If the line is contained in the plane, the function returns line_point.
 If there is no intersection, then the function throws an exception.
 The user should first check whether an intersection exists using
 IntersectionPlaneLineExists.
 */
MCL_API Point IntersectionPlaneLine(const Point& line_point,
                                    const Point& line_direction,
                                    const Point& plane_point,
                                    const Point& plane_normal);

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
MCL_API bool IntersectionPlaneLineExists(const Point& line_point,
                                         const Point& line_direction,
                                         const Point& plane_point,
                                         const Point& plane_normal);
  
bool PointTest();
  
/** */
class PointSpeedLimiter {
public:
  PointSpeedLimiter(Point position, Real max_speed) :
  max_speed_(max_speed),
  next_position_(position), current_position_(position) {}
  
  void SetNextPosition(mcl::Point next_position) {
    next_position_ = next_position;
  }
  
  void Tick(Real time_elapsed_since_last_tick) {
    // Detect if the point is moving faster than `max_speed`
    Real speed = Distance(next_position_, current_position_) /
          time_elapsed_since_last_tick;
    
    if (speed <= max_speed_) {
      current_position_ = next_position_;
    } else {
      current_position_ = PointOnLine(current_position_,
                                      next_position_,
                                      max_speed_*time_elapsed_since_last_tick);
    }
  }
  
  bool HasReachedPosition() {
    return current_position_.Equals(next_position_);
  }
  
  mcl::Point position() {
    return current_position_;
  }
  
private:
  /** This is the (un-throttled position) */
  Point next_position_;
  Point current_position_;
  Real max_speed_;
};
  
  
} // namespace mcl

#endif
