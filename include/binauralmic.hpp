/*
 binauralmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_BINAURALMIC_H
#define SAL_BINAURALMIC_H

#include <map>
#include "vector.hpp"
#include "receiver.hpp"
#include "saltypes.hpp"
#include "salconstants.hpp"
#include "firfilter.hpp"

namespace sal
{
enum class HeadRefOrientation
{
  x_facing,
  // Head facing positive x-axis; positive z-axis passing through jaw and then scalp
  y_facing
  // Head facing positive y-axis; positive z-axis passing through jaw and then scalp
};


enum class Ear {
  kLeft,
  kRight
};


template<typename T>
class BinauralDirectivity : public DirectivityInterface<T>
{
protected:
  HeadRefOrientation reference_orientation_;

public:
  BinauralDirectivity(
    HeadRefOrientation reference_orientation)
    : reference_orientation_(reference_orientation)
  {
  }
};

template<typename T>
class FirBinauralDirectivity : public BinauralDirectivity<T>
{
private:
  mcl::FirFilter<T> left_filter_;
  mcl::FirFilter<T> right_filter_;
  Point previous_point_;
  size_t update_length_;

  virtual mcl::Vector<T>& GetBrir(
    const Ear ear,
    const Point& point) noexcept = 0;
  
  
  void UpdateFilters(
    const Point& point) noexcept
  {
    // If it is the first time we update, set the impulse response immediately
    const size_t update_length = (isnan(previous_point_.x())) ? 0 : update_length_;
    
    if (! IsEqual(point, previous_point_))
    {
      // Update cache variables
      previous_point_ = point;

      left_filter_.SetImpulseResponse
      (
        GetBrir(Ear::kLeft, point),
        update_length);
      right_filter_.SetImpulseResponse
      (
        GetBrir(Ear::kRight, point),
        update_length);
    }
  }
  
public:
  FirBinauralDirectivity(
    HeadRefOrientation reference_orientation = HeadRefOrientation::x_facing,
    size_t update_length = 0)
    : BinauralDirectivity<T>(reference_orientation)
    , left_filter_(mcl::FirFilter<T>())
    , right_filter_(mcl::FirFilter<T>())
    , previous_point_(Point(NAN, NAN, NAN))
    , update_length_(update_length)
  {
  }


  void ReceiveAndAddToBuffer(
    const mcl::Vector<T>& input,
    const Point& point,
    Buffer<T>& output) noexcept override
  {
    mcl::Vector<T> ref_left = output.GetChannelReference(Buffer<T>::kLeftChannel);
    mcl::Vector<T> ref_right = output.GetChannelReference(Buffer<T>::kRightChannel);
    UpdateFilters(point);
    left_filter_.FilterAdd
    (
      input,
      output.GetChannelReference(Buffer<T>::kLeftChannel),
      ref_left);
    right_filter_.FilterAdd
    (
      input,
      output.GetChannelReference(Buffer<T>::kRightChannel),
      ref_right);
  }
  
  void Reset() noexcept override
  {
    left_filter_.Reset();
    right_filter_.Reset();
//    previous_point_ = Point(NAN, NAN, NAN);
  }
};


template<typename T>
class DatabaseBinauralDirectivity : public FirBinauralDirectivity<T>
{
public:
  DatabaseBinauralDirectivity(
    HeadRefOrientation reference_orientation = HeadRefOrientation::x_facing,
    size_t update_length = 0)
    : FirBinauralDirectivity<T>(reference_orientation, update_length)
  {
  }

  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  static void FilterAll(
    mcl::DigitalFilter<T> filter,
    mcl::Vector<mcl::Vector<Signal<T>>>& hrir_right,
    mcl::Vector<mcl::Vector<Signal<T>>>& hrir_left) noexcept
  {
    for (size_t i = 0; i < hrir_left.size(); ++i)
    {
      for (size_t j = 0; j < hrir_left[i].size(); ++j)
      {
        filter.Reset();
        filter.Filter(hrir_left[i][j], hrir_left[i][j]);
        filter.Reset();
        filter.Filter(hrir_right[i][j], hrir_right[i][j]);
      }
    }
  }
};


} // namespace sal

#endif
