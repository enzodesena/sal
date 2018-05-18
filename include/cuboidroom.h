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
#include "salutilities.h"

namespace sal {

enum CuboidWallId {
  x1,
  x2,
  y1,
  y2,
  z1,
  z2
};
  
class CuboidRoom : public Room {
public:
  // The room filter is *not* an injected dependency. The software will do lots
  // of copies of the object.
  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const std::vector<mcl::IirFilter>& filters,
             const BoundarySetType boundary_set_type = first_order_only) :
          Room(filters, boundary_set_type), dimensions_(Triplet(x, y, z)) {
    if ((Int)filters.size() != num_faces()) { ASSERT(false); }
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

  virtual std::vector<mcl::Point>
  CalculateBoundaryPoints(const mcl::Point& source,
                          const mcl::Point& destination) const noexcept;
  
  virtual std::vector<mcl::IirFilter>
  GetBoundaryFilters(const mcl::Point& source_point,
                     const mcl::Point& mic_point) const noexcept;
  
  virtual mcl::Int num_boundary_points() const noexcept;
  
  mcl::Point ImageSourcePosition(const mcl::Point& source_position,
                                 const mcl::Int mx,
                                 const mcl::Int my,
                                 const mcl::Int mz,
                                 const mcl::Int px,
                                 const mcl::Int py,
                                 const mcl::Int pz) const noexcept;
  
  sal::Time SabineRt60() const;
  
  Triplet dimensions() const noexcept { return dimensions_.value(); }
  
  sal::Length x() const noexcept { return dimensions_.value().x(); }
  
  sal::Length y() const noexcept { return dimensions_.value().y(); }
  
  sal::Length z() const noexcept { return dimensions_.value().z(); }
  
  void set_dimensions(const Triplet& dimensions) noexcept {
    dimensions_ = dimensions;
  }
  
  void set_target_dimensions(const Triplet& target_position) noexcept;
  
  void set_max_speed(const Speed max_speed) noexcept;
  
  virtual void UpdateShape(const Time time_elapsed_since_last_update) noexcept;
  
  bool HasReachedTarget() const noexcept;
  
  
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
                             const mcl::Point& observation_pos) const;
  
  virtual sal::Int num_faces() const noexcept { return 6; }
  
  virtual sal::Length max_distance() const noexcept {
    return dimensions_.value().norm();
  }
  
  static bool Test();
  
  virtual bool
  IsPointInRoom(const mcl::Point& point,
                const sal::Length precision = VERY_SMALL) const noexcept;
  
  virtual std::string ShapeDescription() const noexcept;
  
  virtual ~CuboidRoom() {}
private:
  
  // Dimensions of the cuboid
  TripletHandler dimensions_;
  
  
  static mcl::Point IntersectionPoint(const CuboidWallId wall_id,
                                      const Triplet dimensions,
                                      const mcl::Point& observation_pos,
                                      const mcl::Point& image_pos) noexcept;
};

} // namespace sal
  
#endif
