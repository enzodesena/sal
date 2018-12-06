/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "ism.hpp"
#include "salconstants.hpp"
#include "microphone.hpp"
#include "monomics.hpp"
#include "source.hpp"

using mcl::IsEqual;
using sal::Time;
using sal::Length;
using sal::Sample;
using mcl::IirFilter;
using sal::Int;
using sal::Source;
using sal::Microphone;
using mcl::Point;
using sal::Sample;
using sal::OmniMic;
using sal::Length;
using mcl::GainFilter;

namespace sal
{
bool Ism::Test()
{
  //  %% Test with rigid walls
  //  
  //  test_rir = sroom([1 1 500], [1 3 500], [5 5 1000], ones(2,3), 9);
  Time sampling_frequency = 44100;

  OmniMic mic
  (
    Point
    (
      1.0 * SOUND_SPEED / sampling_frequency,
      1.0 * SOUND_SPEED / sampling_frequency,
      500.0 * SOUND_SPEED / sampling_frequency));

  Source source
  (
    Point
    (
      1.0 * SOUND_SPEED / sampling_frequency,
      3.0 * SOUND_SPEED / sampling_frequency,
      500.0 * SOUND_SPEED / sampling_frequency));

  MonoBuffer impulse(9);
  impulse.SetSample(0, 1.0);

  CuboidRoom room
  (
    5.0 * SOUND_SPEED / sampling_frequency,
    5.0 * SOUND_SPEED / sampling_frequency,
    1000.0 * SOUND_SPEED / sampling_frequency,
    GainFilter(1.0));

  Ism ism(&room, &source, &mic, none, 9, sampling_frequency);
  MonoBuffer test_rir(impulse.num_samples());
  ism.Run(impulse.GetReadPointer(), impulse.num_samples(), test_rir);

  mcl::Vector<Sample> cmp = mcl::Zeros<Sample>(9);

  cmp[2] = 1.0 / 2.0;

  Length cmp_1l = sqrt(pow(2.0, 2.0) + pow(2.0, 2.0));
  Int cmp_1l_tap = mcl::RoundToInt(cmp_1l);
  cmp[cmp_1l_tap] += 1.0 / cmp_1l;

  cmp[4] += 1.0 / 4.0; // I-order bottom
  cmp[6] += 1.0 / 6.0; // I-order top

  Length cmp_1r = sqrt(pow(2.0, 2.0) + pow(8.0, 2.0));
  Int cmp_1r_tap = mcl::RoundToInt(cmp_1r); // 8
  cmp[cmp_1r_tap] += 1.0 / cmp_1r;

  Length cmp_2lb = sqrt(pow(2.0, 2.0) + pow(4.0, 2.0));
  Int cmp_2lb_tap = mcl::RoundToInt(cmp_2lb); // 4
  cmp[cmp_2lb_tap] += 1.0 / cmp_2lb;

  Length cmp_2lt = sqrt(pow(2.0, 2.0) + pow(6.0, 2.0));
  Int cmp_2lt_tap = mcl::RoundToInt(cmp_2lt); // 6
  cmp[cmp_2lt_tap] += 1.0 / cmp_2lt;

  cmp[8] += 1.0 / 8.0; // II-order bottom

  Length cmp_3lb = sqrt(pow(8.0, 2.0) + pow(2.0, 2.0));
  Int cmp_3lb_tap = mcl::RoundToInt(cmp_3lb); // 4
  cmp[cmp_3lb_tap] += 1.0 / cmp_3lb;

  //  0
  //  0.5000
  //  0.3536
  //  0.4736
  //  0
  //  0.3248
  //  0
  //  0.3675
  //  0.1118
  ASSERT(mcl::IsEqual(cmp, test_rir.GetReadPointer()));

  ////////////////////////////////////
  // Test with wall absorption      //
  ////////////////////////////////////

  Sample beta_x1 = 1.0 / sqrt(2.0);
  Sample beta_x2 = 1.0 / sqrt(3.0);
  Sample beta_y1 = 1.0 / sqrt(5.0);
  Sample beta_y2 = 1.0 / sqrt(6.0);

  mcl::Vector<mcl::IirFilter> iir_filters;
  iir_filters.push_back(GainFilter(beta_x1));
  iir_filters.push_back(GainFilter(beta_x2));
  iir_filters.push_back(GainFilter(beta_y1));
  iir_filters.push_back(GainFilter(beta_y2));
  iir_filters.push_back(GainFilter(0.0));
  iir_filters.push_back(GainFilter(0.0));

  CuboidRoom room_absorption
  (
    5.0 * SOUND_SPEED / sampling_frequency,
    5.0 * SOUND_SPEED / sampling_frequency,
    1000.0 * SOUND_SPEED / sampling_frequency,
    iir_filters);

  Ism isma(&room_absorption, &source, &mic, none, 9, sampling_frequency);
  test_rir.Reset();
  isma.Run(impulse.GetReadPointer(), impulse.num_samples(), test_rir);

  mcl::Vector<Sample> cmpa = mcl::Zeros<Sample>(9);

  cmpa[2] = 1.0 / 2.0; // LOS

  Length cmpa_1l = sqrt(pow(2.0, 2.0) + pow(2.0, 2.0));
  Int cmpa_1l_tap = mcl::RoundToInt(cmpa_1l);
  cmpa[cmpa_1l_tap] += 1.0 / cmpa_1l * beta_x1;

  cmpa[4] += 1.0 / 4.0 * beta_y1; // I-order bottom
  cmpa[6] += 1.0 / 6.0 * beta_y2; // I-order top

  Length cmpa_1r = sqrt(pow(2.0, 2.0) + pow(8.0, 2.0));
  Int cmpa_1r_tap = mcl::RoundToInt(cmpa_1r); // 8
  cmpa[cmpa_1r_tap] += 1.0 / cmpa_1r * beta_x2;

  Length cmpa_2lb = sqrt(pow(2.0, 2.0) + pow(4.0, 2.0));
  Int cmpa_2lb_tap = mcl::RoundToInt(cmpa_2lb); // 4
  cmpa[cmpa_2lb_tap] += 1.0 / cmpa_2lb * beta_x1 * beta_y1;

  Length cmpa_2lt = sqrt(pow(2.0, 2.0) + pow(6.0, 2.0));
  Int cmpa_2lt_tap = mcl::RoundToInt(cmpa_2lt); // 6
  cmpa[cmpa_2lt_tap] += 1.0 / cmpa_2lt * beta_x1 * beta_y2;

  cmpa[8] += 1.0 / 8.0 * beta_y1 * beta_y2; // II-order bottom

  Length cmpa_3lb = sqrt(pow(8.0, 2.0) + pow(2.0, 2.0));
  Int cmpa_3lb_tap = mcl::RoundToInt(cmpa_3lb); // 4
  cmpa[cmpa_3lb_tap] += 1.0 / cmpa_3lb * beta_y1 * beta_x1 * beta_y2;

  //  0
  //  0.5000
  //  0.2500
  //  0.1825
  //  0
  //  0.1137
  //  0
  //  0.1085
  //  0.0289

  ASSERT(mcl::IsEqual(cmpa, test_rir.GetReadPointer()));

  // Testing peterson
  // TODO: complete this test.
  //  Ism ism_b(&room_absorption, &source, &mic, peterson, 10, sampling_frequency);
  //  mic.Reset();
  //  source.stream()->Reset();
  //  mcl::Vector<sal::Sample> signal = {0.3, -0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  //  source.stream()->Push(signal);
  //  ism_b.Run();
  //  mcl::Print(mic.stream()->PullAll());

  return true;
}
} // namespace sal
