/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "kemarmic.hpp"
#include "receiver.hpp"
#include "salconstants.hpp"
#include "vectorop.hpp"


namespace sal
{
inline bool KemarMicTest()
{
  using mcl::IsEqual;

  const std::string kemar_path = std::string("");

  const Int impulse_response_length = 128;

  Signal<Sample> impulse(impulse_response_length);
  Sample sample = 0.5;
  impulse[0] = sample;

  Sample normalising_value = sample / NORMALISING_VALUE_KEMAR;

  // Testing frontal direction
  Receiver<Sample> mic_i
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_i(impulse_response_length);

  mic_i.ReceiveAndAddToBuffer(impulse, Point(1.0, 0.0, 0.0), stream_i);


  Signal<Sample> cmp_imp_front_left = {
    5, -6, 19, -24, 39, -56, 74, -101, 120, -137, 161, 30,
    5321, 6066, -6762, -253, 195, -5129, 7285, 5986, 2332, 11785, 11069, -6536,
    -5109, 4539,
    -8167, -13539, -2026, -1365, -7336, -1277, 3012, 80, 1084, 4212, 3286, 1359,
    2467, 1543,
    -905, -191, 1215, -587, -1697, -1097, -1235, -2135, -2711, -2422, -2046,
    -1331, -253,
    114, 194, 681, 1310, 1375, 929, 696, 553, 151, -244, -225, -474, -819, -699,
    -736, -921,
    -736, -542, -605, -447, -73, 76, 157, 352, 549, 419, 112, -34, -6, -104,
    -306, -273, -311,
    -395, -292, -165, -175, -147, -5, -60, -246, -136, 59, 57, 16, 36, -37,
    -145, -119, -153,
    -195, -179, -216, -240, -128, 25, 74, 12, -44, -33, -95, -202, -185, -158,
    -192, -162, -83,
    -51, -27, -21, -53, -58, -38, -56, -58
  };
  
  cmp_imp_front_left = mcl::Multiply(cmp_imp_front_left, normalising_value);
  
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      stream_i.GetChannelReference(Buffer<Sample>::kLeftChannel),
      stream_i.GetChannelReference(Buffer<Sample>::kRightChannel)));
  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_front_left, stream_i.GetChannelReference(Buffer<Sample>::kLeftChannel), SMALL));

  Receiver<Sample> mic_o
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 0, 1, PI / 2.0));
  StereoBuffer<Sample> stream_o(impulse_response_length);

  mic_o.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), stream_o);
  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_front_left, stream_o.GetChannelReference(Buffer<Sample>::kLeftChannel), SMALL));

  // Testing frontal direction for reference point at y-axis
  Receiver<Sample> mic_ia
  (
    KemarDirectivity<Sample>
    (
      44100.0,
      kemar_path,
      KemarDirectivity<Sample>::kFullBrirLength,
      0,
      HeadRefOrientation::y_facing),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_ia(impulse_response_length);
  mic_ia.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), stream_ia);
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      cmp_imp_front_left,
      stream_ia.GetChannelReference(Buffer<Sample>::kLeftChannel), SMALL));

  // Testing upward direction
  Signal<Sample> cmp_imp_up_left = {
    5, -16, 26, -41, 63, -81, 119, -73, 2020, 4378, 5862,
    -903, -11453, -1186, 9080, 4800, 2577, 4817, 4371, 2144, 575, -3634, -5593,
    -3944, -3191,
    -4101, -3272, -1131, -482, 514, 1269, 2172, 1956, 1147, 1256, 990, -222,
    -780, 134, 460,
    257, -481, -1587, -2274, -1906, -1100, -432, -2, 355, 547, 419, 651, 931,
    678, 145, -284,
    -374, -347, -98, -141, -816, -1128, -690, -358, -272, 105, 75, -125, -120,
    -96, -275, -592,
    -607, -361, -364, -647, -563, -77, 190, 92, 42, 0, -123, -222, -170, -55,
    59, 53, -195, -453,
    -387, -120, 23, -107, -440, -607, -424, -74, 136, 51, -75, -11, 93, 170,
    173, 79, -115, -263,
    -269, -222, -146, -100, -138, -198, -199, -118, 6, 70, 29, -59, -127, -124,
    -20, 69, 38, -69,
    -132, -141
  };
  cmp_imp_up_left = mcl::Multiply(cmp_imp_up_left, normalising_value);

  Receiver<Sample> mic_m
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 1, 0, -PI / 2.0));
  StereoBuffer<Sample> stream_m(impulse_response_length);

  mic_m.ReceiveAndAddToBuffer(impulse, Point(-1.0, 0.0, 0.0), stream_m);
  ASSERT(mcl::IsApproximatelyEqual(stream_m.GetChannelReference(Buffer<Sample>::kLeftChannel), cmp_imp_up_left, SMALL));

  // Case for a source to the right (90deg) of the kemar.
  Signal<Sample> cmp_imp_right_left = {
    0, -1, -1, -1, -1, -1, -1, 0, -1, 1, 0, -1, 1, 0, -1, 0,
    -3, -2, -2, 3, 1, -3, 5, 17, 8, 1, 14, -17, -17, 142, 652, 557, 356, 1077,
    804, 509, 932, 639,
    379, 581, 1414, 3573, 5326, 3672, 748, -99, -453, -1911, -2745, -2775,
    -3069, -2714,
    -1804, -1237, -863, -252, 404, 726, 1007, 1192, 1025, 678, 534, 361, -178,
    -598, -774,
    -892, -1080, -1175, -1030, -821, -496, -222, -30, 127, 197, 220, 167, 118,
    30, -50, -129,
    -220, -251, -323, -346, -341, -344, -352, -298, -175, -99, -45, -14, 15, 41,
    37, -16, -73,
    -89, -103, -130, -168, -193, -164, -147, -163, -151, -116, -74, -54, -51,
    -73, -98, -93,
    -104, -114, -126, -133, -138, -155, -170, -195, -172, -112, -56, -26
  };
  cmp_imp_right_left = mcl::Multiply(cmp_imp_right_left, normalising_value);


  Signal<Sample> cmp_imp_right_right = {
    -335, 531, -666, 11016, 14274, -20450, -14263,
    12368, 6401, -801, 19447, 19852, -12004, -5756, 12468, 1733, -9639, -5131,
    -8237, -8810,
    85, -2066, -8668, -5494, 3117, 1712, -260, 4570, 5301, 2338, 1627, 2189,
    -842, -1992,
    -1338, -1702, -3319, -3868, -3063, -2585, -812, 342, 377, -215, 265, 1103,
    1114, 1065,
    1393, 929, -46, -95, 322, -262, -1020, -1130, -1409, -1717, -1222, -280,
    -226, -397, -211,
    273, 410, 477, 842, 837, 272, -36, -70, -360, -378, 3, -131, -616, -560,
    -251, -219, -103,
    254, 128, -151, -100, 40, 131, 284, 246, -105, -429, -382, -195, -60, -30,
    -174, -450, -620,
    -324, 100, 208, -37, -213, -289, -228, -13, 73, -31, -230, -347, -326, -174,
    2, 46, -72,
    -203, -152, 84, 193, 130, 18, -104, -176, -72, 82, 65, -52, -127, -156
  };
  cmp_imp_right_right = mcl::Multiply(cmp_imp_right_right, normalising_value);

  Receiver<Sample> mic_p
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_p(impulse_response_length);

  mic_p.ReceiveAndAddToBuffer(impulse, Point(0.0, -1.0, 0.0), stream_p);

  ASSERT(mcl::IsApproximatelyEqual(stream_p.GetChannelReference(Buffer<Sample>::kLeftChannel), cmp_imp_right_left, SMALL));
  ASSERT(mcl::IsApproximatelyEqual(stream_p.GetChannelReference(Buffer<Sample>::kRightChannel), cmp_imp_right_right, SMALL));

  // Case for a source to the right (90deg) of the kemar with reference on the y axis
  Receiver<Sample> mic_pa
  (
    KemarDirectivity<Sample>
    (
      44100.0,
      kemar_path,
      KemarDirectivity<Sample>::kFullBrirLength,
      0,
      HeadRefOrientation::y_facing),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_pa(impulse_response_length);
  mic_pa.ReceiveAndAddToBuffer(impulse, Point(1.0, 0.0, 0.0), stream_pa);
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      cmp_imp_right_left,
      stream_pa.GetChannelReference(Buffer<Sample>::kLeftChannel), SMALL));
  ASSERT
  (
    mcl::IsApproximatelyEqual
    (
      cmp_imp_right_right,
      stream_pa.GetChannelReference(Buffer<Sample>::kRightChannel), SMALL));

  // Case for a source to the left (-90deg) of the kemar.
  Signal<Sample> cmp_imp_left_right(cmp_imp_right_left);

  Signal<Sample> cmp_imp_left_left(cmp_imp_right_right);

  Receiver<Sample> mic_r
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  StereoBuffer<Sample> stream_r(impulse_response_length);
  stream_r.SetSamplesToZero();
  mic_r.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), stream_r);

  ASSERT(mcl::IsApproximatelyEqual(stream_r.GetChannelReference(Buffer<Sample>::kLeftChannel), cmp_imp_left_left, SMALL));
  ASSERT(mcl::IsApproximatelyEqual(stream_r.GetChannelReference(Buffer<Sample>::kRightChannel), cmp_imp_left_right, SMALL));

  // Case for a source in the back
  Signal<Sample> cmp_imp_back = {
    -3, -4, 3, -4, 6, -9, 16, -25, 19, -31, 34, -46, 60, -8,
    2200, 5530, 192, -4501, -1159, 994, -791, 3572, 12125, 6811, -604, 2758,
    2477, -2946,
    -4064, -3530, -5220, -5273, -2800, -2070, -2631, -454, 1570, 847, 1284,
    2371, 1898, 744,
    291, 266, -401, -869, -519, -607, -1259, -1358, -1433, -1302, -662, -168,
    -84, -173, 223,
    718, 668, 709, 834, 593, 149, -154, -298, -432, -521, -623, -815, -876,
    -560, -307, -258,
    -149, 9, 40, 62, 235, 376, 223, -47, -141, -249, -333, -247, -136, -208,
    -341, -301, -266,
    -265, -124, 53, 48, -53, -68, -32, 61, 155, 167, 37, -124, -209, -254, -266,
    -276, -308,
    -389, -407, -289, -120, -33, -35, -51, -66, -41, 3, 26, -14, -87, -134,
    -143, -108, -50,
    -25, -76, -124, -110
  };
  cmp_imp_back = mcl::Multiply(cmp_imp_back, normalising_value);

  Receiver<Sample> mic_t
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat<Length>(0, 1, 0, -PI / 2.0));
  StereoBuffer<Sample> stream_t(impulse_response_length);

  mic_t.ReceiveAndAddToBuffer(impulse, Point(0.0, 0.0, -1.0), stream_t);

  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kLeftChannel), cmp_imp_back, SMALL));
  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kRightChannel), cmp_imp_back, SMALL));

  // Testing reset
  stream_t.SetSamplesToZero();

  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(1.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(! mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kLeftChannel)[0], 0.0, 1.0E-10));
  ASSERT(! mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kRightChannel)[0], 0.0, 1.0E-10));

  stream_t.SetSamplesToZero();
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(! mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kLeftChannel)[0], 0.0, 1.0E-10));
  ASSERT(! mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kRightChannel)[0], 0.0, 1.0E-10));

  stream_t.SetSamplesToZero();
  mic_t.Reset();
  mic_t.ReceiveAndAddToBuffer(mcl::UnaryVector<Sample>(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kLeftChannel)[0], 0.0));
  ASSERT(mcl::IsApproximatelyEqual(stream_t.GetChannelReference(Buffer<Sample>::kRightChannel)[0], 0.0));

  // Testing multiple wave_ids with signals
  Receiver<Sample> mic_u
  (
    KemarDirectivity<Sample>(44100.0, kemar_path),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity(),
    3);
  StereoBuffer<Sample> stream_u(impulse_response_length);

  mic_u.ReceiveAndAddToBuffer(impulse, Point(1.0, 0.0, 0.0), 0, stream_u);

  Signal<Sample> output_u_left = stream_u.GetChannelReference(Buffer<Sample>::kLeftChannel);
  Signal<Sample> output_u_right = stream_u.GetChannelReference(Buffer<Sample>::kRightChannel);

  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_front_left, output_u_left, SMALL));
  ASSERT(mcl::IsApproximatelyEqual(cmp_imp_front_left, output_u_right, SMALL));

  mic_u.ReceiveAndAddToBuffer(impulse, Point(0.0, -1.0, 0.0), 1, stream_u);

  output_u_left = stream_u.GetChannelReference(Buffer<Sample>::kLeftChannel);
  output_u_right = stream_u.GetChannelReference(Buffer<Sample>::kRightChannel);

  Signal<Sample> cmp_u_left = mcl::Add(cmp_imp_front_left, cmp_imp_right_left);
  Signal<Sample> cmp_u_right = mcl::Add(cmp_imp_front_left, cmp_imp_right_right);

  ASSERT(mcl::IsApproximatelyEqual(cmp_u_left, output_u_left, SMALL));
  ASSERT(mcl::IsApproximatelyEqual(cmp_u_right, output_u_right, SMALL));

  mic_u.Reset();
  stream_u.SetSamplesToZero();
  mic_u.ReceiveAndAddToBuffer(impulse, Point(1.0, 0.0, 0.0), 1, stream_u);
  mic_u.ReceiveAndAddToBuffer(impulse, Point(0.0, 1.0, 0.0), 2, stream_u);

  Signal<Sample> output_u_b_left = stream_u.GetChannelReference(Buffer<Sample>::kLeftChannel);
  Signal<Sample> output_u_b_right = stream_u.GetChannelReference(Buffer<Sample>::kRightChannel);

  Signal<Sample> cmp_u_left_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_left);
  Signal<Sample> cmp_u_right_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_right);

  ASSERT(mcl::IsApproximatelyEqual(cmp_u_left_b, output_u_b_left, 0.0001));
  ASSERT(mcl::IsApproximatelyEqual(cmp_u_right_b, output_u_b_right, 0.0001));

  return true;
}
} // namespace sal
