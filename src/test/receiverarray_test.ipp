/*
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "directivity.hpp"
#include "receiverarray.hpp"
#include "quaternion.hpp"

namespace sal
{

inline bool ReceiverArrayTest()
{
  const size_t num_microphones(5);
  const Length array_radius(0.5);

  ReceiverArray<Sample> array_a = ReceiverArray<Sample>::MakeCircularArray
  (
    TrigDirectivity<Sample>(mcl::UnaryVector<Sample>(1.0)),
    Point(0.0, 0.0, 1.5),
    Quaternion::Identity(),
    array_radius,
    UniformAngles(num_microphones, 0));

  ASSERT(mcl::IsEqual(array_a.GetArrayCentre(), Point(0.0, 0.0, 1.5)));


  ASSERT(mcl::IsEqual(array_a[0].position(), Point(array_radius, 0.0, 1.5)));
  ASSERT(mcl::IsEqual(array_a[1].position(), Point(
        array_radius * cos(2.0 * PI / 5.0),
        array_radius * sin(2.0 * PI / 5.0),
        1.5)));

  ASSERT(
    IsApproximatelyEqual(array_a[0].orientation(), mcl::AxAng2Quat(0.0, 0.0, 1.0, 0.0)));
  ASSERT(
    mcl::IsApproximatelyEqual(
      array_a[1].orientation(),
      mcl::AxAng2Quat(0.0, 0.0, 1.0, 2.0 * PI / 5.0)));

  array_a.SetArrayCentre(Point(1.0, 0.0, 1.5));
  ASSERT(mcl::IsApproximatelyEqual(
    array_a[0].position(), Point(1.0 + array_radius, 0.0, 1.5)));
  ASSERT
  (mcl::IsApproximatelyEqual(
    array_a[1].position(), Point(
        1.0 + array_radius * cos(2.0 * PI / 5.0),
        0.0 + array_radius * sin(2.0 * PI / 5.0),
        1.5)));

  ReceiverArray<Sample> array_b = ReceiverArray<Sample>::MakeCircularArray
  (
    TrigDirectivity<Sample>(mcl::UnaryVector<Sample>(1.0)),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0),
    array_radius,
    UniformAngles(num_microphones, 0));
  
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_b[0].orientation(),
      mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0)));
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_b[1].orientation(),
      mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_b[0].position(), Point(0.0, array_radius, 0.0)));
  ASSERT
  (
    mcl::IsApproximatelyEqual(array_b[1].position(),
      Point
      (
        array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
        array_radius * sin(2.0 * PI / 5.0 + PI / 2.0),
        0.0)));
  
  ReceiverArray<Sample> array_c = ReceiverArray<Sample>::MakeCircularArray
  (
    TrigDirectivity<Sample>(mcl::UnaryVector<Sample>(1.0)),
    Point(0.0, 1.0, -1.0),
    mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0),
    array_radius,
    UniformAngles(num_microphones, 0));
  
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_c[0].orientation(),
      mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0)));
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_c[1].orientation(),
      mcl::AxAng2Quat(0.0, 0.0, 1.0, PI / 2.0 + 2.0 * PI / 5.0)));

  ASSERT(
    mcl::IsApproximatelyEqual
    (
      array_c[0].position(),
      Point(0.0, 1.0 + array_radius, -1.0)));
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      array_c[1].position(),
      Point
      (
        array_radius * cos(2.0 * PI / 5.0 + PI / 2.0),
        1.0 + array_radius * sin(2.0 * PI / 5.0 + PI / 2.0),
        -1.0)));
  
  ReceiverArray<Sample> stereo_array = ReceiverArray<Sample>::MakeStereoArray
  (
    TrigDirectivity<Sample>(mcl::UnaryVector<Sample>(1.0)),
    Point(0.2, 0.0, 1.5),
    Quaternion::Identity(),
    1.0,
    PI / 4.0,
    +PI / 8.0);
  
  ASSERT(stereo_array.size() == 2);
  ASSERT(
    mcl::IsApproximatelyEqual(
      stereo_array[0].position(),
      Point(0.2 + 1.0 * cos(0), 1.0 * sin(0), 1.5)));
  ASSERT(
    mcl::IsApproximatelyEqual(
      stereo_array[1].position(), Point(
        0.2 + 1.0 * cos(PI / 4.0), 1.0 * sin(PI / 4.0), 1.5)));

  
  ReceiverArray<Sample> array_d = ReceiverArray<Sample>::MakeCircularArray
  (
    TrigDirectivity<Sample>(mcl::BinaryVector<Sample>(0.5,0.5)),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity(),
    array_radius,
    mcl::UnaryVector<Sample>(0.0));
  
  ASSERT(IsEqual(array_d[0].position(), Point(array_radius, 0.0, 0.0)));
  
  Buffer<Sample> output(1,1);
  array_d[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(1.0,0.0,0.0), output);
  ASSERT(mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));
  
  output.ResetSamples();
  array_d[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(-1.0,0.0,0.0), output);
  ASSERT(mcl::IsApproximatelyEqual(output.GetSample(0,0), 0.0));
  
  output.ResetSamples();
  array_d[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(0.0,1.0,0.0), output);
  ASSERT(! mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));
  
  output.ResetSamples();
  array_d.SetArrayOrientation(mcl::AxAng2Quat(0.0, 0.0, 1.0, PI/2.0));
  ASSERT(mcl::IsApproximatelyEqual(array_d[0].position(), Point(0.0, array_radius, 0.0)));
  array_d[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(0.0,1.0,0.0), output);
  ASSERT(mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));
  
  
  
  
  ReceiverArray<Sample> array_e = ReceiverArray<Sample>::MakeCircularArray
  (
    TrigDirectivity<Sample>(mcl::BinaryVector<Sample>(0.5,0.5)),
    Point(1.0, 0.0, 0.0),
    mcl::AxAng2Quat(0.0, 0.0, 1.0, PI/2.0),
    array_radius,
    mcl::UnaryVector<Sample>(0.0));
  
  ASSERT(IsEqual(array_e[0].position(), Point(1.0, array_radius, 0.0)));
  
  output.ResetSamples();
  array_e[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(1.0,1.0,0.0), output);
  ASSERT(mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));

  output.ResetSamples();
  array_e[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(0.0,1.0,0.0), output);
  ASSERT(! mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));

  output.ResetSamples();
  array_e.SetArrayOrientation(mcl::AxAng2Quat(0.0, 0.0, 1.0, PI));
  array_e.SetArrayOrientation(mcl::AxAng2Quat(0.0, 0.0, 1.0, PI));
  ASSERT(mcl::IsApproximatelyEqual(array_e[0].position(), Point(1.0-array_radius, 0.0, 0.0)));
  array_e[0].ReceiveAdd(mcl::UnaryVector(1.0), Point(-1.0,0.0,0.0), output);
  ASSERT(mcl::IsApproximatelyEqual(output.GetSample(0,0), 1.0));
  
  return true;
}
} // namespace sal
