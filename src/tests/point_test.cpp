/*
 point_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 150 $
 Last changed date:  $Date: 2012-10-05 06:11:12 +0100 (Fri, 05 Oct 2012) $
 */

#include "point.h"
#include "mcl.h"
#include "sattypes.h"
#include "satconstants.h"
#include <cmath>

namespace sat {

bool Point::Test() {
  const Angle pi = 4*atan(1);
  using mcl::IsEqual;
  
  Point point_a(0.0,0.0,1.0);
  assert(point_a.Equals(Point(0.0,0.0,1.0)));
  assert(! point_a.Equals(Point(1.0,0.0,1.0)));
  assert(IsEqual(point_a.norm(), 1.0));
  assert(IsEqual(point_a.theta(),0));
  
  Point point_b(0.0,0.0,0.0);
  assert(IsEqual(Point::Distance(point_a, point_b),1.0));
  assert(IsEqual(point_b.norm(), 0.0));
  
  Point point_c(1.0,2.0,-1.0);
  assert(IsEqual(Point::Distance(point_a,point_c), Point::Distance(point_c,point_a)));
  assert(IsEqual(Point::Distance(point_a,point_c), sqrt(4.0+1.0+4.0)));
  assert(IsEqual(Point::DotProduct(point_a,point_c), -1.0));
  assert(IsEqual(point_c.norm(), sqrt(1.0+pow(2.0,2)+1.0)));
  
  Point point_d(1.0,0.0,1.0);
  assert(IsEqual(point_d.phi(), 0.0));
  assert(IsEqual(point_d.theta(), pi/4.0));
  assert(IsEqual(point_d.norm(), sqrt(2.0)));
  assert(IsEqual(Point::Theta(point_b,point_d), pi/4.0));
  
  Point point_e(0.0,1.0,0.0);
  assert(IsEqual(point_e.phi(), pi/2.0));
  assert(IsEqual(point_e.theta(), pi/2.0));
  assert(IsEqual(Point::Theta(point_e,point_b), pi/2.0));
  assert(IsEqual(Point::Theta(point_b,point_e), pi/2.0));
  assert(IsEqual(Point::Phi(point_b,point_e), pi/2.0));
  assert(IsEqual(Point::Phi(point_e,point_b), -pi/2.0));
  
  assert(IsEqual(AngleBetweenDirections(point_a.theta(), point_a.phi(),
                                        point_d.theta(), point_d.phi()),
                 pi/4.0));
  assert(IsEqual(AngleBetweenDirections(point_d.theta(), point_d.phi(),
                                        point_a.theta(), point_a.phi()),
                 pi/4.0));
  
  assert(Point::RotateAboutX(point_e, 0.0).Equals(point_e));
  assert(Point::RotateAboutY(point_e, 0.0).Equals(point_e));
  assert(Point::RotateAboutZ(point_e, 0.0).Equals(point_e));
  
  assert(Point::RotateAboutX(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(Point::RotateAboutX(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(Point::RotateAboutX(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(Point::RotateAboutX(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  assert(Point::RotateAboutX(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  assert(Point::RotateAboutX(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  
  assert(Point::RotateAboutY(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,0.0,-1.0)));
  assert(Point::RotateAboutY(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(Point::RotateAboutY(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(Point::RotateAboutY(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(Point::RotateAboutY(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(Point::RotateAboutY(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  
  assert(Point::RotateAboutZ(Point(1.0,0.0,0.0), pi/2.0).Equals(Point(0.0,1.0,0.0)));
  assert(Point::RotateAboutZ(Point(1.0,0.0,0.0), -pi/2.0).Equals(Point(0.0,-1.0,0.0)));
  assert(Point::RotateAboutZ(Point(0.0,1.0,0.0), pi/2.0).Equals(Point(-1.0,0.0,0.0)));
  assert(Point::RotateAboutZ(Point(0.0,1.0,0.0), -pi/2.0).Equals(Point(1.0,0.0,0.0)));
  assert(Point::RotateAboutZ(Point(0.0,0.0,1.0), pi/2.0).Equals(Point(0.0,0.0,1.0)));
  assert(Point::RotateAboutZ(Point(0.0,0.0,1.0), -pi/2.0).Equals(Point(0.0,0.0,1.0)));
  
  assert(Point::RotateAboutX(Point(1.5,0.0,0.0), pi/2.0).Equals(Point(1.5,0.0,0.0)));
  assert(Point::RotateAboutY(Point(0.0,1.5,0.0), pi/2.0).Equals(Point(0.0,1.5,0.0)));
  assert(Point::RotateAboutZ(Point(0.0,0.0,1.5), pi/2.0).Equals(Point(0.0,0.0,1.5)));
  
  Point point_f = Point::RotateAboutX(Point::RotateAboutY(
                         Point::RotateAboutZ(Point(1.5,-1.0,0.5),pi/5.0),pi/6.0),pi/7.0);
  
  assert((Point::Rotate(Point(1.5,-1.0,0.5), pi/5.0, pi/6.0, pi/7.0)).Equals(point_f));
  assert((Point::Rotate(Point(0.0,0.0,1.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,0.0,1.0)));
  assert((Point::Rotate(Point(0.0,0.0,1.0), 0.0, pi/2.0, 0.0)).Equals(Point(1.0,0.0,0.0)));
  assert((Point::Rotate(Point(0.0,0.0,1.0), 0.0, 0.0, pi/2.0)).Equals(Point(0.0,-1.0,0.0)));
  assert((Point::Rotate(Point(1.0,0.0,0.0), pi/2.0, 0.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  assert((Point::Rotate(Point(0.0,1.0,0.0), 0.0, pi/2.0, 0.0)).Equals(Point(0.0,1.0,0.0)));
  assert((Point::Rotate(Point(0.0,1.0,0.0), -PI/2.0, -PI/2.0, 0.0)).Equals(Point(0.0,0.0,1.0)));

  
  assert(Point::PointSpherical(1.0, PI/2.0, 0.0).Equals(Point(1.0, 0.0, 0.0)));
  assert(Point::PointSpherical(2.0, PI/2.0, 0.0).Equals(Point(2.0, 0.0, 0.0)));
  assert(Point::PointSpherical(1.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 1.0)));
  assert(Point::PointSpherical(2.0, 0.0, 0.0).Equals(Point(0.0, 0.0, 2.0)));
  assert(Point::PointSpherical(1.5, PI/2.0, PI/2.0).
         Equals(Point(0.0, 1.5, 0.0)));
  assert(Point::PointSpherical(1.0, -PI/2.0, 0.0).
         Equals(Point(-1.0, 0.0, 0.0)));

  assert(Point::PointSpherical(1.0, PI/4.0, 0.0).
         Equals(Point(1.0/sqrt(2.0), 0.0, 1.0/sqrt(2.0))));
  
  
  assert(Point::PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  
  assert(Point::Sum(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Point::Sum(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Point::Sum(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(1.5,0.0,2.0)));      
  
  assert(Point::Subtract(Point(1.0,-1.0,2.0), Point(0.0,0.0,0.0)).Equals(Point(1.0,-1.0,2.0)));
  assert(Point::Subtract(Point(0.0,0.0,0.0), Point(1.0,-1.0,2.0)).Equals(Point(-1.0,1.0,-2.0)));
  assert(Point::Subtract(Point(0.5,1.0,-0.0), Point(1.0,-1.0,2.0)).Equals(Point(-0.5,2.0,-2.0)));   
  
  assert(Point::PointOnLine(Point(0,0,0), Point(1,0,0), 0.3).Equals(Point(0.3,0,0)));
  assert(Point::PointOnLine(Point(1,0,0), Point(0,0,0), 0.3).Equals(Point(0.7,0,0)));
  assert(Point::PointOnLine(Point(0,0,0), Point(0,1,0), 0.3).Equals(Point(0,0.3,0)));
  assert(Point::PointOnLine(Point(0,1,0), Point(0,0,0), 0.3).Equals(Point(0,0.7,0)));
  assert(Point::PointOnLine(Point(0,0,0), Point(2,2,0), 1.0).Equals(Point(1.0/sqrt(2.0),1.0/sqrt(2.0),0)));
  assert(Point::PointOnLine(Point(0,0,0), Point(0,2,2), 1.0).Equals(Point(0,1.0/sqrt(2.0),1.0/sqrt(2.0))));
  assert(Point::PointOnLine(Point(0,2,2), Point(0,0,0), 1.0).Equals(Point(0,2-1.0/sqrt(2.0),2-1.0/sqrt(2.0))));
  assert(Point::PointOnLine(Point(1,1,0), Point(2,2,0), 1.0).Equals(Point(1.0+1.0/sqrt(2.0),1.0+1.0/sqrt(2.0),0)));
  assert(Point::PointOnLine(Point(1,1,0), Point(1,3,0), 1.0).Equals(Point(1,2.0,0)));
  assert(Point::PointOnLine(Point(1,1,0), Point(3,1,0), 1.0).Equals(Point(2.0,1,0)));
  
  assert(Point::PointSpherical(1.0, PI/4.0, PI/2.0).
         Equals(Point(0.0, 1.0/sqrt(2.0), 1.0/sqrt(2.0))));
  
  Point point_g(1.0, 2.0, 1.5);
  Point point_h = Point::Normalized(point_g);
  assert(Point::IsEqual(point_g, Point(1.0, 2.0, 1.5)));
  assert(IsEqual(point_h.norm(), 1.0));
  assert(Point::IsEqual(point_h, Point(0.371390676354104,
                                       0.742781352708207,
                                       0.557086014531156), VERY_SMALL));
  
  return true;
}

} // namespace sat
