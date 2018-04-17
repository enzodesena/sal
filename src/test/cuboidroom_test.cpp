/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "cuboidroom.h"



namespace sal {

bool CuboidRoom::Test() {
  using mcl::IsEqual;
  using mcl::IirFilter;
  using mcl::Point;
  using mcl::GainFilter;
  using mcl::IdenticalFilter;
  
  IirFilter filter = IdenticalFilter();
  CuboidRoom room(1.0, 1.0, 1.0, filter);
  
  ASSERT(IsEqual(room.max_distance(), sqrt(3.0)));
  ASSERT(IsEqual(room.max_distance(), sqrt(3.0)));
  Point source_point,observ_point,res;
  
  source_point = Point(0,0,0);
  observ_point = Point(1,0,1);
  
  //  0 x_1 (old 3)
  //  1 x_2 (old 1)
  //  2 y_1 (old 0)
  //  3 y_2 (old 2)
  //  4 z_1 (old 5)
  //  5 z_2 (old 4)
  
  res = room.ReflectionPoint(x2,source_point,observ_point);
  ASSERT(res.Equals(Point(1,0,1)));
  
  res = room.ReflectionPoint(y2,source_point,observ_point);
  ASSERT(res.Equals(Point(0.5,1,0.5)));
  
  res = room.ReflectionPoint(x1,source_point,observ_point);
  ASSERT(res.Equals(Point(0,0,0)));
  
  res = room.ReflectionPoint(z2,source_point,observ_point);
  ASSERT(res.Equals(Point(1,0,1)));
  
  res = room.ReflectionPoint(z1,source_point,observ_point);
  ASSERT(res.Equals(Point(0,0,0)));
  
  
  source_point = Point(0.2,0.2,0.2);
  observ_point = Point(0.8,0.8,0.8);
  
  //  0 x_1 (old 3)
  //  1 x_2 (old 1)
  //  2 y_1 (old 0)
  //  3 y_2 (old 2)
  //  4 z_1 (old 5)
  //  5 z_2 (old 4)
  
  res = room.ReflectionPoint(x1,source_point,observ_point);
  ASSERT(res.Equals(Point(0,0.32,0.32)));
  
  res = room.ReflectionPoint(x2,source_point,observ_point);
  ASSERT(res.Equals(Point(1,1-0.32,1-0.32)));
  
  res = room.ReflectionPoint(y1,source_point,observ_point);
  ASSERT(res.Equals(Point(0.32,0,0.32)));
  
  res = room.ReflectionPoint(y2,source_point,observ_point);
  ASSERT(res.Equals(Point(1-0.32,1,1-0.32)));
  
  res = room.ReflectionPoint(z1,source_point,observ_point);
  ASSERT(res.Equals(Point(0.32,0.32,0)));
  
  res = room.ReflectionPoint(z2,source_point,observ_point);
  ASSERT(res.Equals(Point(1-0.32,1-0.32,1)));
  
  // Testing Sabine formula calculation
  
  Length edge = 3.0;
  Sample beta = 0.6;
  Sample alpha = 1.0 - pow(beta,2.0);
  CuboidRoom room_sabine(edge, edge, edge, GainFilter(beta));
  
  Time rt60 = 0.161*edge/(6.0*alpha);
  ASSERT(mcl::IsEqual(room_sabine.SabineRt60(), rt60));
  
  // Testing Sabine formula with more complicated room
  
  Length room_x = 1.0;
  Length room_y = 2.0;
  Length room_z = 3.0;
  
  std::vector<mcl::IirFilter> wall_filters;
  Sample alpha_x1 = 0.1;
  Sample beta_x1 = sqrt(1.0-alpha_x1);
  wall_filters.push_back(GainFilter(beta_x1));
  Sample alpha_x2 = 0.2;
  Sample beta_x2 = sqrt(1.0-alpha_x2);
  wall_filters.push_back(GainFilter(beta_x2));
  Sample alpha_y1 = 0.3;
  Sample beta_y1 = sqrt(1.0-alpha_y1);
  wall_filters.push_back(GainFilter(beta_y1));
  Sample alpha_y2 = 0.4;
  Sample beta_y2 = sqrt(1.0-alpha_y2);
  wall_filters.push_back(GainFilter(beta_y2));
  Sample alpha_z1 = 0.5;
  Sample beta_z1 = sqrt(1.0-alpha_z1);
  wall_filters.push_back(GainFilter(beta_z1));
  Sample alpha_z2 = 0.6;
  Sample beta_z2 = sqrt(1.0-alpha_z2);
  wall_filters.push_back(GainFilter(beta_z2));
  
  CuboidRoom room_sabine_2(room_x, room_y, room_z, wall_filters);
  
  Time rt60_2 = 0.161*(room_x*room_y*room_z)/
      (room_y*room_z*(alpha_x1+alpha_x2)+
       room_x*room_z*(alpha_y1+alpha_y2)+
       room_x*room_y*(alpha_z1+alpha_z2));
  
  ASSERT(mcl::IsEqual(room_sabine_2.SabineRt60(), rt60_2));
  
  return true;
}

} // namespace sal
