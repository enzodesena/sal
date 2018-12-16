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
#include "microphone.hpp"
#include "saltypes.hpp"
#include "array.hpp"
#include "salconstants.hpp"
#include "firfilter.hpp"

namespace sal
{
enum class HeadRefOrientation
{
  standard,
  // Head facing positive x-axis; positive z-axis passing through jaw and then scalp
  y_z
  // Head facing positive y-axis; positive z-axis passing through jaw and then scalp
};


class BinauralMicInstance;




class DatabaseBinauralMic : public BinauralMic
{
public:
  DatabaseBinauralMic(
    const mcl::Point& position,
    mcl::Quaternion orientation,
    Int update_length,
    HeadRefOrientation reference_orientation = HeadRefOrientation::standard);

  /**
   Filters all responses by `filter`. Useful for instance for including
   an inverse headphone filter
   */
  void FilterAll(
    mcl::DigitalFilter* filter);


  virtual ~DatabaseBinauralMic()
  {
  }


protected:
  // Database
  std::vector<mcl::Vector<Signal>> hrtf_database_right_;
  std::vector<mcl::Vector<Signal>> hrtf_database_left_;
};


template<typename T>
class BinauralFilter : public SpatialFilterInterface<T>
{
private:
  BinauralFilter(
    size_t update_length,
    const HeadRefOrientation reference_orientation = HeadRefOrientation::
      standard)
    : filter_left_(mcl::GainFilter(1.0))
    , filter_right_(mcl::GainFilter(1.0))
    , update_length_(update_length)
    , reference_orientation_(reference_orientation)
  {
  }


  void AddPlaneWaveRelative(
    const Sample* input_data,
    Int num_samples,
    const mcl::Point& point,
    Buffer& output_buffer) noexcept;

  void UpdateFilter(
    const mcl::Point& point) noexcept;

  mcl::DigitalFilter filter_left_;
  mcl::DigitalFilter filter_right_;
  size_t update_length_;
  HeadRefOrientation reference_orientation_;
};
} // namespace sal

#endif
