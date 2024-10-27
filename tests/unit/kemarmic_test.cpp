/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include "kemarmic.h"
#include "microphone.h"
#include "salconstants.h"
#include "vectorop.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {

bool KemarMic::Test() {
  using mcl::IsEqual;

  const Int impulse_response_length = 128;

  MonoBuffer impulse(impulse_response_length);
  Sample sample = 0.5;
  impulse.SetSample(0, sample);

  Sample normalising_value = sample / NORMALISING_VALUE_KEMAR;

  // Testing frontal direction
  KemarMic mic_i(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                 kCompactDataset);
  StereoBuffer buffer_i(impulse_response_length);

  mic_i.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), buffer_i);

  const Sample imp_front_left[] = {
      5,     -6,    19,    -24,   39,    -56,   74,    -101,   120,   -137,
      161,   30,    5321,  6066,  -6762, -253,  195,   -5129,  7285,  5986,
      2332,  11785, 11069, -6536, -5109, 4539,  -8167, -13539, -2026, -1365,
      -7336, -1277, 3012,  80,    1084,  4212,  3286,  1359,   2467,  1543,
      -905,  -191,  1215,  -587,  -1697, -1097, -1235, -2135,  -2711, -2422,
      -2046, -1331, -253,  114,   194,   681,   1310,  1375,   929,   696,
      553,   151,   -244,  -225,  -474,  -819,  -699,  -736,   -921,  -736,
      -542,  -605,  -447,  -73,   76,    157,   352,   549,    419,   112,
      -34,   -6,    -104,  -306,  -273,  -311,  -395,  -292,   -165,  -175,
      -147,  -5,    -60,   -246,  -136,  59,    57,    16,     36,    -37,
      -145,  -119,  -153,  -195,  -179,  -216,  -240,  -128,   25,    74,
      12,    -44,   -33,   -95,   -202,  -185,  -158,  -192,   -162,  -83,
      -51,   -27,   -21,   -53,   -58,   -38,   -56,   -58};

  Signal cmp_imp_front_left(
      imp_front_left, imp_front_left + sizeof(imp_front_left) / sizeof(Sample));
  cmp_imp_front_left = mcl::Multiply(cmp_imp_front_left, normalising_value);

  ASSERT(IsEqual(buffer_i.GetLeftReadView(), buffer_i.GetRightReadView()));

  ASSERT(IsEqual(cmp_imp_front_left, buffer_i.GetLeftReadView()));

  KemarMic mic_o(Point(0.0, 0.0, 0.0), mcl::AxAng2Quat(0, 0, 1, PI / 2.0),
                 kCompactDataset);
  StereoBuffer buffer_o(impulse_response_length);

  mic_o.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), buffer_o);
  ASSERT(mcl::IsEqual(cmp_imp_front_left, buffer_o.GetLeftReadView()));

  // Testing frontal direction for reference point at y-axis
  KemarMic mic_ia(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                  kCompactDataset, kFullBrirLength, 0, HeadRefOrientation::y_z);
  StereoBuffer buffer_ia(impulse_response_length);
  mic_ia.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), buffer_ia);
  ASSERT(mcl::IsEqual(cmp_imp_front_left, buffer_ia.GetLeftReadView()));

  // Testing upward direction
  const Sample imp_up_left[] = {
      5,    -16,   26,     -41,   63,    -81,   119,   -73,   2020, 4378,
      5862, -903,  -11453, -1186, 9080,  4800,  2577,  4817,  4371, 2144,
      575,  -3634, -5593,  -3944, -3191, -4101, -3272, -1131, -482, 514,
      1269, 2172,  1956,   1147,  1256,  990,   -222,  -780,  134,  460,
      257,  -481,  -1587,  -2274, -1906, -1100, -432,  -2,    355,  547,
      419,  651,   931,    678,   145,   -284,  -374,  -347,  -98,  -141,
      -816, -1128, -690,   -358,  -272,  105,   75,    -125,  -120, -96,
      -275, -592,  -607,   -361,  -364,  -647,  -563,  -77,   190,  92,
      42,   0,     -123,   -222,  -170,  -55,   59,    53,    -195, -453,
      -387, -120,  23,     -107,  -440,  -607,  -424,  -74,   136,  51,
      -75,  -11,   93,     170,   173,   79,    -115,  -263,  -269, -222,
      -146, -100,  -138,   -198,  -199,  -118,  6,     70,    29,   -59,
      -127, -124,  -20,    69,    38,    -69,   -132,  -141};

  Signal cmp_imp_up_left(imp_up_left,
                         imp_up_left + sizeof(imp_up_left) / sizeof(Sample));
  cmp_imp_up_left = mcl::Multiply(cmp_imp_up_left, normalising_value);

  KemarMic mic_m(Point(0.0, 0.0, 0.0), mcl::AxAng2Quat(0, 1, 0, -PI / 2.0),
                 kCompactDataset);
  StereoBuffer buffer_m(impulse_response_length);

  mic_m.AddPlaneWave(impulse, Point(-1.0, 0.0, 0.0), buffer_m);
  ASSERT(IsEqual(buffer_m.GetLeftReadView(), cmp_imp_up_left));

  // Case for a source to the right (90deg) of the kemar.
  const Sample imp_right_left[] = {
      0,    -1,   -1,    -1,    -1,    -1,    -1,    0,     -1,    1,     0,
      -1,   1,    0,     -1,    0,     -3,    -2,    -2,    3,     1,     -3,
      5,    17,   8,     1,     14,    -17,   -17,   142,   652,   557,   356,
      1077, 804,  509,   932,   639,   379,   581,   1414,  3573,  5326,  3672,
      748,  -99,  -453,  -1911, -2745, -2775, -3069, -2714, -1804, -1237, -863,
      -252, 404,  726,   1007,  1192,  1025,  678,   534,   361,   -178,  -598,
      -774, -892, -1080, -1175, -1030, -821,  -496,  -222,  -30,   127,   197,
      220,  167,  118,   30,    -50,   -129,  -220,  -251,  -323,  -346,  -341,
      -344, -352, -298,  -175,  -99,   -45,   -14,   15,    41,    37,    -16,
      -73,  -89,  -103,  -130,  -168,  -193,  -164,  -147,  -163,  -151,  -116,
      -74,  -54,  -51,   -73,   -98,   -93,   -104,  -114,  -126,  -133,  -138,
      -155, -170, -195,  -172,  -112,  -56,   -26};

  Signal cmp_imp_right_left(
      imp_right_left, imp_right_left + sizeof(imp_right_left) / sizeof(Sample));
  cmp_imp_right_left = mcl::Multiply(cmp_imp_right_left, normalising_value);

  const Sample imp_right_right[] = {
      -335,  531,   -666,   11016, 14274, -20450, -14263, 12368, 6401,  -801,
      19447, 19852, -12004, -5756, 12468, 1733,   -9639,  -5131, -8237, -8810,
      85,    -2066, -8668,  -5494, 3117,  1712,   -260,   4570,  5301,  2338,
      1627,  2189,  -842,   -1992, -1338, -1702,  -3319,  -3868, -3063, -2585,
      -812,  342,   377,    -215,  265,   1103,   1114,   1065,  1393,  929,
      -46,   -95,   322,    -262,  -1020, -1130,  -1409,  -1717, -1222, -280,
      -226,  -397,  -211,   273,   410,   477,    842,    837,   272,   -36,
      -70,   -360,  -378,   3,     -131,  -616,   -560,   -251,  -219,  -103,
      254,   128,   -151,   -100,  40,    131,    284,    246,   -105,  -429,
      -382,  -195,  -60,    -30,   -174,  -450,   -620,   -324,  100,   208,
      -37,   -213,  -289,   -228,  -13,   73,     -31,    -230,  -347,  -326,
      -174,  2,     46,     -72,   -203,  -152,   84,     193,   130,   18,
      -104,  -176,  -72,    82,    65,    -52,    -127,   -156};

  Signal cmp_imp_right_right(
      imp_right_right,
      imp_right_right + sizeof(imp_right_right) / sizeof(Sample));
  cmp_imp_right_right = mcl::Multiply(cmp_imp_right_right, normalising_value);

  KemarMic mic_p(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                 kCompactDataset);
  StereoBuffer buffer_p(impulse_response_length);

  mic_p.AddPlaneWave(impulse, Point(0.0, -1.0, 0.0), buffer_p);

  ASSERT(mcl::IsEqual(buffer_p.GetLeftReadView(), cmp_imp_right_left));
  ASSERT(mcl::IsEqual(buffer_p.GetRightReadView(), cmp_imp_right_right));

  // Case for a source to the right (90deg) of the kemar with reference on the y
  // axis
  KemarMic mic_pa(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                  kCompactDataset, kFullBrirLength, 0, HeadRefOrientation::y_z);
  StereoBuffer buffer_pa(impulse_response_length);
  mic_pa.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), buffer_pa);
  ASSERT(mcl::IsEqual(cmp_imp_right_left, buffer_pa.GetLeftReadView()));
  ASSERT(mcl::IsEqual(cmp_imp_right_right, buffer_pa.GetRightReadView()));

  // Case for a source to the left (-90deg) of the kemar.
  Signal cmp_imp_left_right(
      imp_right_left, imp_right_left + sizeof(imp_right_left) / sizeof(Sample));
  cmp_imp_left_right = mcl::Multiply(cmp_imp_left_right, normalising_value);

  Signal cmp_imp_left_left(
      imp_right_right,
      imp_right_right + sizeof(imp_right_right) / sizeof(Sample));
  cmp_imp_left_left = mcl::Multiply(cmp_imp_left_left, normalising_value);

  KemarMic mic_r(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                 kCompactDataset);
  StereoBuffer buffer_r(impulse_response_length);

  mic_r.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), buffer_r);

  ASSERT(mcl::IsEqual(buffer_r.GetLeftReadView(), cmp_imp_left_left));
  ASSERT(mcl::IsEqual(buffer_r.GetRightReadView(), cmp_imp_left_right));

  // Case for a source in the back
  const Sample imp_back[] = {
      -3,    -4,    3,    -4,   6,     -9,    16,    -25,   19,    -31,   34,
      -46,   60,    -8,   2200, 5530,  192,   -4501, -1159, 994,   -791,  3572,
      12125, 6811,  -604, 2758, 2477,  -2946, -4064, -3530, -5220, -5273, -2800,
      -2070, -2631, -454, 1570, 847,   1284,  2371,  1898,  744,   291,   266,
      -401,  -869,  -519, -607, -1259, -1358, -1433, -1302, -662,  -168,  -84,
      -173,  223,   718,  668,  709,   834,   593,   149,   -154,  -298,  -432,
      -521,  -623,  -815, -876, -560,  -307,  -258,  -149,  9,     40,    62,
      235,   376,   223,  -47,  -141,  -249,  -333,  -247,  -136,  -208,  -341,
      -301,  -266,  -265, -124, 53,    48,    -53,   -68,   -32,   61,    155,
      167,   37,    -124, -209, -254,  -266,  -276,  -308,  -389,  -407,  -289,
      -120,  -33,   -35,  -51,  -66,   -41,   3,     26,    -14,   -87,   -134,
      -143,  -108,  -50,  -25,  -76,   -124,  -110};

  Signal cmp_imp_back(imp_back, imp_back + sizeof(imp_back) / sizeof(Sample));
  cmp_imp_back = mcl::Multiply(cmp_imp_back, normalising_value);

  KemarMic mic_t(Point(0.0, 0.0, 0.0), mcl::AxAng2Quat(0, 1, 0, -PI / 2.0),
                 kCompactDataset);
  StereoBuffer buffer_t(impulse_response_length);

  mic_t.AddPlaneWave(impulse, Point(0.0, 0.0, -1.0), buffer_t);

  ASSERT(mcl::IsEqual(buffer_t.GetLeftReadView(), cmp_imp_back));
  ASSERT(mcl::IsEqual(buffer_t.GetRightReadView(), cmp_imp_back));

  // Testing reset
  buffer_t.Reset();

  mic_t.AddPlaneWave(MonoBuffer::Unary(1.0), Point(0.0, 0.0, -1.0), buffer_t);
  ASSERT(!IsEqual(buffer_t.GetLeftReadView()[0], 0.0, 1.0E-10));
  ASSERT(!IsEqual(buffer_t.GetRightReadView()[0], 0.0, 1.0E-10));

  buffer_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0, 0.0, -1.0), buffer_t);
  ASSERT(!IsEqual(buffer_t.GetLeftReadView()[0], 0.0, 1.0E-10));
  ASSERT(!IsEqual(buffer_t.GetRightReadView()[0], 0.0, 1.0E-10));

  buffer_t.Reset();
  mic_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0, 0.0, -1.0), buffer_t);
  ASSERT(IsEqual(buffer_t.GetLeftReadView()[0], 0.0));
  ASSERT(IsEqual(buffer_t.GetRightReadView()[0], 0.0));

  // Testing multiple wave_ids with signals
  KemarMic mic_u(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                 kCompactDataset);
  StereoBuffer buffer_u(impulse_response_length);

  mic_u.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), 0, buffer_u);

  Signal output_u_left(buffer_u.GetLeftReadView().begin(),
                       buffer_u.GetLeftReadView().end());
  Signal output_u_right(buffer_u.GetRightReadView().begin(),
                        buffer_u.GetRightReadView().end());

  ASSERT(IsEqual(cmp_imp_front_left, output_u_left));
  ASSERT(IsEqual(cmp_imp_front_left, output_u_right));

  mic_u.AddPlaneWave(impulse, Point(0.0, -1.0, 0.0), 1, buffer_u);

  output_u_left = Signal(buffer_u.GetLeftReadView().begin(),
                         buffer_u.GetLeftReadView().end());
  output_u_right = Signal(buffer_u.GetRightReadView().begin(),
                          buffer_u.GetRightReadView().end());

  Signal cmp_u_left = mcl::Add(cmp_imp_front_left, cmp_imp_right_left);
  Signal cmp_u_right = mcl::Add(cmp_imp_front_left, cmp_imp_right_right);

  ASSERT(IsEqual(cmp_u_left, output_u_left));
  ASSERT(IsEqual(cmp_u_right, output_u_right));

  buffer_u.Reset();
  mic_u.AddPlaneWave(impulse, Point(1.0, 0.0, 0.0), 1, buffer_u);
  mic_u.AddPlaneWave(impulse, Point(0.0, 1.0, 0.0), 2, buffer_u);

  Signal output_u_b_left(buffer_u.GetLeftReadView().begin(),
                         buffer_u.GetLeftReadView().end());
  Signal output_u_b_right(buffer_u.GetRightReadView().begin(),
                          buffer_u.GetRightReadView().end());

  Signal cmp_u_left_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_left);
  Signal cmp_u_right_b = mcl::Add(cmp_imp_front_left, cmp_imp_left_right);

  ASSERT(IsEqual(cmp_u_left_b, output_u_b_left));
  ASSERT(IsEqual(cmp_u_right_b, output_u_b_right));

  // Testing diffuse dataset
  KemarMic mic_diffuse(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                       kDiffuseDataset);
  StereoBuffer buffer_diffuse(impulse_response_length);

  mic_diffuse.AddPlaneWave(impulse, Point(0.0, 0.0, 1.0), buffer_diffuse);

  Signal cmp_diffuse_up_left_ear = {
      -3,    6,     -23,    39,    -68,   102,   -143, 205,  -178, 2419, 3925,
      5019,  -2985, -11161, 4587,  8199,  2124,  4031, 4318, 3512, 2529, -65,
      -3018, -1360, -1597,  -2303, -1786, -1785, -804, -329, -84,  -694, 196,
      -77,   -528,  -613,   -1038, -1042, -824,  100,  141,  116,  -301, -756,
      -1180, -1056, -525,   -121,  -181,  -278,  -355, -499, -127, -37,  -246,
      -473,  -717,  -456,   -208,  -10,   -146,  -647, -796, -467, -268, -234,
      47,    -286,  -379,   -316,  -484,  -653,  -829, -794, -567, -600, -835,
      -584,  -139,  -85,    -278,  -272,  -235,  -288, -421, -340, -155, -53,
      -90,   -271,  -404,   -273,  -37,   48,    -125, -439, -492, -282, -46,
      50,    -21,   -39,    27,    39,    93,    91,   51,   -54,  -121, -73,
      -6,    69,    89,     53,    11,    2,     44,   109,  134,  92,   40,
      14,    11,    81,     131,   106,   46,    34};

  ASSERT(IsEqual(buffer_diffuse.GetLeftReadView(),
                 mcl::Multiply(cmp_diffuse_up_left_ear, normalising_value)));
  ASSERT(IsEqual(buffer_diffuse.GetRightReadView(),
                 mcl::Multiply(cmp_diffuse_up_left_ear, normalising_value)));

  Angle elevation = 40.0 / 180.0 * PI;
  Angle azimuth = 77.0 / 180.0 * PI;
  mic_diffuse.Reset();
  buffer_diffuse.Reset();
  // The minus for azimuth is because for our reference system, a positive
  // azimuth angle is to the left of the person, while for kemar is on the right
  mic_diffuse.AddPlaneWave(
      impulse,
      Point(cos(-azimuth) * cos(elevation), sin(-azimuth) * cos(elevation),
            sin(elevation)),
      buffer_diffuse);

  Signal cmp_diffuse_elevation_40_azimuth_77_left_ear = {
      1,    2,    2,    2,    2,    3,    1,     3,     1,    0,    1,    2,
      2,    1,    2,    -3,   6,    1,    10,    -9,    13,   -16,  24,   -9,
      26,   -37,  457,  1140, 1379, 84,   -4082, -2932, 915,  3198, 4760, 2980,
      1622, 1258, 872,  989,  1427, 1096, 619,   388,   -130, -98,  -179, -335,
      -346, -276, -276, -231, -428, -557, 7,     -239,  -427, -422, -441, -699,
      -787, -340, -342, -421, -467, -414, -438,  -299,  -154, -425, -569, -457,
      -544, -649, -534, -496, -541, -672, -702,  -587,  -468, -455, -434, -401,
      -441, -457, -480, -427, -413, -424, -460,  -535,  -524, -476, -438, -395,
      -321, -275, -241, -219, -191, -164, -170,  -200,  -249, -261, -209, -132,
      -103, -111, -114, -108, -85,  -65,  -34,   -36,   -62,  -74,  -63,  -25,
      -12,  -11,  -44,  -99,  -109, -72,  -11,   12};

  Signal cmp_diffuse_elevation_40_azimuth_77_right_ear = {
      23,    -69,    97,     -167,  245,   -355,  495,   -736,  4268,  9963,
      2867,  -21935, -16659, 20230, 11812, 12247, 9962,  -2069, 2451,  1655,
      4907,  -1679,  -3551,  -5170, -3461, -3178, -3914, -303,  -2968, -1606,
      -2291, 176,    -344,   426,   40,    -1111, -41,   -441,  -529,  -1673,
      -753,  -913,   -144,   -375,  -244,  -233,  -969,  -116,  388,   55,
      -815,  -659,   -373,   -931,  -696,  -1003, -1235, -1544, -1221, -747,
      -452,  -302,   -298,   -42,   -395,  -418,  -335,  -140,  -296,  -478,
      -716,  -518,   -202,   -77,   -180,  -149,  -58,   -118,  -182,  -189,
      -13,   -18,    -102,   -202,  -235,  -124,  37,    102,   62,    -99,
      -182,  -209,   -48,    -32,   -37,   -115,  -91,   -100,  -41,   73,
      108,   140,    -19,    -145,  -153,  -76,   -25,   79,    21,    -66,
      -147,  -78,    35,     94,    62,    -24,   -17,   53,    147,   231,
      205,   114,    86,     93,    110,   140,   170,   172};

  ASSERT(IsEqual(buffer_diffuse.GetLeftReadView(),
                 mcl::Multiply(cmp_diffuse_elevation_40_azimuth_77_left_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_diffuse.GetRightReadView(),
                 mcl::Multiply(cmp_diffuse_elevation_40_azimuth_77_right_ear,
                               normalising_value)));

  mic_diffuse.Reset();
  buffer_diffuse.Reset();
  // Check for the other side (hence `azimuth` is positive here)
  mic_diffuse.AddPlaneWave(impulse,
                           Point(cos(azimuth) * cos(elevation),
                                 sin(azimuth) * cos(elevation), sin(elevation)),
                           buffer_diffuse);
  ASSERT(IsEqual(buffer_diffuse.GetLeftReadView(),
                 mcl::Multiply(cmp_diffuse_elevation_40_azimuth_77_right_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_diffuse.GetRightReadView(),
                 mcl::Multiply(cmp_diffuse_elevation_40_azimuth_77_left_ear,
                               normalising_value)));

  // Testing compact dataset
  KemarMic mic_compact(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                       kCompactDataset);
  StereoBuffer buffer_compact(impulse_response_length);

  mic_compact.AddPlaneWave(impulse, Point(0.0, 0.0, 1.0), buffer_compact);

  Signal cmp_compact_up_left_ear = {
      5,    -16,   26,     -41,   63,    -81,   119,   -73,   2020, 4378,
      5862, -903,  -11453, -1186, 9080,  4800,  2577,  4817,  4371, 2144,
      575,  -3634, -5593,  -3944, -3191, -4101, -3272, -1131, -482, 514,
      1269, 2172,  1956,   1147,  1256,  990,   -222,  -780,  134,  460,
      257,  -481,  -1587,  -2274, -1906, -1100, -432,  -2,    355,  547,
      419,  651,   931,    678,   145,   -284,  -374,  -347,  -98,  -141,
      -816, -1128, -690,   -358,  -272,  105,   75,    -125,  -120, -96,
      -275, -592,  -607,   -361,  -364,  -647,  -563,  -77,   190,  92,
      42,   0,     -123,   -222,  -170,  -55,   59,    53,    -195, -453,
      -387, -120,  23,     -107,  -440,  -607,  -424,  -74,   136,  51,
      -75,  -11,   93,     170,   173,   79,    -115,  -263,  -269, -222,
      -146, -100,  -138,   -198,  -199,  -118,  6,     70,    29,   -59,
      -127, -124,  -20,    69,    38,    -69,   -132,  -141};

  ASSERT(IsEqual(buffer_compact.GetLeftReadView(),
                 mcl::Multiply(cmp_compact_up_left_ear, normalising_value)));
  ASSERT(IsEqual(buffer_compact.GetRightReadView(),
                 mcl::Multiply(cmp_compact_up_left_ear, normalising_value)));

  elevation = 40.0 / 180.0 * PI;
  azimuth = 77.0 / 180.0 * PI;
  mic_compact.Reset();
  buffer_compact.Reset();
  // The minus for azimuth is because for our reference system, a positive
  // azimuth angle is to the left of the person, while for kemar is on the right
  mic_compact.AddPlaneWave(
      impulse,
      Point(cos(-azimuth) * cos(elevation), sin(-azimuth) * cos(elevation),
            sin(elevation)),
      buffer_compact);

  Signal cmp_compact_elevation_40_azimuth_77_left_ear = {
      1,     1,     1,     1,     2,    1,    2,    0,     -1,    0,    1,
      2,     0,     1,     -3,    3,    3,    9,    -5,    7,     -9,   14,
      0,     18,    -25,   378,   1172, 1643, 548,  -3654, -4390, -408, 3323,
      5308,  4375,  2582,  1574,  1229, 974,  618,  73,    -445,  -791, -1147,
      -1241, -1374, -1380, -1125, -563, -57,  98,   88,    216,   785,  797,
      438,   254,   280,   -93,   -630, -575, -580, -697,  -600,  -379, -341,
      -177,  74,    -99,   -281,  -69,  11,   -193, -218,  -167,  -276, -443,
      -466,  -396,  -320,  -283,  -259, -226, -201, -177,  -227,  -240, -210,
      -202,  -252,  -332,  -339,  -311, -312, -295, -207,  -106,  -28,  13,
      16,    -6,    -33,   -60,   -114, -158, -155, -131,  -132,  -137, -109,
      -72,   -51,   -46,   -38,   -50,  -70,  -79,  -89,   -86,   -83,  -77,
      -87,   -127,  -157,  -156,  -125, -93,  -57};

  Signal cmp_compact_elevation_40_azimuth_77_right_ear = {
      -45,    54,     -103,  148,   -210,  282,   -439,  3392,  10351, 6478,
      -18606, -24451, 11017, 19796, 14070, 11854, 3061,  -384,  1677,  5820,
      -4030,  -10225, -8506, -7117, -7193, -5437, -2574, -3967, -1387, 1474,
      4091,   2852,   3386,  4949,  3037,  1785,  1476,  347,   -1523, -1252,
      -2156,  -3171,  -3151, -1957, -1144, -1196, -134,  1041,  1140,  452,
      722,    1273,   632,   286,   -297,  -1362, -1947, -1331, -941,  -951,
      -553,   -320,   -159,  -49,   218,   290,   207,   21,    -25,   -328,
      -408,   20,     240,   -95,   -274,  -150,  -193,  -236,  -54,   95,
      -90,    -233,   -208,  -135,  37,    246,   206,   -57,   -259,  -258,
      -217,   -31,    19,    -171,  -393,  -318,  -138,  -45,   -70,   -141,
      -115,   -125,   -94,   -21,   -22,   -117,  -127,  -142,  -157,  -181,
      -158,   -143,   -152,  -153,  -141,  -105,  -67,   -17,   32,    28,
      11,     37,     48,    5,     -37,   -56,   -64,   -46};

  ASSERT(IsEqual(buffer_compact.GetLeftReadView(),
                 mcl::Multiply(cmp_compact_elevation_40_azimuth_77_left_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_compact.GetRightReadView(),
                 mcl::Multiply(cmp_compact_elevation_40_azimuth_77_right_ear,
                               normalising_value)));

  mic_compact.Reset();
  buffer_compact.Reset();
  // Check for the other side (hence `azimuth` is positive here)
  mic_compact.AddPlaneWave(impulse,
                           Point(cos(azimuth) * cos(elevation),
                                 sin(azimuth) * cos(elevation), sin(elevation)),
                           buffer_compact);
  ASSERT(IsEqual(buffer_compact.GetLeftReadView(),
                 mcl::Multiply(cmp_compact_elevation_40_azimuth_77_right_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_compact.GetRightReadView(),
                 mcl::Multiply(cmp_compact_elevation_40_azimuth_77_left_ear,
                               normalising_value)));

  // Testing full dataset

  const Int long_impulse_response_length = 512;
  MonoBuffer long_impulse(long_impulse_response_length);
  long_impulse.SetSample(0, sample);
  KemarMic mic_full(Point(0.0, 0.0, 0.0), mcl::Quaternion::Identity(),
                    kFullDataset);
  StereoBuffer buffer_full(long_impulse_response_length);

  mic_full.AddPlaneWave(long_impulse, Point(0.0, 0.0, 1.0), buffer_full);

  Signal cmp_full_up_left_ear = {
      -1,    -1,    0,     0,     1,     1,     0,     -2,    -1,     2,
      4,     5,     3,     1,     -2,    -5,    -4,    -4,    0,      -1,
      -2,    -4,    -6,    3,     8,     3,     4,     -11,   16,     -25,
      39,    -48,   73,    -30,   1546,  3893,  5588,  373,   -10031, -5025,
      6528,  6989,  2867,  2455,  2630,  1273,  457,   -2740, -6547,  -7058,
      -5361, -4545, -3695, -1328, 539,   2002,  3401,  4573,  4285,   3232,
      2445,  1509,  78,    -1299, -1421, -1253, -1556, -2011, -2142,  -2278,
      -2110, -1193, -145,  351,   685,   1005,  1001,  1248,  1689,   1569,
      893,   184,   -204,  -373,  -357,  -522,  -1162, -1732, -1593,  -991,
      -466,  62,    318,   402,   557,   666,   491,   91,    -149,   -129,
      -179,  -474,  -592,  -286,  47,    42,    -69,   -106,  -156,   -235,
      -149,  64,    228,   250,   75,    -177,  -197,  55,    252,    113,
      -295,  -574,  -466,  -96,   200,   197,   53,    36,    147,    290,
      343,   257,   39,    -178,  -251,  -209,  -125,  -64,   -62,    -91,
      -84,   -15,   97,    199,   207,   114,   13,    -34,   5,      92,
      124,   51,    -35,   -61,   -35,   -3,    25,    24,    -9,     0,
      53,    102,   138,   149,   112,   69,    56,    50,    17,     -14,
      -4,    40,    99,    137,   161,   169,   109,   67,    25,     -60,
      -80,   -46,   -4,    57,    78,    32,    19,    57,    97,     125,
      106,   64,    46,    33,    17,    19,    24,    -14,   -36,    -22,
      -34,   -10,   68,    78,    22,    4,     27,    58,    100,    139,
      129,   85,    74,    89,    77,    62,    58,    38,    22,     25,
      19,    0,     0,     18,    39,    40,    45,    69,    83,     78,
      49,    21,    33,    64,    62,    35,    19,    14,    8,      15,
      50,    99,    110,   76,    4,     -47,   -48,   -43,   72,     183,
      165,   93,    74,    56,    -41,   -77,   -77,   -80,   -47,    22,
      61,    43,    15,    -39,   -108,  -148,  -93,   14,    98,     146,
      167,   210,   237,   309,   389,   251,   24,    -75,   -99,    -133,
      -114,  -95,   -165,  -244,  -229,  -155,  -147,  -208,  -188,   -150,
      -192,  -49,   235,   406,   509,   498,   218,   -52,   -126,   -157,
      -252,  -365,  -428,  -350,  -143,  23,    76,    123,   158,    169,
      258,   309,   240,   119,   21,    -41,   -67,   -73,   -126,   -194,
      -200,  -146,  -98,   -50,   8,     50,    77,    77,    113,    181,
      202,   130,   64,    48,    14,    8,     2,     -63,   -137,   -114,
      86,    142,   -246,  -455,  -324,  -207,  3,     217,   239,    172,
      191,   224,   136,   -109,  -375,  -392,  -99,   174,   159,    82,
      154,   256,   116,   -66,   -141,  -247,  -267,  -247,  -292,   -330,
      -199,  -2,    37,    -31,   21,    131,   196,   283,   363,    378,
      265,   40,    -186,  -328,  -343,  -413,  -464,  -286,  -50,    120,
      177,   234,   266,   190,   156,   123,   49,    29,    20,     -71,
      -195,  -145,  -67,   -130,  -152,  -71,   75,    108,   71,     87,
      86,    69,    43,    -32,   -134,  -132,  -76,   -105,  -82,    -80,
      -65,   -21,   56,    95,    37,    35,    -13,   74,    175,    97,
      -19,   -31,   -42,   -127,  -245,  -389,  -304,  -120,  -49,    0,
      149,   243,   235,   206,   73,    -2,    19,    44,    -23,    -101,
      -60,   58,    43,    -93,   -200,  -234,  -179,  -104,  -44,    17,
      112,   132,   76,    26,    49,    86,    36,    -71,   -119,   -66,
      -61,   -121,  -145,  -92,   8,     96,    47,    3,     125,    151,
      13,    -104,  -182,  -193,  -77,   78,    127,   157,   183,    49,
      -59,   -22,   16,    22,    7,     -19,   -8,    29,    -4,     -58,
      -68,   -9,    8,     -53,   -34,   -22,   -28,   15,    27,     -29,
      -98,   -57};

  ASSERT(IsEqual(buffer_full.GetLeftReadView(),
                 mcl::Multiply(cmp_full_up_left_ear, normalising_value)));
  ASSERT(IsEqual(buffer_full.GetRightReadView(),
                 mcl::Multiply(cmp_full_up_left_ear, normalising_value)));

  elevation = 40.0 / 180.0 * PI;
  azimuth = 77.0 / 180.0 * PI;
  mic_full.Reset();
  buffer_full.Reset();
  // The minus for azimuth is because for our reference system, a positive
  // azimuth angle is to the left of the person, while for kemar is on the right
  mic_full.AddPlaneWave(long_impulse,
                        Point(cos(-azimuth) * cos(elevation),
                              sin(-azimuth) * cos(elevation), sin(elevation)),
                        buffer_full);

  Signal cmp_full_elevation_40_azimuth_77_left_ear = {
      2,     1,     1,     0,     0,     -1,    0,     0,     -1,   -1,   -1,
      0,     1,     2,     1,     0,     0,     1,     2,     2,    1,    0,
      -1,    0,     1,     1,     1,     1,     1,     1,     2,    1,    1,
      0,     -1,    -1,    0,     1,     0,     0,     -3,    1,    3,    8,
      -2,    3,     -7,    8,     3,     14,    -17,   283,   1000, 1559, 751,
      -2929, -4675, -1578, 2906,  5541,  4799,  2520,  944,   756,  987,  506,
      -879,  -2292, -2860, -2724, -2180, -1786, -1600, -1225, -413, 504,  1118,
      1267,  1227,  1538,  1506,  956,   350,   -52,   -531,  -945, -816, -711,
      -926,  -1083, -909,  -649,  -288,  137,   160,   -38,   102,  367,  363,
      281,   215,   22,    -239,  -318,  -264,  -267,  -345,  -392, -355, -261,
      -138,  -67,   -30,   -10,   19,    43,    11,    -23,   -24,  -39,  -73,
      -57,   -17,   -6,    -12,   -12,   -25,   -57,   -84,   -114, -138, -115,
      -50,   -13,   -21,   -22,   9,     57,    91,    98,    60,   -5,   -45,
      -42,   -21,   -30,   -63,   -99,   -126,  -117,  -72,   -22,  -7,   -3,
      3,     13,    46,    85,    117,   114,   77,    36,    11,   14,   22,
      20,    -13,   -66,   -85,   -56,   -2,    34,    42,    43,   60,   88,
      100,   79,    48,    40,    47,    53,    43,    18,    -2,   -8,   -8,
      -14,   -11,   8,     42,    73,    84,    79,    70,    69,   74,   78,
      69,    52,    30,    22,    26,    36,    44,    47,    44,   27,   12,
      12,    28,    41,    41,    39,    54,    73,    74,    67,   66,   73,
      77,    71,    49,    16,    -9,    -10,   24,    74,    96,   73,   23,
      -31,   -77,   -96,   -67,   5,     95,    138,   100,   51,   57,   87,
      75,    15,    -27,   7,     78,    108,   90,    100,   153,  176,  141,
      87,    49,    32,    3,     -94,   -194,  -191,  -122,  -91,  -120, -137,
      -88,   12,    117,   188,   210,   205,   180,   140,   93,   31,   -38,
      -108,  -162,  -165,  -137,  -113,  -84,   -44,   5,     74,   139,  187,
      214,   192,   186,   218,   245,   161,   6,     -36,   -34,  -116, -119,
      41,    -8,    -124,  -39,   -49,   -161,  -89,   57,    6,    57,   213,
      183,   111,   170,   144,   -54,   -109,  -109,  -158,  -86,  -11,  -64,
      -47,   43,    41,    49,    102,   64,    -39,   -12,   16,   -99,  -89,
      -63,   -128,  -91,   -59,   -71,   -27,   -32,   -117,  -45,  91,   55,
      -17,   38,    197,   263,   175,   69,    -19,   -46,   -9,   -36,  -69,
      -70,   -117,  -70,   17,    -16,   -39,   -5,    97,    183,  73,   -109,
      -111,  -9,    74,    119,   31,    -79,   -130,  -153,  -110, -111, -149,
      -82,   18,    55,    96,    103,   52,    39,    33,    41,   67,   22,
      -20,   30,    32,    4,     1,     -24,   -63,   -69,   -50,  -64,  -71,
      -92,   -76,   -35,   -19,   8,     42,    62,    26,    15,   53,   36,
      16,    22,    -66,   -115,  -78,   -50,   -43,   -35,   -26,  -11,  -17,
      -21,   -40,   -118,  -91,   -13,   35,    50,    54,    2,    -39,  31,
      24,    -12,   -90,   -89,   -7,    -31,   -36,   -29,   -35,  -11,  -41,
      -64,   -45,   -41,   -1,    10,    15,    25,    -29,   -62,  -43,  -19,
      -34,   -22,   3,     -15,   8,     0,     -18,   15,    32,   10,   -21,
      -30,   -55,   -70,   -49,   -14,   11,    21,    29,    24,   1,    -21,
      -33,   -27,   -14,   -36,   -30,   -9,    -8,    28,    31,   25,   5,
      -5,    33,    8,     -30,   -49,   -58,   -53,   -56,   -52,  -32,  -33,
      -37,   -21,   -11,   29,    41,    9,     26,    43,    50,   12,   -52,
      -53,   -59,   -41,   9,     21,    21};

  Signal cmp_full_elevation_40_azimuth_77_right_ear = {
      0,     -3,    -3,    -3,    -1,    -1,    -3,     -3,     -1,     3,
      7,     7,     3,     -1,    -6,    -6,    -6,     2,      4,      3,
      -12,   -7,    8,     15,    -2,    9,     -35,    30,     -64,    92,
      -125,  172,   -273,  2523,  8863,  7600,  -13537, -25557, 1204,   20449,
      18864, 11515, 1783,  -3424, -838,  6396,  -971,   -13321, -16733, -12808,
      -8727, -5055, -1067, -1622, -11,   4569,  9037,   9005,   7934,   7092,
      4751,  2367,  464,   -1479, -4498, -5682, -5120,  -4055,  -3761,  -3215,
      -2241, -1643, -56,   2192,  2962,  1904,  1545,   2308,   2402,   2031,
      1057,  -614,  -2105, -2126, -1348, -1046, -1284,  -1522,  -1225,  -667,
      152,   853,   1115,  909,   675,   517,   589,    817,    796,    278,
      -302,  -539,  -591,  -678,  -777,  -733,  -693,   -661,   -504,   -197,
      215,   641,   868,   786,   484,   210,   81,     135,    109,    -99,
      -410,  -579,  -535,  -310,  -49,   50,    1,      -140,   -181,   -11,
      236,   291,   162,   -54,   -180,  -132,  68,     232,    218,    40,
      -160,  -164,  29,    201,   221,   98,    -72,    -107,   41,     193,
      197,   96,    0,     -30,   25,    138,   169,    108,    8,      -32,
      -1,    60,    112,   102,   31,    -40,   -32,    18,     59,     73,
      63,    30,    26,    76,    92,    41,    -6,     0,      45,     68,
      71,    59,    29,    31,    57,    77,    70,     53,     35,     29,
      48,    51,    34,    53,    100,   120,   101,    79,     57,     26,
      26,    64,    66,    33,    8,     -12,   -18,    13,     70,     98,
      67,    14,    1,     28,    76,    128,   136,    101,    65,     56,
      53,    67,    120,   145,   98,    38,    -7,     -119,   -230,   -189,
      -37,   137,   234,   262,   230,   85,    -60,    -104,   -136,   -241,
      -243,  -7,    266,   303,   175,   100,   146,    241,    198,    99,
      7,     -141,  -203,  -248,  -336,  -325,  -195,   -33,    39,     48,
      157,   308,   338,   232,   105,   52,    68,     89,     52,     27,
      -31,   -129,  -134,  -139,  -176,  -185,  -164,   -77,    9,      17,
      46,    177,   254,   256,   244,   177,   109,    49,     -35,    -142,
      -202,  -155,  -105,  -87,   -83,   -62,   11,     16,     32,     125,
      133,   164,   250,   230,   168,   120,   -4,     -235,   -410,   -403,
      -321,  -251,  -217,  -181,  -73,   194,   534,    599,    367,    257,
      281,   151,   -118,  -416,  -529,  -365,  -102,   -22,    -255,   -335,
      -21,   312,   345,   234,   178,   108,   -28,    -160,   -233,   -227,
      -157,  -154,  -76,   87,    191,   249,   176,    45,     -94,    -230,
      -349,  -316,  -144,  -45,   42,    153,   186,    221,    267,    213,
      83,    0,     -115,  -259,  -292,  -296,  -234,   -107,   52,     200,
      226,   226,   290,   279,   84,    -132,  -145,   -96,    -103,   -144,
      -227,  -192,  -32,   71,    57,    41,    50,     15,     -17,    18,
      90,    89,    26,    -29,   -39,   -17,   -50,    -100,   -112,   -103,
      -127,  -193,  -161,  -48,   39,    54,    22,     46,     85,     115,
      136,   45,    -55,   -49,   -27,   -32,   -59,    -106,   -151,   -110,
      -61,   -62,   -43,   3,     43,    53,    36,     36,     43,     -25,
      -54,   -8,    -4,    -37,   -57,   -52,   -57,    -83,    -96,    -53,
      35,    65,    5,     -48,   -48,   -44,   -30,    23,     87,     97,
      30,    20,    74,    41,    4,     16,    -15,    -112,   -186,   -125,
      -44,   -34,   -50,   -83,   -28,   53,    48,     44,     30,     41,
      73,    18,    -24,   -52,   -43,   41,    9,      -108,   -157,   -106,
      10,    53,    -15,   -51,   2,     76,    87,     50,     29,     14,
      11,    22,    29,    7,     -47,   -84,   -80,    -66,    -37,    -6,
      -31,   -67,   -77,   -32,   22,    23,    -13,    -67,    -45,    28,
      55,    34};

  ASSERT(IsEqual(buffer_full.GetLeftReadView(),
                 mcl::Multiply(cmp_full_elevation_40_azimuth_77_left_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_full.GetRightReadView(),
                 mcl::Multiply(cmp_full_elevation_40_azimuth_77_right_ear,
                               normalising_value)));

  mic_full.Reset();
  buffer_full.Reset();
  // Check for the other side (hence `azimuth` is positive here)
  mic_full.AddPlaneWave(long_impulse,
                        Point(cos(azimuth) * cos(elevation),
                              sin(azimuth) * cos(elevation), sin(elevation)),
                        buffer_full);
  ASSERT(IsEqual(buffer_full.GetLeftReadView(),
                 mcl::Multiply(cmp_full_elevation_40_azimuth_77_right_ear,
                               normalising_value)));
  ASSERT(IsEqual(buffer_full.GetRightReadView(),
                 mcl::Multiply(cmp_full_elevation_40_azimuth_77_left_ear,
                               normalising_value)));

  return true;
}

}  // namespace sal
