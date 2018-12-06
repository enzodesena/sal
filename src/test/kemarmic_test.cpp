/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "kemarmic.hpp"
#include "microphone.hpp"
#include "salconstants.hpp"
#include "vectorop.hpp"

using mcl::Point;
using mcl::Quaternion;

namespace sal
{
bool KemarMic::Test()
{
  using mcl::IsEqual;

  const std::string kemar_path = std::string("");

  const Int impulse_response_length = 128;

  MonoBuffer impulse(impulse_response_length);
  Sample sample = 0.5;
  impulse.SetSample(0, sample);

  Sample normalising_value = sample / NORMALISING_VALUE_KEMAR;

  // Testing frontal direction
  KemarMic mic_i
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path);
  StereoBuffer stream_i(impulse_response_length);

  mic_i.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), stream_i);

  const Sample imp_front_left[] = {
    5, -6, 19, -24, 39, -56, 74, -101, 120, -137, 161, 30,
    5321, 6066, -6762, -253, 195, -5129, 7285, 5986, 2332, 11785, 11069, -6536, -5109, 4539,
    -8167, -13539, -2026, -1365, -7336, -1277, 3012, 80, 1084, 4212, 3286, 1359, 2467, 1543,
    -905, -191, 1215, -587, -1697, -1097, -1235, -2135, -2711, -2422, -2046, -1331, -253,
    114, 194, 681, 1310, 1375, 929, 696, 553, 151, -244, -225, -474, -819, -699, -736, -921,
    -736, -542, -605, -447, -73, 76, 157, 352, 549, 419, 112, -34, -6, -104, -306, -273, -311,
    -395, -292, -165, -175, -147, -5, -60, -246, -136, 59, 57, 16, 36, -37, -145, -119, -153,
    -195, -179, -216, -240, -128, 25, 74, 12, -44, -33, -95, -202, -185, -158, -192, -162, -83,
    -51, -27, -21, -53, -58, -38, -56, -58
  };

  Signal cmp_imp_front_left
  (imp_front_left,
   imp_front_left + sizeof(imp_front_left) / sizeof(Sample));
  cmp_imp_front_left = mcl::Multiply(cmp_imp_front_left, normalising_value);

  ASSERT
  (IsEqual
    (stream_i.GetLeftReadPointer(),
     stream_i.GetRightReadPointer(),
     impulse_response_length));
  ASSERT(IsEqual(cmp_imp_front_left, stream_i.GetLeftReadPointer()));

  KemarMic mic_o
  (Point(0.0, 0.0, 0.0),
   mcl::AxAng2Quat(0, 0, 1, PI / 2.0),
   kemar_path);
  StereoBuffer stream_o(impulse_response_length);

  mic_o.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), stream_o);
  ASSERT(mcl::IsEqual(cmp_imp_front_left, stream_o.GetLeftReadPointer()));

  // Testing frontal direction for reference point at y-axis
  KemarMic mic_ia
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path,
   kFullBrirLength,
   0,
   HeadRefOrientation::y_z);
  StereoBuffer stream_ia(impulse_response_length);
  mic_ia.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), stream_ia);
  ASSERT
  (mcl::IsEqual
    (cmp_imp_front_left,
     stream_ia.GetLeftReadPointer()));

  // Testing upward direction
  const Sample imp_up_left[] = {
    5, -16, 26, -41, 63, -81, 119, -73, 2020, 4378, 5862,
    -903, -11453, -1186, 9080, 4800, 2577, 4817, 4371, 2144, 575, -3634, -5593, -3944, -3191,
    -4101, -3272, -1131, -482, 514, 1269, 2172, 1956, 1147, 1256, 990, -222, -780, 134, 460,
    257, -481, -1587, -2274, -1906, -1100, -432, -2, 355, 547, 419, 651, 931, 678, 145, -284,
    -374, -347, -98, -141, -816, -1128, -690, -358, -272, 105, 75, -125, -120, -96, -275, -592,
    -607, -361, -364, -647, -563, -77, 190, 92, 42, 0, -123, -222, -170, -55, 59, 53, -195, -453,
    -387, -120, 23, -107, -440, -607, -424, -74, 136, 51, -75, -11, 93, 170, 173, 79, -115, -263,
    -269, -222, -146, -100, -138, -198, -199, -118, 6, 70, 29, -59, -127, -124, -20, 69, 38, -69,
    -132, -141
  };

  Signal cmp_imp_up_left
  (imp_up_left,
   imp_up_left + sizeof(imp_up_left) / sizeof(Sample));
  cmp_imp_up_left = mcl::Multiply(cmp_imp_up_left, normalising_value);

  KemarMic mic_m(Point(0.0, 0.0, 0.0), mcl::AxAng2Quat(0, 1, 0, -PI / 2.0), kemar_path);
  StereoBuffer stream_m(impulse_response_length);

  mic_m.AddPlaneWave(impulse, Point(-1.0, 0.0, 0.0), stream_m);
  ASSERT(IsEqual(stream_m.GetLeftReadPointer(), cmp_imp_up_left));

  // Case for a source to the right (90deg) of the kemar.
  const Sample imp_right_left[] = {
    0, -1, -1, -1, -1, -1, -1, 0, -1, 1, 0, -1, 1, 0, -1, 0,
    -3, -2, -2, 3, 1, -3, 5, 17, 8, 1, 14, -17, -17, 142, 652, 557, 356, 1077, 804, 509, 932, 639,
    379, 581, 1414, 3573, 5326, 3672, 748, -99, -453, -1911, -2745, -2775, -3069, -2714,
    -1804, -1237, -863, -252, 404, 726, 1007, 1192, 1025, 678, 534, 361, -178, -598, -774,
    -892, -1080, -1175, -1030, -821, -496, -222, -30, 127, 197, 220, 167, 118, 30, -50, -129,
    -220, -251, -323, -346, -341, -344, -352, -298, -175, -99, -45, -14, 15, 41, 37, -16, -73,
    -89, -103, -130, -168, -193, -164, -147, -163, -151, -116, -74, -54, -51, -73, -98, -93,
    -104, -114, -126, -133, -138, -155, -170, -195, -172, -112, -56, -26
  };

  Signal cmp_imp_right_left
  (imp_right_left,
   imp_right_left + sizeof(imp_right_left) / sizeof(Sample));
  cmp_imp_right_left = mcl::Multiply(cmp_imp_right_left, normalising_value);

  const Sample imp_right_right[] = {
    -335, 531, -666, 11016, 14274, -20450, -14263,
    12368, 6401, -801, 19447, 19852, -12004, -5756, 12468, 1733, -9639, -5131, -8237, -8810,
    85, -2066, -8668, -5494, 3117, 1712, -260, 4570, 5301, 2338, 1627, 2189, -842, -1992,
    -1338, -1702, -3319, -3868, -3063, -2585, -812, 342, 377, -215, 265, 1103, 1114, 1065,
    1393, 929, -46, -95, 322, -262, -1020, -1130, -1409, -1717, -1222, -280, -226, -397, -211,
    273, 410, 477, 842, 837, 272, -36, -70, -360, -378, 3, -131, -616, -560, -251, -219, -103,
    254, 128, -151, -100, 40, 131, 284, 246, -105, -429, -382, -195, -60, -30, -174, -450, -620,
    -324, 100, 208, -37, -213, -289, -228, -13, 73, -31, -230, -347, -326, -174, 2, 46, -72,
    -203, -152, 84, 193, 130, 18, -104, -176, -72, 82, 65, -52, -127, -156
  };

  Signal cmp_imp_right_right
  (imp_right_right,
   imp_right_right + sizeof(imp_right_right) / sizeof(Sample));
  cmp_imp_right_right = mcl::Multiply(cmp_imp_right_right, normalising_value);

  KemarMic mic_p
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path);
  StereoBuffer stream_p(impulse_response_length);

  mic_p.AddPlaneWave(impulse, Point(0.0, -1.0, 0.0), stream_p);

  ASSERT(mcl::IsEqual(stream_p.GetLeftReadPointer(), cmp_imp_right_left));
  ASSERT(mcl::IsEqual(stream_p.GetRightReadPointer(), cmp_imp_right_right));

  // Case for a source to the right (90deg) of the kemar with reference on the y axis
  KemarMic mic_pa
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path,
   kFullBrirLength,
   0,
   HeadRefOrientation::y_z);
  StereoBuffer stream_pa(impulse_response_length);
  mic_pa.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), stream_pa);
  ASSERT
  (mcl::IsEqual
    (cmp_imp_right_left,
     stream_pa.GetLeftReadPointer()));
  ASSERT
  (mcl::IsEqual
    (cmp_imp_right_right,
     stream_pa.GetRightReadPointer()));

  // Case for a source to the left (-90deg) of the kemar.
  Signal cmp_imp_left_right
  (imp_right_left,
   imp_right_left + sizeof(imp_right_left) / sizeof(Sample));
  cmp_imp_left_right = mcl::Multiply(cmp_imp_left_right, normalising_value);

  Signal cmp_imp_left_left
  (imp_right_right,
   imp_right_right + sizeof(imp_right_right) / sizeof(Sample));
  cmp_imp_left_left = mcl::Multiply(cmp_imp_left_left, normalising_value);

  KemarMic mic_r
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path);
  StereoBuffer stream_r(impulse_response_length);

  mic_r.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), stream_r);

  ASSERT(mcl::IsEqual(stream_r.GetLeftReadPointer(), cmp_imp_left_left));
  ASSERT(mcl::IsEqual(stream_r.GetRightReadPointer(), cmp_imp_left_right));

  // Case for a source in the back
  const Sample imp_back[] = {
    -3, -4, 3, -4, 6, -9, 16, -25, 19, -31, 34, -46, 60, -8,
    2200, 5530, 192, -4501, -1159, 994, -791, 3572, 12125, 6811, -604, 2758, 2477, -2946,
    -4064, -3530, -5220, -5273, -2800, -2070, -2631, -454, 1570, 847, 1284, 2371, 1898, 744,
    291, 266, -401, -869, -519, -607, -1259, -1358, -1433, -1302, -662, -168, -84, -173, 223,
    718, 668, 709, 834, 593, 149, -154, -298, -432, -521, -623, -815, -876, -560, -307, -258,
    -149, 9, 40, 62, 235, 376, 223, -47, -141, -249, -333, -247, -136, -208, -341, -301, -266,
    -265, -124, 53, 48, -53, -68, -32, 61, 155, 167, 37, -124, -209, -254, -266, -276, -308,
    -389, -407, -289, -120, -33, -35, -51, -66, -41, 3, 26, -14, -87, -134, -143, -108, -50,
    -25, -76, -124, -110
  };

  Signal cmp_imp_back
  (imp_back,
   imp_back + sizeof(imp_back) / sizeof(Sample));
  cmp_imp_back = mcl::Multiply(cmp_imp_back, normalising_value);

  KemarMic mic_t
  (Point(0.0, 0.0, 0.0),
   mcl::AxAng2Quat(0, 1, 0, -PI / 2.0),
   kemar_path);
  StereoBuffer stream_t(impulse_response_length);

  mic_t.AddPlaneWave(impulse, Point(0.0, 0.0, -1.0), stream_t);

  ASSERT(mcl::IsEqual(stream_t.GetLeftReadPointer(), cmp_imp_back));
  ASSERT(mcl::IsEqual(stream_t.GetRightReadPointer(), cmp_imp_back));

  // Testing reset
  stream_t.Reset();

  mic_t.AddPlaneWave(MonoBuffer::Unary(1.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(! IsEqual(stream_t.GetLeftReadPointer()[0], 0.0, 1.0E-10));
  ASSERT(! IsEqual(stream_t.GetRightReadPointer()[0], 0.0, 1.0E-10));

  stream_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(! IsEqual(stream_t.GetLeftReadPointer()[0], 0.0, 1.0E-10));
  ASSERT(! IsEqual(stream_t.GetRightReadPointer()[0], 0.0, 1.0E-10));

  stream_t.Reset();
  mic_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0, 0.0, -1.0), stream_t);
  ASSERT(IsEqual(stream_t.GetLeftReadPointer()[0], 0.0));
  ASSERT(IsEqual(stream_t.GetRightReadPointer()[0], 0.0));

  // Testing multiple wave_ids with signals
  KemarMic mic_u
  (Point(0.0, 0.0, 0.0),
   mcl::Quaternion::Identity(),
   kemar_path);
  StereoBuffer stream_u(impulse_response_length);

  mic_u.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), 0, stream_u);

  Signal output_u_left
  (stream_u.GetLeftReadPointer(),
   stream_u.GetLeftReadPointer() + impulse_response_length);
  Signal output_u_right
  (stream_u.GetRightReadPointer(),
   stream_u.GetRightReadPointer() + impulse_response_length);

  ASSERT(IsEqual(cmp_imp_front_left, output_u_left));
  ASSERT(IsEqual(cmp_imp_front_left, output_u_right));

  mic_u.AddPlaneWave(impulse, Point(0.0, -1.0, 0.0), 1, stream_u);

  output_u_left = Signal
  (stream_u.GetLeftReadPointer(),
   stream_u.GetLeftReadPointer() + impulse_response_length);
  output_u_right = Signal
  (stream_u.GetRightReadPointer(),
   stream_u.GetRightReadPointer() + impulse_response_length);

  Signal cmp_u_left = mcl::Add(cmp_imp_front_left, cmp_imp_right_left);
  Signal cmp_u_right = mcl::Add(cmp_imp_front_left, cmp_imp_right_right);

  ASSERT(IsEqual(cmp_u_left, output_u_left));
  ASSERT(IsEqual(cmp_u_right, output_u_right));

  stream_u.Reset();
  mic_u.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), 1, stream_u);
  mic_u.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), 2, stream_u);

  Signal output_u_b_left
  (stream_u.GetLeftReadPointer(),
   stream_u.GetLeftReadPointer() + impulse_response_length);
  Signal output_u_b_right
  (stream_u.GetRightReadPointer(),
   stream_u.GetRightReadPointer() + impulse_response_length);

  Signal cmp_u_left_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_left);
  Signal cmp_u_right_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_right);

  ASSERT(IsEqual(cmp_u_left_b, output_u_b_left));
  ASSERT(IsEqual(cmp_u_right_b, output_u_b_right));

  //  KemarMic mic_l(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
  //                 "",
  //                 kFullBrirLength,
  //                 0,
  //                 HeadRefOrientation::standard,
  //                 24000);

  return true;
}
} // namespace sal
