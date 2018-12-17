/*
 microphone_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "receiver.hpp"
#include "directivity.hpp"
#include "audiobuffer.hpp"


namespace sal
{
inline bool ReceiverTest()
{
  using mcl::IsEqual;

  //////////////////////////////////
  // OmniMic tests                //
  //////////////////////////////////

  Receiver<Sample> mic_a(OmniDirectivity<Sample>(), Point(0.0, 0.0, 1.0));

  ASSERT(IsEqual(mic_a.position(), Point(0.0, 0.0, 1.0)));

  Buffer<Sample> buffer(1,1);
  mic_a.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(-1.0), Point(1.0, 0.0, 2.0), buffer);
  ASSERT(buffer.GetSample(0,0) == -1.0);
  
  buffer.SetSamplesToZero();
  mic_a.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(2.0), Point(1.0, 0.0, 2.0), buffer);
  ASSERT(buffer.GetSample(0,0) == 2.0);

  //////////////////////////////////
  // TrigMic tests                //
  //////////////////////////////////
  mcl::Vector<Sample> coefficients(3);
  coefficients[0] = 0.5;
  coefficients[1] = -1.0;
  coefficients[2] = -0.5;

  Receiver<Sample> mic_i(
    TrigDirectivity<Sample>(coefficients),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0, 1, PI / 4.0));

  buffer.SetSamplesToZero();
  mic_i.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(0.0, 1.0, 0.0), buffer);
  Sample mic_i_cmp = 0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0));
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), mic_i_cmp));

  Receiver<Sample> mic_e(
    TrigDirectivity<Sample>(coefficients),
    Point(0.0, 0.0, 1.0),
    mcl::AxAng2Quat<Length>(0, 0, 1, PI / 2.0));
  
  buffer.SetSamplesToZero();
  mic_e.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(0.0, 1.0, 0.0), buffer);
  Sample mic_e_cmp = 0.5 * (0.5 - 1.0 * cos(PI / 4.0) - 0.5 * pow(cos(PI / 4.0), 2.0));
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), mic_e_cmp));


  Receiver<Sample> mic_f(
    TrigDirectivity<Sample>(coefficients),
    Point(2.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0, 1, PI / 2.0));
  
  buffer.SetSamplesToZero();
  mic_f.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(0.0, 1.0, 0.0), buffer);
  const Angle angle_f = PI / 2.0 - acos(2.0 / sqrt(5.0));
  const Sample mic_f_cmp = 0.5 * (0.5 - 1.0 * cos(angle_f) - 0.5 * pow(cos(angle_f), 2.0));
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), mic_f_cmp));

  Receiver<Sample> mic_g(
    TrigDirectivity<Sample>(coefficients),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  
  buffer.SetSamplesToZero();
  mic_g.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(1.0, 0.0, 0.0), buffer);
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), 0.5 * (0.5 - 1.0 - 0.5)));


  Receiver<Sample> mic_h(
    TrigDirectivity<Sample>(coefficients),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0, 1, PI / 5.0));
  
  buffer.SetSamplesToZero();
  mic_h.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(2.0, 2.0 * tan(PI / 5.0), 0.0), buffer);
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), 0.5 * (0.5 - 1.0 - 0.5)));
  

  Receiver<Sample> mic_p(
    TrigDirectivity<Sample>(coefficients),
    Point(2.0, 1.2, 0.5),
    mcl::AxAng2Quat<Length>(0, 0, 1, 0));
  
  buffer.SetSamplesToZero();
  mic_p.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(2.0 + cos(0), 1.2 + sin(0), 0.5 + 0.4), buffer);
  const Sample sample = buffer.GetSample(0,0);
  mic_p.SetOrientation(mcl::AxAng2Quat<Length>(0, 0, 1, PI / 10.0));
  
  buffer.SetSamplesToZero();
  mic_p.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.5), Point(2.0 + cos(PI / 10.0), 1.2 + sin(PI / 10.0), 0.5 + 0.4), buffer);
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), sample));
  
  //////////////////////////////////
  // GainMic tests                //
  //////////////////////////////////

  Receiver<Sample> mic_v(OmniDirectivity<Sample>(0.5), Point(0.0, 0.0, 0.0));
  
  buffer.SetSamplesToZero();
  mic_v.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(-1.0), Point(1.0, 0.0, 2.0), buffer);
  ASSERT(mcl::IsApproximatelyEqual(buffer.GetSample(0,0), -1.0*0.5));
  return true;
}
} // namespace sal
