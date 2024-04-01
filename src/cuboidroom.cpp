/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "cuboidroom.h"
#include <vector>
#include <cassert>
#define EPSILON 1E-10


namespace sal {
  
using sal::Length;
using mcl::Point;
using mcl::Int;
using sal::Int;
using sal::Time;
using sal::Sample;
using mcl::IsLargerOrEqual;
using mcl::IsSmallerOrEqual;


bool CuboidRoom::IsPointInRoom(const Point& point,
                               const Length wall_distance) const noexcept {
  return std::isgreaterequal(point.x(), origin_position_.x() + wall_distance) &&
         std::isgreaterequal(point.y(), origin_position_.y() + wall_distance) &&
         std::isgreaterequal(point.z(), origin_position_.z() + wall_distance) &&
         std::islessequal(point.x(), origin_position_.x() + dimensions_.x() - wall_distance) &&
         std::islessequal(point.y(), origin_position_.y() + dimensions_.y() - wall_distance) &&
         std::islessequal(point.z(), origin_position_.z() + dimensions_.z() - wall_distance);
}

std::vector<Point> CuboidRoom::CalculateBoundaryPoints(const Point& source_point,
                                                       const Point& mic_point) const noexcept {
  // These points are normalised such that they are between 0<x<Lx etc...
  mcl::Point shifted_source_point = mcl::Subtract(source_point, origin_position_);
  mcl::Point shifted_mic_point = mcl::Subtract(mic_point, origin_position_);
  std::vector<Point> reflection_points(6);
  
  reflection_points[0] = ReflectionPoint(kX1, shifted_source_point, shifted_mic_point);
  reflection_points[1] = ReflectionPoint(kX2, shifted_source_point, shifted_mic_point);
  reflection_points[2] = ReflectionPoint(kY1, shifted_source_point, shifted_mic_point);
  reflection_points[3] = ReflectionPoint(kY2, shifted_source_point, shifted_mic_point);
  reflection_points[4] = ReflectionPoint(kZ1, shifted_source_point, shifted_mic_point);
  reflection_points[5] = ReflectionPoint(kZ2, shifted_source_point, shifted_mic_point);
  
  ASSERT(mcl::IsEqual(reflection_points[0].x(), 0.0));
  ASSERT(mcl::IsEqual(reflection_points[1].x(), dimensions_.x()));
  ASSERT(mcl::IsEqual(reflection_points[2].y(), 0.0));
  ASSERT(mcl::IsEqual(reflection_points[3].y(), dimensions_.y()));
  ASSERT(mcl::IsEqual(reflection_points[4].z(), 0.0));
  ASSERT(mcl::IsEqual(reflection_points[5].z(), dimensions_.z()));
  
  if (boundary_set_type_ == kFirstAndSecondOrder) {
    Point a_kX2 = IntersectionPoint(kX2, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 1,0,0, 1,1,0));
    Point a_kY1 = IntersectionPoint(kY1, dimensions_, shifted_mic_point,
                                   ImageSourcePosition(shifted_source_point, 1,0,0, 1,1,0));
    if (IsPointInRoom(a_kX2, EPSILON)) {
      ASSERT(!IsPointInRoom(a_kY1, EPSILON));
      reflection_points.push_back(a_kX2);
    } else {
      ASSERT(!IsPointInRoom(a_kX2, EPSILON));
      reflection_points.push_back(a_kY1);
    }
    
    Point b_kX2 = IntersectionPoint(kX2, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 1,1,0, 1,1,0));
    Point b_kY2 = IntersectionPoint(kY2, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 1,1,0, 1,1,0));
    if (IsPointInRoom(b_kX2, EPSILON)) {
      ASSERT(!IsPointInRoom(b_kY2, EPSILON));
      reflection_points.push_back(b_kX2);
    } else {
      ASSERT(!IsPointInRoom(b_kX2, EPSILON));
      reflection_points.push_back(b_kY2);
    }
    
    Point c_kX1 = IntersectionPoint(kX1, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 0,1,0, 1,1,0));
    Point c_kY2 = IntersectionPoint(kY2, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 0,1,0, 1,1,0));
    if (IsPointInRoom(c_kX1, EPSILON)) {
      ASSERT(!IsPointInRoom(c_kY2, EPSILON));
      reflection_points.push_back(c_kX1);
    } else {
      ASSERT(!IsPointInRoom(c_kX1, EPSILON));
      reflection_points.push_back(c_kY2);
    }
    
    Point d_kX1 = IntersectionPoint(kX1, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 0,0,0, 1,1,0));
    Point d_kY1 = IntersectionPoint(kY1, dimensions_,
                                   shifted_mic_point, ImageSourcePosition(shifted_source_point, 0,0,0, 1,1,0));
    if (IsPointInRoom(d_kX1, EPSILON)) {
      ASSERT(!IsPointInRoom(d_kY1, EPSILON));
      reflection_points.push_back(d_kX1);
    } else {
      ASSERT(!IsPointInRoom(d_kX1, EPSILON));
      reflection_points.push_back(d_kY1);
    }
  }
  
  for (Int i=0; i<(Int) reflection_points.size(); ++i) {
    reflection_points[i] = mcl::Sum(reflection_points[i], origin_position_);
  }
  
  ASSERT(mcl::IsEqual(reflection_points[0].x(), origin_position_.x()));
  ASSERT(mcl::IsEqual(reflection_points[1].x(), dimensions_.x()+origin_position_.x()));
  ASSERT(mcl::IsEqual(reflection_points[2].y(), origin_position_.y()));
  ASSERT(mcl::IsEqual(reflection_points[3].y(), dimensions_.y()+origin_position_.y()));
  ASSERT(mcl::IsEqual(reflection_points[4].z(), origin_position_.z()));
  ASSERT(mcl::IsEqual(reflection_points[5].z(), dimensions_.z()+origin_position_.z()));
  
  return reflection_points;
}

