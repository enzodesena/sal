/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "cuboidroom.h"
#include <vector>
#include <cassert>
#define EPSILON 1E-10


namespace sal {
  
using sal::Length;
using mcl::Point;
using mcl::Int;
using sal::UInt;
using sal::Time;
using sal::Sample;
using mcl::IsLargerOrEqual;
using mcl::IsSmallerOrEqual;


bool CuboidRoom::IsPointInRoom(const Point& point,
                               const Length wall_distance) const noexcept {
  return isgreaterequal(point.x(), 0.0 + wall_distance) &&
         isgreaterequal(point.y(), 0.0 + wall_distance) &&
         isgreaterequal(point.z(), 0.0 + wall_distance) &&
         islessequal(point.x(), dimensions_.value().x() - wall_distance) &&
         islessequal(point.y(), dimensions_.value().y() - wall_distance) &&
         islessequal(point.z(), dimensions_.value().z() - wall_distance);
}

std::vector<Point> CuboidRoom::CalculateBoundaryPoints(const Point& source_point,
                                                       const Point& mic_point) noexcept {
  
  std::vector<Point> reflection_points;
  
  reflection_points.push_back(ReflectionPoint(x1, source_point, mic_point));
  reflection_points.push_back(ReflectionPoint(x2, source_point, mic_point));
  reflection_points.push_back(ReflectionPoint(y1, source_point, mic_point));
  reflection_points.push_back(ReflectionPoint(y2, source_point, mic_point));
  reflection_points.push_back(ReflectionPoint(z1, source_point, mic_point));
  reflection_points.push_back(ReflectionPoint(z2, source_point, mic_point));
  
  if (boundary_set_type_ == first_and_second_cross_horiz) {
    Point a_x2 = IntersectionPoint(x2, mic_point, ImageSourcePosition(source_point, 1,0,0, 1,1,0));
    Point a_y1 = IntersectionPoint(y1, mic_point, ImageSourcePosition(source_point, 1,0,0, 1,1,0));
    if (IsPointInRoom(a_x2, EPSILON)) {
      assert(!IsPointInRoom(a_y1, EPSILON));
      reflection_points.push_back(a_x2);
    } else {
      assert(!IsPointInRoom(a_x2, EPSILON));
      reflection_points.push_back(a_y1);
    }
    
    Point b_x2 = IntersectionPoint(x2, mic_point, ImageSourcePosition(source_point, 1,1,0, 1,1,0));
    Point b_y2 = IntersectionPoint(y2, mic_point, ImageSourcePosition(source_point, 1,1,0, 1,1,0));
    if (IsPointInRoom(b_x2, EPSILON)) {
      assert(!IsPointInRoom(b_y2, EPSILON));
      reflection_points.push_back(b_x2);
    } else {
      assert(!IsPointInRoom(b_x2, EPSILON));
      reflection_points.push_back(b_y2);
    }
    
    Point c_x1 = IntersectionPoint(x1, mic_point, ImageSourcePosition(source_point, 0,1,0, 1,1,0));
    Point c_y2 = IntersectionPoint(y2, mic_point, ImageSourcePosition(source_point, 0,1,0, 1,1,0));
    if (IsPointInRoom(c_x1, EPSILON)) {
      assert(!IsPointInRoom(c_y2, EPSILON));
      reflection_points.push_back(c_x1);
    } else {
      assert(!IsPointInRoom(c_x1, EPSILON));
      reflection_points.push_back(c_y2);
    }
    
    Point d_x1 = IntersectionPoint(x1, mic_point, ImageSourcePosition(source_point, 0,0,0, 1,1,0));
    Point d_y1 = IntersectionPoint(y1, mic_point, ImageSourcePosition(source_point, 0,0,0, 1,1,0));
    if (IsPointInRoom(d_x1, EPSILON)) {
      assert(!IsPointInRoom(d_y1, EPSILON));
      reflection_points.push_back(d_x1);
    } else {
      assert(!IsPointInRoom(d_x1, EPSILON));
      reflection_points.push_back(d_y1);
    }
  }
  
  return reflection_points;
}

std::vector<mcl::IirFilter>
CuboidRoom::GetBoundaryFilters(const Point& source_point,
                               const Point& mic_point) noexcept {
  std::vector<mcl::IirFilter> boundary_filters(wall_filters_);
  
  if (boundary_set_type_ == first_and_second_cross_horiz) {
    Point a_x2 = IntersectionPoint(x2, mic_point,
                                   ImageSourcePosition(source_point, 1,0,0, 1,1,0));
    if (IsPointInRoom(a_x2, EPSILON)) {
      boundary_filters.push_back(boundary_filters[x2]);
    } else {
      boundary_filters.push_back(boundary_filters[y1]);
    }
    
    Point b_x2 = IntersectionPoint(x2, mic_point,
                                   ImageSourcePosition(source_point, 1,1,0, 1,1,0));
    if (IsPointInRoom(b_x2, EPSILON)) {
      boundary_filters.push_back(boundary_filters[x2]);
    } else {
      boundary_filters.push_back(boundary_filters[y2]);
    }
    
    Point c_x1 = IntersectionPoint(x2, mic_point,
                                   ImageSourcePosition(source_point, 0,1,0, 1,1,0));
    if (IsPointInRoom(c_x1, EPSILON)) {
      boundary_filters.push_back(boundary_filters[x1]);
    } else {
      boundary_filters.push_back(boundary_filters[y2]);
    }
    
    Point d_x1 = IntersectionPoint(x1, mic_point,
                                   ImageSourcePosition(source_point, 0,0,0, 1,1,0));
    if (IsPointInRoom(d_x1, EPSILON)) {
      boundary_filters.push_back(boundary_filters[x1]);
    } else {
      boundary_filters.push_back(boundary_filters[y1]);
    }
  }
  return boundary_filters;
}

mcl::UInt CuboidRoom::num_boundary_points() noexcept {
  switch (boundary_set_type_) {
    case first_order_only:
      return 6;
      break;
    case first_and_second_cross_horiz:
      return 10;
      break;
    default:
      assert(false);
      assert(false);
      return (mcl::UInt) NAN;
      break;
  }
}


Point CuboidRoom::IntersectionPoint(const CuboidWallId wall_id,
                                    const Point& observation_pos,
                                    const Point& image_pos) {
  Point image_position;
  Point plane_point;
  Point plane_normal;
  
  switch (wall_id) {
    case x1: // x_1
      plane_point = Point(0,0,0);
      plane_normal = Point(1,0,0);
      break;
    case x2: // x_2
      plane_point = Point(dimensions_.value().x(),0,0);
      plane_normal = Point(1,0,0);
      break;
    case y1: // y_1
      plane_point = Point(0,0,0);
      plane_normal = Point(0,1,0);
      break;
    case y2: // y_2
      plane_point = Point(0,dimensions_.value().y(),0);
      plane_normal = Point(0,1,0);
      break;
    case z1: // z_1
      plane_point = Point(0,0,0);
      plane_normal = Point(0,0,1);
      break;
    case z2: // z_2
      plane_point = Point(0,0,dimensions_.value().z());
      plane_normal = Point(0,0,1);
      break;
    default:
      assert(false);
  }
  
  return mcl::IntersectionPlaneLine(observation_pos,
                                    mcl::Subtract(image_pos, observation_pos),
                                    plane_point,
                                    plane_normal);
}


Point CuboidRoom::ReflectionPoint(const CuboidWallId wall_id,
                                  const Point& source_pos,
                                  const Point& observation_pos) {
  Point image_position;
  Point plane_point;
  Point plane_normal;
  
  // For the following conversions, see Enzo's notes.
  switch (wall_id) {
    case x1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 1,0,0);
      break;
    case x2:
      image_position = ImageSourcePosition(source_pos, 1,0,0, 1,0,0);
      break;
    case y1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 0,1,0);
      break;
    case y2:
      image_position = ImageSourcePosition(source_pos, 0,1,0, 0,1,0);
      break;
    case z1:
      image_position = ImageSourcePosition(source_pos, 0,0,0, 0,0,1);
      break;
    case z2:
      image_position = ImageSourcePosition(source_pos, 0,0,1, 0,0,1);
      break;
    default:
      assert(false);
      assert(false);
  }
  return IntersectionPoint(wall_id, observation_pos, image_position);
}


