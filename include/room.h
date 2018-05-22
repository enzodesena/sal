/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_ROOM_H
#define SAL_ROOM_H

#include "saltypes.h"
#include "point.h"
#include "iirfilter.h"
#include "comparisonop.h"

namespace sal {

  
enum BoundarySetType {
  kFirstOrder,
  kFirstAndSecondOrder
};
  
  
class Room {
public:
  // This is the standard constructor where you feed the room `shape` and for
  // each face of the shape you also provide a filter. The order in the 
  // `filters` vector follows that of the employed shape.
  Room(const std::vector<mcl::IirFilter>& wall_filters) noexcept :
          wall_filters_(wall_filters), boundary_set_type_(kFirstOrder) {}
  
  const std::vector<mcl::IirFilter>& wall_filters() const noexcept {
    return wall_filters_;
  }
  
  // Resets the wall filters. Warning! It may cancel the state of the old ones,
  // with probable audible artifacts.
  void set_wall_filters(const std::vector<mcl::IirFilter>& wall_filters) noexcept {
    wall_filters_ = wall_filters;
  }
  
  void set_wall_filter(const Int wall_id, const mcl::IirFilter& filter) noexcept {
    ASSERT(wall_id>=0 && wall_id<num_boundary_points());
    wall_filters_[wall_id] = filter;
  }
  
  void set_wall_filters(const mcl::IirFilter& filter) noexcept {
    wall_filters_.assign(num_faces(), filter);
  }
  
  void SetFiltersNumeratorCoefficient(const Int coeff_id, const Sample value) {
    for (Int i=0; i < (Int) wall_filters_.size(); ++i) {
      wall_filters_[i].set_numerator_coefficient(coeff_id, value);
    }
  }
  
  // Returns a vector of points located at geometrical reflections,
  // relative to source and destinatin points.
  virtual std::vector<mcl::Point>
  CalculateBoundaryPoints(const mcl::Point& source,
                          const mcl::Point& destination) const noexcept = 0;
  
  virtual std::vector<mcl::IirFilter>
  GetBoundaryFilters(const mcl::Point& source_point,
                     const mcl::Point& mic_point) const noexcept = 0;
  
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
  virtual bool
  IsPointInRoom(const mcl::Point& point,
                const sal::Length wall_distance = 0.0) const noexcept = 0;
  
  virtual std::string ShapeDescription() const noexcept = 0;
  
  virtual ~Room() noexcept {}
protected:
  std::vector<mcl::IirFilter> wall_filters_;
  BoundarySetType boundary_set_type_;
};
  
} // namespace sal

#endif
