/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_ROOM_H
#define SAL_ROOM_H

#include "comparisonop.h"
#include "iirfilter.h"
#include "point.h"
#include "saltypes.h"

namespace sal {

enum BoundarySetType { kFirstOrder, kFirstAndSecondOrder };

class Room {
 public:
  // This is the standard constructor where you feed the room `shape` and for
  // each face of the shape you also provide a filter. The order in the
  // `filters` vector follows that of the employed shape.
  Room(const std::vector<dsp::Filter>& wall_filters) noexcept
      : wall_filters_(wall_filters), boundary_set_type_(kFirstOrder) {}

  const std::vector<dsp::Filter>& wall_filters() const noexcept {
    return wall_filters_;
  }

  // Resets the wall filters. Warning! It may cancel the state of the old ones,
  // with probable audible artifacts.
  void SetWallFilters(
      const std::vector<dsp::Filter>& wall_filters) noexcept {
    wall_filters_ = wall_filters;
  }

  void SetWallFilter(const Int wall_id, const dsp::Filter& filter) noexcept {
    ASSERT(wall_id >= 0 && wall_id < num_boundary_points());
    wall_filters_[wall_id] = filter;
  }

  void SetWallFilters(const dsp::Filter& filter) noexcept {
    wall_filters_.assign(num_faces(), filter);
  }

  // Returns a vector of points located at geometrical reflections,
  // relative to source and destinatin points.
  virtual std::vector<dsp::Point> CalculateBoundaryPoints(
      const dsp::Point& source,
      const dsp::Point& destination) const noexcept = 0;

  virtual std::vector<dsp::Filter> GetBoundaryFilters(
      const dsp::Point& source_point,
      const dsp::Point& mic_point) const noexcept = 0;

  virtual sal::Int num_boundary_points() const noexcept = 0;

  // Returns the shape's number of faces.
  virtual sal::Int num_faces() const noexcept = 0;

  // Returns the maximum distance between two points inside the shape.
  virtual sal::Length max_distance() const noexcept = 0;

  /** Returns true if the specified point is inside the room.
   @param[in] wall_distance This parameter allows to set also a safe distance
   form the walls. For instance, if wall_distance is set to 1 meter,
   and the point is less than 1 meter away from a wall,
   this method will return false. */
  virtual bool IsPointInRoom(
      const dsp::Point& point,
      const sal::Length wall_distance = 0.0) const noexcept = 0;

  virtual std::string ShapeDescription() const noexcept = 0;

  virtual ~Room() noexcept {}

 protected:
  std::vector<dsp::Filter> wall_filters_;
  BoundarySetType boundary_set_type_;
};

}  // namespace sal

#endif
