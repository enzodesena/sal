/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_CUBOIDROOM_H
#define SAL_CUBOIDROOM_H

#include <vector>

#include "comparisonop.h"
#include "iirfilter.h"
#include "point.h"
#include "room.h"
#include "salutilities.h"

namespace sal {

enum CuboidWallId { kX1, kX2, kY1, kY2, kZ1, kZ2 };

class CuboidRoom : public Room {
 public:
  // The room filter is *not* an injected dependency. The software will do lots
  // of copies of the object.
  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const std::vector<dsp::IirFilter>& filter_prototypes)
      : Room(filter_prototypes),
        dimensions_(Triplet(x, y, z)),
        origin_position_(Triplet(0, 0, 0)) {
    if ((Int)filter_prototypes.size() != num_faces()) {
      ASSERT(false);
    }
  }

  CuboidRoom(sal::Length x, sal::Length y, sal::Length z,
             const dsp::IirFilter& filter_prototype)
      : Room(std::vector<dsp::IirFilter>(6, filter_prototype)),
        dimensions_(Triplet(x, y, z)),
        origin_position_(Triplet(0, 0, 0)) {}

  /**
   Constructrs a cuboid room.
   @param[in] room_dimensions A triplet containing the 3 dimensions of the room
   @param[in] origin_position A triplet containing the origin of the room,
   i.e. the smallest
   @param[in] filter_prototype Prototype of the wall filter.
   */
  CuboidRoom(const Triplet& room_dimensions, const Triplet& origin_position,
             const dsp::IirFilter& filter_prototype)
      : Room(std::vector<dsp::IirFilter>(6, filter_prototype)),
        dimensions_(room_dimensions),
        origin_position_(origin_position) {}

  virtual std::vector<dsp::Point> CalculateBoundaryPoints(
      const dsp::Point& source, const dsp::Point& destination) const noexcept;

  virtual std::vector<dsp::IirFilter> GetBoundaryFilters(
      const dsp::Point& source_point,
      const dsp::Point& mic_point) const noexcept;

  virtual dsp::Int num_boundary_points() const noexcept;

  dsp::Point ImageSourcePosition(const dsp::Point& source_position,
                                 const dsp::Int mx, const dsp::Int my,
                                 const dsp::Int mz, const dsp::Int px,
                                 const dsp::Int py,
                                 const dsp::Int pz) const noexcept;

  sal::Time SabineRt60() const;

  Triplet dimensions() const noexcept { return dimensions_; }

  void SetDimensions(const Triplet& dimensions) noexcept {
    dimensions_ = dimensions;
  }

  void SetOriginPosition(const Triplet& position) {
    origin_position_ = position;
  }

  virtual sal::Int num_faces() const noexcept { return 6; }

  virtual sal::Length max_distance() const noexcept {
    return dimensions_.norm();
  }

  static bool Test();

  virtual bool IsPointInRoom(
      const dsp::Point& point,
      const sal::Length precision = VERY_SMALL) const noexcept;

  virtual std::string ShapeDescription() const noexcept;

  virtual ~CuboidRoom() {}

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
  dsp::Point ReflectionPoint(const CuboidWallId face_index,
                             const dsp::Point& source_pos,
                             const dsp::Point& observation_pos) const;

  // Dimensions of the cuboid
  Triplet dimensions_;

  Triplet origin_position_;

  static dsp::Point IntersectionPoint(const CuboidWallId wall_id,
                                      const Triplet dimensions,
                                      const dsp::Point& observation_pos,
                                      const dsp::Point& image_pos) noexcept;
};

}  // namespace sal

#endif
