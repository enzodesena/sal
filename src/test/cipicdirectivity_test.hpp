/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "salutilities.hpp"
#include "cipicdir.hpp"
#include "receiver.hpp"
#include "vectorop.hpp"
#include "salconstants.hpp"


namespace sal
{
inline bool CipicMicTest()
{
  const std::string cipic_path = std::string("hrtfs/cipic/subject21");

  const Int impulse_response_length = 34;

  mcl::Vector<Sample> impulse(impulse_response_length);
  Sample sample = 0.5;
  impulse[0] = sample;

  Sample normalising_value = sample * 1.0 / NORMALISING_VALUE_CIPIC;

  // Testing frontal direction
  Receiver<Sample> mic_i
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_i(impulse_response_length);

  mic_i.ReceiveAndAddToBuffer(impulse, Point(1.0, 0.0, 0.0), stream_i);

  Signal<Sample> cmp_imp_front_left = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, -0.000030517578125, -0.000030517578125, -0.000091552734375,
    -0.000183105468750, -0.000335693359375, -0.000518798828125,
    -0.000549316406250, -0.000762939453125, -0.000213623046875,
    -0.000793457031250, -0.000885009765625, -0.000671386718750,
    -0.000701904296875, -0.000885009765625, -0.000762939453125,
    -0.001708984375000, 0.000183105468750, 0.000732421875000
  };

  cmp_imp_front_left = mcl::Multiply<Sample>(cmp_imp_front_left, normalising_value);
  ASSERT(IsEqual(stream_i.GetChannelReference(Channel::kLeft), cmp_imp_front_left));

