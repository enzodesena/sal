/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef SAL_ROOM_H
#define SAL_ROOM_H

#include "saltypes.h"
#include "point.h"
#include "iirfilter.h"
#include "comparisonop.h"

#ifdef SAL_EXPORTS
  #define SAL_API __declspec(dllexport)
#else
  #define SAL_API
#endif

namespace sal {

  
enum BoundarySetType {
  first_order_only,
  first_and_second_cross_horiz
};
  
  
class SAL_API Room {
public:
  // This is the standard constructor where you feed the room `shape` and for
  // each face of the shape you also provide a filter. The order in the 
  // `filters` vector follows that of the employed shape.
  Room(const std::vector<mcl::IirFilter>& wall_filters,
       const BoundarySetType boundary_set_type = first_order_only) noexcept :
          wall_filters_(wall_filters), boundary_set_type_(boundary_set_type) {}
  
  std::vector<mcl::IirFilter> wall_filters() noexcept { return wall_filters_; }
  
  // Resets the wall filters. Warning! It may cancel the state of the old ones,
  // with probable audible artifacts.
  void set_wall_filters(const std::vector<mcl::IirFilter>& wall_filters) noexcept {
    wall_filters_ = wall_filters;
  }
  
  void set_wall_filters(const mcl::IirFilter& filter) noexcept {
    wall_filters_.assign(num_faces(), filter);
  }
  
  // Returns a vector of points located at geometrical reflections,
  // relative to source and destinatin points.
  virtual std::vector<mcl::Point>
  CalculateBoundaryPoints(const mcl::Point& source,
                          const mcl::Point& destination) noexcept = 0;
  
  virtual std::vector<mcl::IirFilter>
  GetBoundaryFilters(const mcl::Point& source_point,
                     const mcl::Point& mic_point) noexcept = 0;
  
  virtual sal::UInt num_boundary_points() noexcept = 0;
  
  // Returns the shape's number of faces.
  virtual sal::UInt num_faces() const noexcept = 0;
  
  // Returns the maximum distance between two points inside the shape.
  virtual sal::Length max_distance() const noexcept = 0;
  
  virtual void UpdateShape(const Time time_since_last_update) noexcept = 0;
  
  virtual bool
  IsPointInRoom(const mcl::Point& point,
                const sal::Length precision = VERY_SMALL) const noexcept = 0;
  
  virtual std::string ShapeDescription() const noexcept = 0;
  
  virtual ~Room() noexcept {}
protected:
  std::vector<mcl::IirFilter> wall_filters_;
  BoundarySetType boundary_set_type_;
};
  
} // namespace sal

#endif