std::vector<mcl::IirFilter>
CuboidRoom::GetBoundaryFilters(const Point& source_point,
                               const Point& mic_point) const noexcept {
  std::vector<mcl::IirFilter> boundary_filters(wall_filters_);
  
  if (boundary_set_type_ == kFirstAndSecondOrder) {
    Point a_kX2 = IntersectionPoint(kX2, dimensions_, mic_point,
                                   ImageSourcePosition(source_point, 1,0,0, 1,1,0));
    if (IsPointInRoom(a_kX2, EPSILON)) {
      boundary_filters.push_back(boundary_filters[kX2]);
    } else {
      boundary_filters.push_back(boundary_filters[kY1]);
    }
    
    Point b_kX2 = IntersectionPoint(kX2, mic_point, dimensions_,
                                   ImageSourcePosition(source_point, 1,1,0, 1,1,0));
    if (IsPointInRoom(b_kX2, EPSILON)) {
      boundary_filters.push_back(boundary_filters[kX2]);
    } else {
      boundary_filters.push_back(boundary_filters[kY2]);
    }
    
    Point c_kX1 = IntersectionPoint(kX2, mic_point, dimensions_,
                                   ImageSourcePosition(source_point, 0,1,0, 1,1,0));
    if (IsPointInRoom(c_kX1, EPSILON)) {
      boundary_filters.push_back(boundary_filters[kX1]);
    } else {
      boundary_filters.push_back(boundary_filters[kY2]);
    }
    
    Point d_kX1 = IntersectionPoint(kX1, mic_point, dimensions_,
                                   ImageSourcePosition(source_point, 0,0,0, 1,1,0));
    if (IsPointInRoom(d_kX1, EPSILON)) {
      boundary_filters.push_back(boundary_filters[kX1]);
    } else {
      boundary_filters.push_back(boundary_filters[kY1]);
    }
  }
  return boundary_filters;
}

mcl::Int CuboidRoom::num_boundary_points() const noexcept {
  switch (boundary_set_type_) {
    case kFirstOrder:
      return 6;
      break;
    case kFirstAndSecondOrder:
      return 10;
      break;
    default:
      ASSERT(false);
      return (mcl::UInt) NAN;
      break;
  }
}