  Receiver<Sample> mic_o
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0.0, 1, PI / 2.0));
  StereoBuffer<Sample> output_buffer_b(impulse_response_length);
  mic_o.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), output_buffer_b);
  ASSERT(IsEqual(output_buffer_b.GetChannelReference(Channel::kLeft), cmp_imp_front_left));

  // Testing upward direction
  Signal<Sample> cmp_imp_up_left = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    -0.000030517578125, -0.000030517578125, -0.000122070312500,
    -0.000244140625000, -0.000274658203125, -0.000427246093750,
    -0.000579833984375, -0.000579833984375, -0.000488281250000,
    -0.000915527343750, -0.000732421875000, -0.000732421875000,
    -0.000823974609375, -0.000946044921875, -0.000823974609375,
    -0.001037597656250, -0.000946044921875, 0.000366210937500
  };
  cmp_imp_up_left = mcl::Multiply(cmp_imp_up_left, normalising_value);

  Receiver<Sample> mic_m
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 1, 0, -PI / 2.0));
  StereoBuffer<Sample> stream_m(impulse_response_length);

  mic_m.ReceiveAndAddToBuffer(impulse, Point(-1.0, 0.0, 0.0), stream_m);
  ASSERT(IsEqual(stream_m.GetChannelReference(Channel::kLeft), cmp_imp_up_left));

  // Case for a source to the right (90deg) of the kemar.
  Signal<Sample> cmp_imp_right_left = {
    0.0, 0.0, 0.0, 0.0,
    -0.000030517578125,
    -0.000061035156250,
    -0.000122070312500,
    -0.000213623046875,
    -0.000335693359375,
    -0.000518798828125,
    -0.000457763671875,
    -0.000579833984375,
    -0.000793457031250,
    -0.000640869140625,
    -0.000915527343750,
    -0.000915527343750,
    -0.000732421875000,
    -0.001007080078125,
    -0.001068115234375,
    -0.000976562500000,
    -0.000946044921875,
    -0.001037597656250,
    -0.000885009765625,
    -0.000793457031250,
    -0.000976562500000,
    -0.000854492187500,
    -0.001159667968750,
    -0.001098632812500,
    -0.000518798828125,
    -0.001007080078125,
    -0.000946044921875,
    -0.000762939453125,
    -0.000976562500000,
    -0.000793457031250
  };
  cmp_imp_right_left = mcl::Multiply(cmp_imp_right_left, normalising_value);

  Signal<Sample> cmp_imp_right_right = {
    0.0, 0.0, 0.0, 0.0,
    -0.000061035156250,
    0.000061035156250,
    -0.000061035156250,
    0.0,
    0.000061035156250,
    -0.000701904296875,
    -0.000183105468750,
    -0.000335693359375,
    -0.000122070312500,
    -0.000427246093750,
    0.0,
    -0.000762939453125,
    -0.000976562500000,
    0.000976562500000,
    -0.002807617187500,
    0.002136230468750,
    -0.006683349609375,
    0.014312744140625,
    -0.022705078125000,
    0.291259765625000,
    0.348205566406250,
    0.081146240234375,
    0.010162353515625,
    -0.477874755859375,
    -0.158020019531250,
    -0.050842285156250,
    -0.069702148437500,
    0.299865722656250,
    0.065338134765625,
    -0.128631591796875
  };
  cmp_imp_right_right = mcl::Multiply(cmp_imp_right_right, normalising_value);


  Receiver<Sample> mic_p
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0.0, 1, 0.0));
  StereoBuffer<Sample> stream_p(impulse_response_length);

  // The cipic database has no entry at 90deg. All sources at 80deg azimuth
  // are equally distant.
  // I put the source slightly forward (in x direction) so that I know
  // that the one with 0 elevation will be selected.
  mic_p.ReceiveAndAddToBuffer(impulse, Point(0.001, -1.0, 0.0), stream_p);
  ASSERT(mcl::IsApproximatelyEqual(stream_p.GetChannelReference(Channel::kLeft), cmp_imp_right_left));
  ASSERT(mcl::IsApproximatelyEqual(stream_p.GetChannelReference(Channel::kRight), cmp_imp_right_right));

  // Case for a source to the left (-90deg) of the kemar.
  Signal<Sample> cmp_imp_left_right = {
    0.0,
    0.0,
    0.0,
    0.0,
    -0.000030517578125,
    0.000091552734375,
    -0.000091552734375,
    -0.000183105468750,
    -0.000305175781250,
    -0.000701904296875,
    -0.000518798828125,
    -0.000732421875000,
    -0.000885009765625,
    -0.000640869140625,
    -0.000915527343750,
    -0.000762939453125,
    -0.000610351562500,
    -0.000946044921875,
    -0.000823974609375,
    -0.000549316406250,
    -0.000976562500000,
    -0.001037597656250,
    -0.000488281250000,
    -0.000457763671875,
    -0.001007080078125,
    -0.000885009765625,
    -0.001007080078125,
    -0.000915527343750,
    -0.000457763671875,
    -0.001037597656250,
    -0.000854492187500,
    -0.000518798828125,
    -0.000854492187500,
    -0.000762939453125
  };
  cmp_imp_left_right = mcl::Multiply(cmp_imp_left_right, normalising_value);

  Signal<Sample> cmp_imp_left_left = {
    0.0,
    0.0,
    0.0,
    0.0,
    -0.000030517578125,
    -0.000030517578125,
    -0.000152587890625,
    -0.000274658203125,
    -0.000396728515625,
    -0.000274658203125,
    -0.000305175781250,
    -0.001007080078125,
    -0.000518798828125,
    -0.000976562500000,
    -0.000732421875000,
    -0.001037597656250,
    -0.001373291015625,
    0.000305175781250,
    -0.002288818359375,
    -0.000274658203125,
    -0.003204345703125,
    0.006408691406250,
    0.010040283203125,
    0.323822021484375,
    0.345489501953125,
    -0.000823974609375,
    -0.072540283203125,
    -0.383178710937500,
    -0.109375000000000,
    -0.019348144531250,
    -0.165466308593750,
    0.306335449218750,
    0.132720947265625,
    -0.196655273437500
  };
  cmp_imp_left_left = mcl::Multiply(cmp_imp_left_left, normalising_value);


  Receiver<Sample> mic_r
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0.0, 1, 0.0));
  StereoBuffer<Sample> stream_r(impulse_response_length);

  mic_r.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), stream_r);
  ASSERT(mcl::IsApproximatelyEqual(stream_r.GetChannelReference(Channel::kLeft), cmp_imp_left_left));
  ASSERT(mcl::IsApproximatelyEqual(stream_r.GetChannelReference(Channel::kRight), cmp_imp_left_right));

  // Case for a source in the back
  Signal<Sample> cmp_imp_back_left = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    -0.000030517578125,
    -0.000030517578125,
    -0.000152587890625,
    -0.000183105468750,
    -0.000244140625000,
    -0.000396728515625,
    -0.000335693359375,
    -0.000610351562500,
    -0.000671386718750,
    -0.000732421875000,
    -0.000701904296875,
    -0.000610351562500,
    -0.000701904296875,
    -0.000823974609375,
    -0.000976562500000,
    -0.000732421875000,
    -0.001251220703125,
    -0.000854492187500,
    -0.001983642578125
  };
  cmp_imp_back_left = mcl::Multiply(cmp_imp_back_left, normalising_value);

  Signal<Sample> cmp_imp_back_right = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    -0.000030517578125,
    -0.000061035156250,
    -0.000030517578125,
    -0.000183105468750,
    -0.000091552734375,
    -0.000152587890625,
    -0.000335693359375,
    -0.000152587890625,
    -0.000457763671875,
    0.000030517578125,
    -0.000610351562500,
    -0.000061035156250,
    -0.000946044921875,
    0.000335693359375,
    -0.001434326171875,
    0.000671386718750,
    -0.002593994140625,
    0.003295898437500,
    -0.002899169921875
  };
  cmp_imp_back_right = mcl::Multiply(cmp_imp_back_right, normalising_value);

  Receiver<Sample> mic_t
  (
    CipicDirectivity<Sample>(cipic_path, CipicDirectivity<Sample>::DataType::kWav),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 1, 0, -PI / 2.0));
  StereoBuffer<Sample> stream_t(impulse_response_length);

  mic_t.ReceiveAndAddToBuffer(impulse, Point(0.0, 0.0, -1.0), stream_t);

  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_back_left, stream_t.GetChannelReference(Channel::kLeft)));
  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_back_right, stream_t.GetChannelReference(Channel::kRight)));

  // Testing reset
  stream_t.SetSamplesToZero();
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(1.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(stream_t.GetChannelReference(Channel::kLeft)[0] != 0.0);
  ASSERT(stream_t.GetChannelReference(Channel::kRight)[0] != 0.0);

  stream_t.SetSamplesToZero();
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(stream_t.GetChannelReference(Channel::kLeft)[0] != 0.0);
  ASSERT(stream_t.GetChannelReference(Channel::kRight)[0] != 0.0);

  stream_t.SetSamplesToZero();
  mic_t.Reset();
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(stream_t.GetChannelReference(Channel::kLeft)[0] == 0.0);
  ASSERT(stream_t.GetChannelReference(Channel::kRight)[0] == 0.0);

  // Testing bypass
  stream_t.SetSamplesToZero();
  mic_t.SetBypass(false);
  mic_t.SetBypass(true);
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(1.2), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(stream_t.GetChannelReference(Channel::kLeft)[0] == 1.2);
  ASSERT(stream_t.GetChannelReference(Channel::kRight)[0] == 1.2);

  stream_t.SetSamplesToZero();
  mic_t.SetBypass(false);
  mic_t.ReceiveAndAddToBuffer(impulse, Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Channel::kLeft), cmp_imp_back_left));
  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Channel::kRight), cmp_imp_back_right));

  return true;
}
} // namespace sal
