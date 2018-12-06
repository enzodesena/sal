/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_CUBOIDROOM_H
#define SAL_CUBOIDROOM_H

#include "vector.hpp"
#include "point.hpp"
#include "iirfilter.hpp"
#include "room.hpp"
#include "comparisonop.hpp"
#include "salutilities.hpp"

namespace sal
{
enum CuboidWallId
{
  kX1,
  kX2,
  kY1,
  kY2,
  kZ1,
  kZ2
};

class CuboidRoom : public Room
{
public:
  // The room filter is *not* an injected dependency. The software will do lots
  // of copies of the object.
  CuboidRoom(
    Length x,
    Length y,
    Length z,
    const mcl::Vector<mcl::IirFilter>& filter_prototypes)
    : Room(filter_prototypes)
    , dimensions_(Triplet(x, y, z))
    , origin_position_(Triplet(0, 0, 0)) { if ((Int)filter_prototypes.size() != num_faces()) { ASSERT(false); } }

  CuboidRoom(
    Length x,
    Length y,
    Length z,
    const mcl::IirFilter& filter_prototype)
    : Room(mcl::Vector<mcl::IirFilter>(6, filter_prototype))
    , dimensions_(Triplet(x, y, z))
    , origin_position_(Triplet(0, 0, 0))
  {
  }

  /**
   Constructrs a cuboid room.
   @param[in] room_dimensions A triplet containing the 3 dimensions of the room
   @param[in] origin_position A triplet containing the origin of the room,
   i.e. the smallest
   @param[in] filter_prototype Prototype of the wall filter.
   */
  CuboidRoom(
    const Triplet& room_dimensions,
    const Triplet& origin_position,
    const mcl::IirFilter& filter_prototype)
    : Room(mcl::Vector<mcl::IirFilter>(6, filter_prototype))
    , dimensions_(room_dimensions)
    , origin_position_(origin_position)
  {
  }

  mcl::Vector<mcl::Point>
  CalculateBoundaryPoints(
    const mcl::Point& source,
    const mcl::Point& destination) const noexcept override;

  mcl::Vector<mcl::IirFilter>
  GetBoundaryFilters(
    const mcl::Point& source_point,
    const mcl::Point& mic_point) const noexcept override;

  mcl::Int num_boundary_points() const noexcept override;

  mcl::Point ImageSourcePosition(
    const mcl::Point& source_position,
    mcl::Int mx,
    mcl::Int my,
    mcl::Int mz,
    mcl::Int px,
    mcl::Int py,
    mcl::Int pz) const noexcept;

  Time SabineRt60() const;

  Triplet dimensions() const noexcept { return dimensions_; }

  void SetDimensions(
    const Triplet& dimensions) noexcept { dimensions_ = dimensions; }

  void SetOriginPosition(
    const Triplet& position) { origin_position_ = position; }

  Int num_faces() const noexcept override { return 6; }

  Length max_distance() const noexcept override { return dimensions_.norm(); }

  static bool Test();

  bool
  IsPointInRoom(
    const mcl::Point& point,
    Length precision = VERY_SMALL) const noexcept override;

  std::string ShapeDescription() const noexcept override;

  virtual ~CuboidRoom()
  {
  }

private:

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
  mcl::Point ReflectionPoint(
    CuboidWallId face_index,
    const mcl::Point& source_pos,
    const mcl::Point& observation_pos) const;

  // Dimensions of the cuboid
  Triplet dimensions_;

  Triplet origin_position_;

  static mcl::Point IntersectionPoint(
    CuboidWallId wall_id,
    Triplet dimensions,
    const mcl::Point& observation_pos,
    const mcl::Point& image_pos) noexcept;
};
} // namespace sal

#endif
