///*
// Spatial Audio Library (SAL)
// Copyright (c) 2013-2018, Enzo De Sena
// All rights reserved.
// 
// Authors: Enzo De Sena, enzodesena@gmail.com
// */
//
//#pragma once
//
//#include "saltypes.hpp"
//#include "point.hpp"
//#include "digitalfilter.hpp"
//#include "comparisonop.hpp"
//
//namespace sal
//{
//enum BoundarySetType
//{
//  kFirstOrder,
//  kFirstAndSecondOrder
//};
//
//template<typename T>
//class RoomInterface
//{
//public:
//  // This is the standard constructor where you feed the room `shape` and for
//  // each face of the shape you also provide a filter. The order in the 
//  // `filters` vector follows that of the employed shape.
//  RoomInterface(
//    const mcl::Vector<mcl::DigitalFilter<T>>& wall_filters) noexcept
//    : wall_filters_(wall_filters)
//    , boundary_set_type_(kFirstOrder)
//  {
//  }
//
//
//  const mcl::Vector<mcl::DigitalFilter<T>>& GetWallFilters() const noexcept
//  {
//    return wall_filters_;
//  }
//
//
//  // Resets the wall filters. Warning! It may cancel the state of the old ones,
//  // with probable audible artifacts.
//  void SetWallFilters(
//    const mcl::Vector<mcl::DigitalFilter<T>>& wall_filters) noexcept
//  {
//    wall_filters_ = wall_filters;
//  }
//
//
//  void SetWallFilter(
//    const size_t wall_id,
//    const mcl::DigitalFilter<T>& filter) noexcept
//  {
//    wall_filters_[wall_id] = filter;
//  }
//
//
//  void SetWallFilters(
//    const mcl::DigitalFilter<T>& filter) noexcept
//  {
//    wall_filters_.assign(num_faces(), filter);
//  }
//
//
//  // Returns a vector of points located at geometrical reflections,
//  // relative to source and destinatin points.
//  virtual mcl::Vector<Point>
//  GetBoundaryPoints(
//    const Point& source,
//    const Point& destination) const noexcept = 0;
//
//  virtual mcl::Vector<mcl::DigitalFilter<T>>
//  GetBoundaryFilters(
//    const Point& source_point,
//    const Point& mic_point) const noexcept = 0;
//
//  // Returns the maximum distance between two points inside the shape.
//  virtual Length max_distance() const noexcept = 0;
//
//  /** Returns true if the specified point is inside the room.
//   @param[in] wall_distance This parameter allows to set also a safe distance
//   form the walls. For instance, if wall_distance is set to 1 meter,
//   and the point is less than 1 meter away from a wall,
//   this method will return false. */
//  virtual bool
//  IsPointInRoom(
//    const Point& point,
//    Length wall_distance = Length(0.0)) const noexcept = 0;
//
//  virtual std::string ShapeDescription() const noexcept = 0;
//
//protected:
//  mcl::Vector<mcl::DigitalFilter<T>> wall_filters_;
//  BoundarySetType boundary_set_type_;
//};
//
//
//} // namespace sal