Point CuboidRoom::IntersectionPoint(const CuboidWallId wall_id,
                                    const Triplet dimensions,
                                    const Point& observation_pos,
                                    const Point& image_pos) noexcept {
  Point image_position;
  Point plane_point;
  Point plane_normal;
  
  switch (wall_id) {
    case kX1: // x_1
      plane_point = Point(0,0,0);
      plane_normal = Point(1,0,0);
      break;
    case kX2: // x_2
      plane_point = Point(dimensions.x(),0,0);
      plane_normal = Point(1,0,0);
      break;
    case kY1: // y_1
      plane_point = Point(0,0,0);
      plane_normal = Point(0,1,0);
      break;
    case kY2: // y_2
      plane_point = Point(0,dimensions.y(),0);
      plane_normal = Point(0,1,0);
      break;
    case kZ1: // z_1
      plane_point = Point(0,0,0);
      plane_normal = Point(0,0,1);
      break;
    case kZ2: // z_2
      plane_point = Point(0,0,dimensions.z());
      plane_normal = Point(0,0,1);
      break;
    default:
      ASSERT(false);
  }
  
  return mcl::IntersectionPlaneLine(observation_pos,
                                    mcl::Subtract(image_pos, observation_pos),
                                    plane_point,
                                    plane_normal);
}


Point CuboidRoom::ReflectionPoint(const CuboidWallId wall_id,
                                  const Point& source_pos,
                                  const Point& observation_pos) const {
  Point image_position;
  Point plane_point;
  Point plane_normal;
  
  // For the following conversions, see Enzo's notes.
  switch (wall_id) {
    case kX1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 1,0,0);
      break;
    case kX2:
      image_position = ImageSourcePosition(source_pos, 1,0,0, 1,0,0);
      break;
    case kY1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 0,1,0);
      break;
    case kY2:
      image_position = ImageSourcePosition(source_pos, 0,1,0, 0,1,0);
      break;
    case kZ1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 0,0,1);
      break;
    case kZ2:
      image_position = ImageSourcePosition(source_pos, 0,0,1, 0,0,1);
      break;
    default:
      ASSERT(false);
  }
  return IntersectionPoint(wall_id, dimensions_, observation_pos,
                           image_position);
}


Time CuboidRoom::SabineRt60() const {
  Length volume = dimensions_.x()*dimensions_.y()*dimensions_.z();
  Length weighted_area = 0.0;
  
  for (Int i=0; i<6; ++i) {
    // Assert that all of them are simple gain filters
    // TODO: Implement for frequency dependent ones
    ASSERT(wall_filters()[i].B().size() == 1);
    ASSERT(wall_filters()[i].A().size() == 1);
    ASSERT(mcl::IsEqual(wall_filters()[i].A()[0], 1.0));
    Sample beta = wall_filters()[i].B()[0];
    Sample alpha = 1.0-pow(beta, 2.0);
    
    Length area;
    switch (i) {
      case 0:   // x_1
      case 1: { // x_2
        area = dimensions_.y()*dimensions_.z();
        break;
      }
      case 2:   // y_1
      case 3: { // y_2
        area = dimensions_.x()*dimensions_.z();
        break;
      }
      case 4:   // z_1
      case 5: { // z_2
        area = dimensions_.x()*dimensions_.y();
        break;
      }
    }
    
    weighted_area += area * alpha;
  }
  
  return 0.161*volume/weighted_area;
}

mcl::Point CuboidRoom::ImageSourcePosition(const Point& source_position,
                                           const Int mx,
                                           const Int my,
                                           const Int mz,
                                           const Int px,
                                           const Int py,
                                           const Int pz) const noexcept {
  Length r2l_x = 2.0*dimensions_.x()*((sal::Length)mx);
  Length r2l_y = 2.0*dimensions_.y()*((sal::Length)my);
  Length r2l_z = 2.0*dimensions_.z()*((sal::Length)mz);
  return Point((1.0-2.0*((Length)px))*source_position.x()+r2l_x,
               (1.0-2.0*((Length)py))*source_position.y()+r2l_y,
               (1.0-2.0*((Length)pz))*source_position.z()+r2l_z);
}
  
  
std::string CuboidRoom::ShapeDescription() const noexcept {
  return "The room is rectangular with " +
      ToString(origin_position_.x()) + "<x<" +
      ToString(origin_position_.x()+dimensions_.x()) + " " +
      ToString(origin_position_.y()) + "<y<" +
      ToString(origin_position_.y()+dimensions_.y()) + " " +
      ToString(origin_position_.z()) + "<z<" +
      ToString(origin_position_.z()+dimensions_.z()) + " [m].";
}
  
} // namespace sal
