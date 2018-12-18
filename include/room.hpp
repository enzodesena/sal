/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "saltypes.hpp"
#include "point.hpp"
#include "iirfilter.hpp"
#include "comparisonop.hpp"

namespace sal
{
enum BoundarySetType
{
  kFirstOrder,
  kFirstAndSecondOrder
};

template<typename T>
class RoomInterface
{
public:
  // This is the standard constructor where you feed the room `shape` and for
  // each face of the shape you also provide a filter. The order in the 
  // `filters` vector follows that of the employed shape.
  RoomInterface(
    const mcl::Vector<mcl::DigitalFilter<T>>& wall_filters) noexcept
    : wall_filters_(wall_filters)
    , boundary_set_type_(kFirstOrder)
  {
  }


  const mcl::Vector<mcl::DigitalFilter<T>>& wall_filters() const noexcept
  {
    return wall_filters_;
  }


  // Resets the wall filters. Warning! It may cancel the state of the old ones,
  // with probable audible artifacts.
  void SetWallFilters(
    const mcl::Vector<mcl::DigitalFilter<T>>& wall_filters) noexcept
  {
    wall_filters_ = wall_filters;
  }


  void SetWallFilter(
    const size_t wall_id,
    const mcl::DigitalFilter<T>& filter) noexcept
  {
    wall_filters_[wall_id] = filter;
  }


  void SetWallFilters(
    const mcl::DigitalFilter<T>& filter) noexcept
  {
    wall_filters_.assign(num_faces(), filter);
  }


  // Returns a vector of points located at geometrical reflections,
  // relative to source and destinatin points.
  virtual mcl::Vector<Point>
  GetBoundaryPoints(
    const Point& source,
    const Point& destination) const noexcept = 0;

  virtual mcl::Vector<mcl::DigitalFilter<T>>
  GetBoundaryFilters(
    const Point& source_point,
    const Point& mic_point) const noexcept = 0;

  // Returns the maximum distance between two points inside the shape.
  virtual Length max_distance() const noexcept = 0;

  /** Returns true if the specified point is inside the room.
   @param[in] wall_distance This parameter allows to set also a safe distance
   form the walls. For instance, if wall_distance is set to 1 meter,
   and the point is less than 1 meter away from a wall,
   this method will return false. */
  virtual bool
  IsPointInRoom(
    const Point& point,
    Length wall_distance = Length(0.0)) const noexcept = 0;

  virtual std::string ShapeDescription() const noexcept = 0;

protected:
  mcl::Vector<mcl::DigitalFilter<T>> wall_filters_;
  BoundarySetType boundary_set_type_;
};




template<typename T>
class Room
{
public:
  template<typename RoomType>
  Room(
    RoomType x)
    : self_(std::make_unique<RoomModel<RoomType>>(std::move(x)))
  {
  }


  Room(
    const Room& x)
    : self_(x.self_->copy_())
  {
  }


  Room(
    Room&& x) noexcept = default;


  Room& operator=(
    const Room& x) noexcept
  {
    Room tmp(x);
    *this = std::move(tmp); // Using move assignment operator
    return *this;
  }


  /** Move assignment operator */
  Room& operator=(
    Room&& x) noexcept = default;


  void ReceiveAndAddToBuffer(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output) noexcept
  {
    self_->ReceiveAndAddToBuffer_(input, point, output);
  }


  void Reset() noexcept
  {
    self_->Reset_();
  }
  
private:
  
  struct RoomConcept
  {
    virtual ~RoomConcept() = default;
    virtual void ReceiveAndAddToBuffer_(
      const mcl::Vector<T>& input,
      const Point& point,
      Buffer<T>& output) = 0;
    virtual void Reset_() = 0;
    virtual std::unique_ptr<RoomConcept> copy_() = 0;
  };


  template<typename RoomType>
  struct RoomModel final : RoomConcept
  {
    RoomType data_;

    RoomModel(
      RoomType x)
      : data_(std::move(x))
    {
    }


    std::unique_ptr<RoomConcept> copy_() override
    {
      return std::make_unique<RoomModel>(*this);
    }


    void ReceiveAndAddToBuffer_(
      const mcl::Vector<T>& input,
      const Point& point,
      Buffer<T>& output) noexcept override
    {
      data_.ReceiveAndAddToBuffer(input, point, output);
    }
  };
  
  std::unique_ptr<RoomConcept> self_; // Concept is filterable object
};


} // namespace sal
