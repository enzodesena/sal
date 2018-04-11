/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "point.h"
#include <cmath>
#include <cassert>
#include "comparisonop.h"

namespace mcl {

bool PointTest() {
  const Real pi = 4*atan(1);
  using mcl::IsEqual;
  
  Point point_a(0.0,0.0,1.0);
  assert(point_a.Equals(Point(0.0,0.0,1.0)));
  assert(! point_a.Equals(Point(1.0,0.0,1.0)));
  assert(IsEqual(point_a.norm(), 1.0));
  assert(IsEqual(point_a.theta(),0));
  
  Point point_b(0.0,0.0,0.0);
  assert(IsEqual(Distance(point_a, point_b),1.0));
  assert(IsEqual(point_b.norm(), 0.0));
  
  Point point_c(1.0,2.0,-1.0);
  assert(IsEqual(Distance(point_a,point_c), Distance(point_c,point_a)));
  assert(IsEqual(Distance(point_a,point_c), sqrt(4.0+1.0+4.0)));
  assert(IsEqual(DotProduct(point_a,point_c), -1.0));
  assert(IsEqual(point_c.norm(), sqrt(1.0+pow(2.0,2)+1.0)));
  
  Point point_d(1.0,0.0,1.0);
  assert(IsEqual(point_d.phi(), 0.0));
  assert(IsEqual(point_d.theta(), pi/4.0));
  assert(IsEqual(point_d.norm(), sqrt(2.0)));
  assert(IsEqual(Theta(point_b,point_d), pi/4.0));
  
  Point point_e(0.0,1.0,0.0);
  assert(IsEqual(point_e.phi(), pi/2.0));
  assert(IsEqual(point_e.theta(), pi/2.0));
  assert(IsEqual(Theta(point_e,point_b), pi/2.0));
  assert(IsEqual(Theta(point_b,point_e), pi/2.0));
  assert(IsEqual(Phi(point_b,point_e), pi/2.0));
  assert(IsEqual(Phi(point_e,point_b), -pi/2.0));
  
  assert(IsEqual(AngleBetweenDirections(point_a.theta(), point_a.phi(),
                                        point_d.theta(), point_d.phi()),
                 pi/4.0));
  assert(IsEqual(AngleBetweenDirections(point_d.theta(), point_d.phi(),
                                        point_a.theta(), point_a.phi()),
                 pi/4.0));
  
  assert(RotateAboutX(point_e, 0.0).Equals(point_e));
  assert(RotateAboutY(point_e, 0.0).Equals(point_e));
  assert(RotateAboutZ(point_e, 0.0).Equals(point_e));
  
  assert(RotateAboutX(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(RotateAboutX(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(RotateAboutX(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(RotateAboutX(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  assert(RotateAboutX(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  assert(RotateAboutX(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  
  assert(RotateAboutY(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  assert(RotateAboutY(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(RotateAboutY(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(RotateAboutY(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(RotateAboutY(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(RotateAboutY(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  
  assert(RotateAboutZ(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(RotateAboutZ(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  assert(RotateAboutZ(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  assert(RotateAboutZ(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(RotateAboutZ(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(RotateAboutZ(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  
  assert(RotateAboutX(Point(1.5,0.0,0.0), pi/2.0).Equals(Point(1.5,0.0,0.0)));
  assert(RotateAboutY(Point(0.0,1.5,0.0), pi/2.0).Equals(Point(0.0,1.5,0.0)));
  assert(RotateAboutZ(Point(0.0,0.0,1.5), pi/2.0).Equals(Point(0.0,0.0,1.5)));
  
  Point point_f = RotateAboutX(RotateAboutY(
                         RotateAboutZ(Point(1.5,-1.0,0.5),pi/5.0),pi/6.0),pi/7.0);
  
  assert((Rotate(Point(1.5,-1.0,0.5), pi/5.0, pi/6.0, pi/7.0)).Equals(point_f));
  assert((Rotate(Point(0.0,0.0,1.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,0.0,1.0)));
  assert((Rotate(Point(0.0,0.0,1.0), 0.0, pi/2.0, 0.0)).Equals(Point(1.0,0.0,0.0)));
  assert((Rotate(Point(0.0,0.0,1.0), 0.0, 0.0, pi/2.0)).Equals(Point(0.0,-1.0,0.0)));
  assert((Rotate(Point(1.0,0.0,0.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  assert((Rotate(Point(0.0,1.0,0.0), 0.0, pi/2.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  assert((Rotate(Point(0.0,1.0,0.0), -PI/2.0, -PI/2.0, 0.0)).Equals(Point(0.0,0.0,1.0)));

  
  assert((Rotate(Point(0.0,1.0,0.0),
                        PI/2.0, PI/2.0, 0.0)).Equals(Point(0.0,0.0,1.0)));
  
  // Proof that is extrinsic, not intrinsic
  assert(!Rotate(Point(0.0,1.0,0.0),
                        PI/2.0, PI/2.0, 0.0).Equals(Point(-1.0,0.0,0.0))); // Intrinsic rotation
  assert(Rotate(Point(0.0,1.0,0.0),
                       PI/2.0, PI/2.0, 0.0).Equals(Point(0.0,0.0,1.0))); // Extrinsic rotation
  

  
  assert(PointSpherical(1.0, PI/2.0, 0.0).Equals(Point(1.0, 0.0, 0.0)));
  assert(PointSpherical(2.0, PI/2.0, 0.0).Equals(Point(2.0, 0.0, 0.0)));
  assert(PointSpherical(1.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 1.0)));
  assert(PointSpherical(2.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 2.0)));
  assert(PointSpherical(1.5, PI/2.0, PI/2.0).
         Equals(Point(0.0, 1.5, 0.0)));
  assert(PointSpherical(1.0, -PI/2.0, 0.0).
         Equals(Point(-1.0, 0.0, 0.0)));

  assert(PointSpherical(1.0, PI/4.0, 0.0).
         Equals(Point(1.0/sqrt(2.0), 0.0, 1.0/sqrt(2.0))));
  
  
  assert(PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  
  assert(Sum(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Sum(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Sum(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.5,0.0,2.0)));      
  
  assert(Subtract(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Subtract(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(-1.0,1.0,-2.0)));
  assert(Subtract(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(-0.5,2.0,-2.0)));   
  
  assert(PointOnLine(Point(0,0,0), Point(1,0,0), 0.3).Equals(Point(0.3,0,0)));
  assert(PointOnLine(Point(1,0,0), Point(0,0,0), 0.3).Equals(Point(0.7,0,0)));
  assert(PointOnLine(Point(0,0,0), Point(0,1,0), 0.3).Equals(Point(0,0.3,0)));
  assert(PointOnLine(Point(0,1,0), Point(0,0,0), 0.3).Equals(Point(0,0.7,0)));
  assert(PointOnLine(Point(0,0,0), Point(2,2,0), 1.0).Equals(Point(1.0/sqrt(2.0),1.0/sqrt(2.0),0)));
  assert(PointOnLine(Point(0,0,0), Point(0,2,2), 1.0).Equals(Point(0,1.0/sqrt(2.0),1.0/sqrt(2.0))));
  assert(PointOnLine(Point(0,2,2), Point(0,0,0), 1.0).Equals(Point(0,2-1.0/sqrt(2.0),2-1.0/sqrt(2.0))));
  assert(PointOnLine(Point(1,1,0), Point(2,2,0), 1.0).Equals(Point(1.0+1.0/sqrt(2.0),1.0+1.0/sqrt(2.0),0)));
  assert(PointOnLine(Point(1,1,0), Point(1,3,0), 1.0).Equals(Point(1,2.0,0)));
  assert(PointOnLine(Point(1,1,0), Point(3,1,0), 1.0).Equals(Point(2.0,1,0)));
  
  assert(PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  Point point_g(1.0, 2.0, 1.5);
  Point point_h = Normalized(point_g);
  assert(IsEqual(point_g, Point(1.0, 2.0, 1.5)));
  assert(IsEqual(point_h.norm(), 1.0));
  assert(IsEqual(point_h, Point(0.371390676354104,
                                       0.742781352708207,
                                       0.557086014531156), VERY_SMALL));
  
  // Test projections
  Point point_i(2.0, 0.0, 0.0);
  assert(IsEqual(Projection(point_i, Point(1.0, 0.0, 0.0)),
                        Point(0.0, 0.0, 0.0)));
  assert(IsEqual(Projection(point_i, Point(0.0, 1.0, 0.0)),
                        point_i));
  assert(IsEqual(Projection(point_i, Point(0.0, 2.0, 0.0)),
                        point_i));
  assert(IsEqual(Projection(point_i, Point(0.0, 0.0, 1.0)),
                        point_i));
  assert(IsEqual(Projection(point_i, Point(0.0, 0.0, 2.0)),
                        point_i));
  
  Point point_l(1.5, 2.0, 3.0);
  Point point_l_cmp =
          PointSpherical(point_l.norm()*cos(PI/2.0-point_l.theta()),
                                PI/2.0,
                                point_l.phi());
  assert(IsEqual(Projection(point_l, Point(0,0,1)), point_l_cmp));
  
  // Test IntersectionPlaneLine
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(1,0,0), Point(1,0,0)),
                 Point(1,0,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(2,0,0),
                                       Point(1,0,0), Point(1,0,0)),
                 Point(1,0,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(0,1,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,1,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  assert(IsEqual(IntersectionPlaneLine(Point(1,1,-1), Point(0,0,1),
                                       Point(0,0,1), Point(0,0,1)),
                 Point(1,1,1)));
  
  // Case line parallel to plane: returning line_point
  assert(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(0,1,0),
                                       Point(2,0,0), Point(0,0,1)),
                 Point(0,0,0)));
  assert(IntersectionPlaneLineExists(Point(0,0,0), Point(0,1,0),
                                     Point(2,0,0), Point(0,0,1)));
  
  assert(! IntersectionPlaneLineExists(Point(0,0,1), Point(0,1,0),
                                       Point(2,0,0), Point(0,0,1)));
  
  
  PointSpeedLimiter speed_limiter(Point(0,0,0), 1);
  speed_limiter.SetNextPosition(Point(1,0,0));
  speed_limiter.Tick(1.0);
  assert(IsEqual(speed_limiter.position(), Point(1,0,0)));
  assert(speed_limiter.HasReachedPosition());
  
  speed_limiter.SetNextPosition(Point(2,0,0));
  speed_limiter.Tick(1.0);
  assert(IsEqual(speed_limiter.position(), Point(2,0,0)));
  assert(speed_limiter.HasReachedPosition());
  
  speed_limiter.SetNextPosition(Point(0,0,0));
  speed_limiter.Tick(2.0);
  assert(IsEqual(speed_limiter.position(), Point(0,0,0)));
  assert(speed_limiter.HasReachedPosition());
  
  speed_limiter.SetNextPosition(Point(2,0,0));
  speed_limiter.Tick(1.0);
  assert(IsEqual(speed_limiter.position(), Point(1,0,0)));
  assert(!speed_limiter.HasReachedPosition());
  speed_limiter.Tick(1.0);
  assert(IsEqual(speed_limiter.position(), Point(2,0,0)));
  assert(speed_limiter.HasReachedPosition());
  
  return true;
}

} // namespace sal
