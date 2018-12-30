/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "vector.hpp"
#include "point.hpp"
#include "digitalfilter.hpp"
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

enum BoundarySetType
{
  kFirstOrder,
  kFirstAndSecondOrder
};

template<typename T>
class CuboidRoom
{
public:
  // The room filter is *not* an injected dependency. The software will do lots
  // of copies of the object.
  CuboidRoom(
    const Length x,
    const Length y,
    const Length z,
    const mcl::Vector<mcl::DigitalFilter<T>>& filter_prototypes,
    const BoundarySetType boundary_set_type = kFirstOrder)
    : wall_filters_(filter_prototypes)
    , dimensions_(Triplet(x, y, z))
    , origin_position_(Triplet(0, 0, 0))
    , boundary_set_type_(boundary_set_type)
  {
    if (filter_prototypes.size() != num_faces())
    {
      ASSERT(false);
    }
  }


  CuboidRoom(
    const Length x,
    const Length y,
    const Length z,
    const mcl::DigitalFilter<T>& filter_prototype,
    const BoundarySetType boundary_set_type = kFirstOrder)
    : wall_filters_(mcl::Vector<mcl::DigitalFilter<T>>(6, filter_prototype))
    , dimensions_(Triplet(x, y, z))
    , origin_position_(Triplet(0, 0, 0))
    , boundary_set_type_(boundary_set_type)
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
    const mcl::DigitalFilter<T>& filter_prototype,
    const BoundarySetType boundary_set_type = kFirstOrder)
    : wall_filters_(mcl::Vector<mcl::DigitalFilter<T>>(6, filter_prototype))
    , dimensions_(room_dimensions)
    , origin_position_(origin_position)
    , boundary_set_type_(boundary_set_type)
  {
  }


  const mcl::Vector<mcl::DigitalFilter<T>>& GetWallFilters() const
  {
    return wall_filters_;
  }

  Point
  GetBoundaryPoint(
    const size_t wall_id,
    const Point& source,
    const Point& destination) const noexcept;

  mcl::Vector<mcl::DigitalFilter<T>>
  GetBoundaryFilters(
    const Point& source_point,
    const Point& mic_point) const noexcept;

  size_t num_boundary_points() const noexcept;

  Point ImageSourcePosition(
    const Point& source_position,
    mcl::Int mx,
    mcl::Int my,
    mcl::Int mz,
    mcl::Int px,
    mcl::Int py,
    mcl::Int pz) const noexcept;

  Time SabineRt60() const;


  Triplet dimensions() const noexcept
  {
    return dimensions_;
  }


  void SetDimensions(
    const Triplet& dimensions) noexcept
  {
    dimensions_ = dimensions;
  }


  void SetOriginPosition(
    const Triplet& position)
  {
    origin_position_ = position;
  }


  size_t num_faces() const noexcept
  {
    return 6;
  }


  Length max_distance() const noexcept
  {
    return dimensions_.norm();
  }


  static bool Test();

  bool
  IsPointInRoom(
    const Point& point,
    Length precision = VERY_SMALL) const noexcept;

  std::string ShapeDescription() const noexcept;


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
  Point ReflectionPoint(
    CuboidWallId face_index,
    const Point& source_pos,
    const Point& observation_pos) const;

  // Dimensions of the cuboid
  Triplet dimensions_;

  Triplet origin_position_;

  static Point IntersectionPoint(
    CuboidWallId wall_id,
    Triplet dimensions,
    const Point& observation_pos,
    const Point& image_pos) noexcept;
  
  // Member variables
  mcl::Vector<mcl::DigitalFilter<T>> wall_filters_;
  BoundarySetType boundary_set_type_;
};
} // namespace sal

#include "cuboidroom.ipp"