Time CuboidRoom::SabineRt60() {
  Length volume = dimensions_.value().x()*dimensions_.value().y()*dimensions_.value().z();
  Length weighted_area = 0.0;
  
  for (UInt i=0; i<6; ++i) {
    // Assert that all of them are simple gain filters
    // TODO: Implement for frequency dependent ones
    assert(wall_filters()[i].B().size() == 1);
    assert(wall_filters()[i].A().size() == 1);
    assert(mcl::IsEqual(wall_filters()[i].A()[0], 1.0));
    Sample beta = wall_filters()[i].B()[0];
    Sample alpha = 1.0-pow(beta, 2.0);
    
    Length area;
    switch (i) {
      case 0:   // x_1
      case 1: { // x_2
        area = dimensions_.value().y()*dimensions_.value().z();
        break;
      }
      case 2:   // y_1
      case 3: { // y_2
        area = dimensions_.value().x()*dimensions_.value().z();
        break;
      }
      case 4:   // z_1
      case 5: { // z_2
        area = dimensions_.value().x()*dimensions_.value().y();
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
                                           const Int pz) noexcept {
  Length r2l_x = 2.0*dimensions_.value().x()*((sal::Length)mx);
  Length r2l_y = 2.0*dimensions_.value().y()*((sal::Length)my);
  Length r2l_z = 2.0*dimensions_.value().z()*((sal::Length)mz);
  return Point((1.0-2.0*((Length)px))*source_position.x()+r2l_x,
               (1.0-2.0*((Length)py))*source_position.y()+r2l_y,
               (1.0-2.0*((Length)pz))*source_position.z()+r2l_z);
}
  
void CuboidRoom::set_target_dimensions(const Triplet& dimensions) noexcept {
  dimensions_.set_target_value(dimensions);
}

bool CuboidRoom::HasReachedTarget() noexcept {
  return dimensions_.HasReachedTarget();
}

void CuboidRoom::set_max_speed(const Speed max_speed) noexcept {
  dimensions_.set_max_speed(max_speed);
}

void CuboidRoom::UpdateShape(const Time time_since_last_update) noexcept {
  dimensions_.Update(time_since_last_update);
}
  
std::string CuboidRoom::ShapeDescription() const noexcept {
  return "The room is rectangular with dimensions (" +
          std::to_string(dimensions_.value().x()) + ", " +
          std::to_string(dimensions_.value().y()) + ", " +
          std::to_string(dimensions_.value().z()) + ") [m].";
}
  
} // namespace sal
