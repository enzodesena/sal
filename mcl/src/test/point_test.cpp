/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
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
  ASSERT(point_a.Equals(Point(0.0,0.0,1.0)));
  ASSERT(! point_a.Equals(Point(1.0,0.0,1.0)));
  ASSERT(IsEqual(point_a.norm(), 1.0));
  ASSERT(IsEqual(point_a.theta(),0));
  
  Point point_b(0.0,0.0,0.0);
  ASSERT(IsEqual(Distance(point_a, point_b),1.0));
  ASSERT(IsEqual(point_b.norm(), 0.0));
  
  Point point_c(1.0,2.0,-1.0);
  ASSERT(IsEqual(Distance(point_a,point_c), Distance(point_c,point_a)));
  ASSERT(IsEqual(Distance(point_a,point_c), sqrt(4.0+1.0+4.0)));
  ASSERT(IsEqual(DotProduct(point_a,point_c), -1.0));
  ASSERT(IsEqual(point_c.norm(), sqrt(1.0+pow(2.0,2)+1.0)));
  
  Point point_d(1.0,0.0,1.0);
  ASSERT(IsEqual(point_d.phi(), 0.0));
  ASSERT(IsEqual(point_d.theta(), pi/4.0));
  ASSERT(IsEqual(point_d.norm(), sqrt(2.0)));
  ASSERT(IsEqual(Theta(point_b,point_d), pi/4.0));
  
  Point point_e(0.0,1.0,0.0);
  ASSERT(IsEqual(point_e.phi(), pi/2.0));
  ASSERT(IsEqual(point_e.theta(), pi/2.0));
  ASSERT(IsEqual(Theta(point_e,point_b), pi/2.0));
  ASSERT(IsEqual(Theta(point_b,point_e), pi/2.0));
  ASSERT(IsEqual(Phi(point_b,point_e), pi/2.0));
  ASSERT(IsEqual(Phi(point_e,point_b), -pi/2.0));
  
  ASSERT(IsEqual(AngleBetweenDirections(point_a.theta(), point_a.phi(),
                                        point_d.theta(), point_d.phi()),
                 pi/4.0));
  ASSERT(IsEqual(AngleBetweenDirections(point_d.theta(), point_d.phi(),
                                        point_a.theta(), point_a.phi()),
                 pi/4.0));
  
  ASSERT(RotateAboutX(point_e, 0.0).Equals(point_e));
  ASSERT(RotateAboutY(point_e, 0.0).Equals(point_e));
  ASSERT(RotateAboutZ(point_e, 0.0).Equals(point_e));
  
  ASSERT(RotateAboutX(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  ASSERT(RotateAboutX(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  ASSERT(RotateAboutX(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  ASSERT(RotateAboutX(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  ASSERT(RotateAboutX(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  ASSERT(RotateAboutX(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  
  ASSERT(RotateAboutY(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  ASSERT(RotateAboutY(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  ASSERT(RotateAboutY(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  ASSERT(RotateAboutY(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  ASSERT(RotateAboutY(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  ASSERT(RotateAboutY(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  
  ASSERT(RotateAboutZ(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  ASSERT(RotateAboutZ(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  ASSERT(RotateAboutZ(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  ASSERT(RotateAboutZ(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  ASSERT(RotateAboutZ(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  ASSERT(RotateAboutZ(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  
  ASSERT(RotateAboutX(Point(1.5,0.0,0.0), pi/2.0).Equals(Point(1.5,0.0,0.0)));
  ASSERT(RotateAboutY(Point(0.0,1.5,0.0), pi/2.0).Equals(Point(0.0,1.5,0.0)));
  ASSERT(RotateAboutZ(Point(0.0,0.0,1.5), pi/2.0).Equals(Point(0.0,0.0,1.5)));
  
  Point point_f = RotateAboutX(RotateAboutY(
                         RotateAboutZ(Point(1.5,-1.0,0.5),pi/5.0),pi/6.0),pi/7.0);
  
  ASSERT((Rotate(Point(1.5,-1.0,0.5), pi/5.0, pi/6.0, pi/7.0)).Equals(point_f));
  ASSERT((Rotate(Point(0.0,0.0,1.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,0.0,1.0)));
  ASSERT((Rotate(Point(0.0,0.0,1.0), 0.0, pi/2.0, 0.0)).Equals(Point(1.0,0.0,0.0)));
  ASSERT((Rotate(Point(0.0,0.0,1.0), 0.0, 0.0, pi/2.0)).Equals(Point(0.0,-1.0,0.0)));
  ASSERT((Rotate(Point(1.0,0.0,0.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  ASSERT((Rotate(Point(0.0,1.0,0.0), 0.0, pi/2.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  ASSERT((Rotate(Point(0.0,1.0,0.0), -PI/2.0, -PI/2.0, 0.0)).Equals(Point(0.0,0.0,1.0)));

  
  ASSERT((Rotate(Point(0.0,1.0,0.0),
                        PI/2.0, PI/2.0, 0.0)).Equals(Point(0.0,0.0,1.0)));
  
  // Proof that is extrinsic, not intrinsic
  ASSERT(!Rotate(Point(0.0,1.0,0.0),
                        PI/2.0, PI/2.0, 0.0).Equals(Point(-1.0,0.0,0.0))); // Intrinsic rotation
  ASSERT(Rotate(Point(0.0,1.0,0.0),
                       PI/2.0, PI/2.0, 0.0).Equals(Point(0.0,0.0,1.0))); // Extrinsic rotation
  

  
  ASSERT(PointSpherical(1.0, PI/2.0, 0.0).Equals(Point(1.0, 0.0, 0.0)));
  ASSERT(PointSpherical(2.0, PI/2.0, 0.0).Equals(Point(2.0, 0.0, 0.0)));
  ASSERT(PointSpherical(1.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 1.0)));
  ASSERT(PointSpherical(2.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 2.0)));
  ASSERT(PointSpherical(1.5, PI/2.0, PI/2.0).
         Equals(Point(0.0, 1.5, 0.0)));
  ASSERT(PointSpherical(1.0, -PI/2.0, 0.0).
         Equals(Point(-1.0, 0.0, 0.0)));

  ASSERT(PointSpherical(1.0, PI/4.0, 0.0).
         Equals(Point(1.0/sqrt(2.0), 0.0, 1.0/sqrt(2.0))));
  
  
  ASSERT(PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  
  ASSERT(Sum(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  ASSERT(Sum(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.0,-1.0,2.0)));
  ASSERT(Sum(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.5,0.0,2.0)));      
  
  ASSERT(Subtract(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  ASSERT(Subtract(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(-1.0,1.0,-2.0)));
  ASSERT(Subtract(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(-0.5,2.0,-2.0)));   
  
  ASSERT(PointOnLine(Point(0,0,0), Point(1,0,0), 0.3).Equals(Point(0.3,0,0)));
  ASSERT(PointOnLine(Point(1,0,0), Point(0,0,0), 0.3).Equals(Point(0.7,0,0)));
  ASSERT(PointOnLine(Point(0,0,0), Point(0,1,0), 0.3).Equals(Point(0,0.3,0)));
  ASSERT(PointOnLine(Point(0,1,0), Point(0,0,0), 0.3).Equals(Point(0,0.7,0)));
  ASSERT(PointOnLine(Point(0,0,0), Point(2,2,0), 1.0).Equals(Point(1.0/sqrt(2.0),1.0/sqrt(2.0),0)));
  ASSERT(PointOnLine(Point(0,0,0), Point(0,2,2), 1.0).Equals(Point(0,1.0/sqrt(2.0),1.0/sqrt(2.0))));
  ASSERT(PointOnLine(Point(0,2,2), Point(0,0,0), 1.0).Equals(Point(0,2-1.0/sqrt(2.0),2-1.0/sqrt(2.0))));
  ASSERT(PointOnLine(Point(1,1,0), Point(2,2,0), 1.0).Equals(Point(1.0+1.0/sqrt(2.0),1.0+1.0/sqrt(2.0),0)));
  ASSERT(PointOnLine(Point(1,1,0), Point(1,3,0), 1.0).Equals(Point(1,2.0,0)));
  ASSERT(PointOnLine(Point(1,1,0), Point(3,1,0), 1.0).Equals(Point(2.0,1,0)));
  
  ASSERT(PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  Point point_g(1.0, 2.0, 1.5);
  Point point_h = Normalized(point_g);
  ASSERT(IsEqual(point_g, Point(1.0, 2.0, 1.5)));
  ASSERT(IsEqual(point_h.norm(), 1.0));
  ASSERT(IsEqual(point_h, Point(0.371390676354104,
                                       0.742781352708207,
                                       0.557086014531156), VERY_SMALL));
  
  // Test projections
  Point point_i(2.0, 0.0, 0.0);
  ASSERT(IsEqual(Projection(point_i, Point(1.0, 0.0, 0.0)),
                        Point(0.0, 0.0, 0.0)));
  ASSERT(IsEqual(Projection(point_i, Point(0.0, 1.0, 0.0)),
                        point_i));
  ASSERT(IsEqual(Projection(point_i, Point(0.0, 2.0, 0.0)),
                        point_i));
  ASSERT(IsEqual(Projection(point_i, Point(0.0, 0.0, 1.0)),
                        point_i));
  ASSERT(IsEqual(Projection(point_i, Point(0.0, 0.0, 2.0)),
                        point_i));
  
  Point point_l(1.5, 2.0, 3.0);
  Point point_l_cmp =
          PointSpherical(point_l.norm()*cos(PI/2.0-point_l.theta()),
                                PI/2.0,
                                point_l.phi());
  ASSERT(IsEqual(Projection(point_l, Point(0,0,1)), point_l_cmp));
  
  // Test IntersectionPlaneLine
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(1,0,0), Point(1,0,0)),
                 Point(1,0,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(2,0,0),
                                       Point(1,0,0), Point(1,0,0)),
                 Point(1,0,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,1,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,1,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(1,0,0),
                                       Point(2,0,0), Point(1,0,0)),
                 Point(2,0,0)));
  
  ASSERT(IsEqual(IntersectionPlaneLine(Point(1,1,-1), Point(0,0,1),
                                       Point(0,0,1), Point(0,0,1)),
                 Point(1,1,1)));
  
  // Case line parallel to plane: returning line_point
  ASSERT(IsEqual(IntersectionPlaneLine(Point(0,0,0), Point(0,1,0),
                                       Point(2,0,0), Point(0,0,1)),
                 Point(0,0,0)));
  ASSERT(IntersectionPlaneLineExists(Point(0,0,0), Point(0,1,0),
                                     Point(2,0,0), Point(0,0,1)));
  
  ASSERT(! IntersectionPlaneLineExists(Point(0,0,1), Point(0,1,0),
                                       Point(2,0,0), Point(0,0,1)));
  
  
  
  Point cross_a = CrossProduct(Point(4, -2, 1), Point(1, -1, 3));
  ASSERT(IsEqual(cross_a, Point(-5, -11, -2)));
    
  Point cross_b = CrossProduct(Point(-2,1,2), Point(-1,0,-2));
  ASSERT(IsEqual(cross_b, Point(-2, -6, 1)));
  
  Point cross_c = CrossProduct(Point(10,1,2), Point(-1,5,-2));
  ASSERT(IsEqual(cross_c, Point(-12, 18, 51)));
  
  Point cross_d = CrossProduct(Point(-1,-1,-1), Point(1,1,1));
  ASSERT(IsEqual(cross_d, Point(0,0,0)));
  
  return true;
}

} // namespace sal
