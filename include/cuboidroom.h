/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SAL_CUBOIDROOM_H
#define SAL_CUBOIDROOM_H


#include <vector>
#include "point.h"
#include "iirfilter.h"
#include "room.h"
#include "comparisonop.h"
#include "exception.h"

#ifdef SAL_EXPORTS
  #define SAL_API __declspec(dllexport)
#else
  #define SAL_API
#endif

namespace sal {

enum CuboidWallId {
  x1,
  x2,
  y1,
  y2,
  z1,
  z2
};
  
class SAL_API CuboidRoom : public Room {
public:
  // The room filter is *not* an injected dependency. The software will do lots
  // of copies of the object.
  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const std::vector<mcl::IirFilter>& filters,
             const BoundarySetType boundary_set_type = first_order_only) :
          Room(filters, boundary_set_type), x_(x), y_(y), z_(z) {
    if (filters.size() != num_faces()) { throw_line(""); }
  }
  
  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const mcl::IirFilter& filter,
             const BoundarySetType boundary_set_type = first_order_only) :
          Room(std::vector<mcl::IirFilter>(6, filter),
               boundary_set_type),
          x_(x), y_(y), z_(z) {}
  
  CuboidRoom(const mcl::Point& room_dimensions,
             const mcl::IirFilter& filter) :
  Room(std::vector<mcl::IirFilter>(6, filter)),
  x_(room_dimensions.x()), y_(room_dimensions.y()), z_(room_dimensions.z()) {}

  virtual std::vector<mcl::Point> CalculateBoundaryPoints(const mcl::Point& source,
                                                          const mcl::Point& destination);
  
  virtual std::vector<mcl::IirFilter> GetBoundaryFilters(const mcl::Point& source_point,
                                                         const mcl::Point& mic_point);
  
  virtual mcl::UInt num_boundary_points();
  
  mcl::Point ImageSourcePosition(const mcl::Point& source_position,
                                 const mcl::Int mx,
                                 const mcl::Int my,
                                 const mcl::Int mz,
                                 const mcl::Int px,
                                 const mcl::Int py,
                                 const mcl::Int pz);
  
  sal::Time SabineRt60();
  
  sal::Length x() { return x_; }
  
  sal::Length y() { return y_; }
  
  sal::Length z() { return z_; }
  
  void set_x(sal::Length x) { x_ = x; }
  
  void set_y(sal::Length y) { y_ = y; }
  
  void set_z(sal::Length z) { z_ = z; }
  
  void set_dimensions(mcl::Point dimensions) {
    x_ = dimensions.x();
    y_ = dimensions.y();
    z_ = dimensions.z();
  }
  
  //  Reference system:
  //
  //  ^ z
  //  |
  //  ---------------
  //  |\            |\
  //  | \     0     | \
  //  |  \          |  \     y
  //  ---2\-----------3-\---->
  //   \   ---------------
  //    \  |          \  |
  //     \ |       1   \ |
  //      \|            \|
  //       ---------------
  //        \
  //         \ x
  //
  // 4: floor
  // 5: ceiling
  mcl::Point ReflectionPoint(const CuboidWallId face_index,
                             const mcl::Point& source_pos,
                             const mcl::Point& observation_pos);
  
  virtual sal::UInt num_faces() const { return 6; }
  
  virtual sal::Length max_distance() const {
    return mcl::Point(x_, y_, z_).norm();
  }
  
  static bool Test();
  
  virtual bool IsPointInRoom(const mcl::Point& point,
                             const sal::Length precision = VERY_SMALL) const;
  
private:
  
  // Dimensions of the cuboid
  sal::Length x_;
  sal::Length y_;
  sal::Length z_;
  
  mcl::Point IntersectionPoint(const CuboidWallId wall_id,
                               const mcl::Point& observation_pos,
                               const mcl::Point& image_pos);
};

} // namespace sal
  
#endif
