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
#include "salutilities.h"

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
          Room(filters, boundary_set_type), dimensions_(Triplet(x, y, z)) {
    if (filters.size() != num_faces()) { throw_line(""); }
  }
  
  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const mcl::IirFilter& filter,
             const BoundarySetType boundary_set_type = first_order_only) :
          Room(std::vector<mcl::IirFilter>(6, filter),
               boundary_set_type), dimensions_(Triplet(x, y, z)) {}
  
  CuboidRoom(const Triplet& room_dimensions,
             const mcl::IirFilter& filter) :
          Room(std::vector<mcl::IirFilter>(6, filter)),
          dimensions_(room_dimensions) {}

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
  
  Triplet dimensions() { return dimensions_.value(); }
  
  sal::Length x() { return dimensions_.value().x(); }
  
  sal::Length y() { return dimensions_.value().y(); }
  
  sal::Length z() { return dimensions_.value().z(); }
  
  void set_dimensions(const Triplet& dimensions) {
    dimensions_ = dimensions;
  }
  
  void set_target_dimensions(const Triplet& target_position);
  
  void set_max_speed(const Speed max_speed);
  
  void UpdateDimensions(const Time time_elapsed_since_last_update);
  
  bool HasReachedTarget();
  
  
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
    return dimensions_.value().norm();
  }
  
  static bool Test();
  
  virtual bool IsPointInRoom(const mcl::Point& point,
                             const sal::Length precision = VERY_SMALL) const;
  
  virtual ~CuboidRoom() {}
private:
  
  // Dimensions of the cuboid
  TripletHandler dimensions_;
  
  
  mcl::Point IntersectionPoint(const CuboidWallId wall_id,
                               const mcl::Point& observation_pos,
                               const mcl::Point& image_pos);
};

} // namespace sal
  
#endif
