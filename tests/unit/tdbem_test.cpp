/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "monomics.h"
#include "tdbem.h"

namespace sal {

using sal::dsp::IirFilter;
using sal::dsp::Point;
using sal::Int;
using sal::Length;
using sal::Microphone;
using sal::OmniMic;
using sal::Sample;
using sal::Source;
using sal::Time;

bool TdBem::SimulationTime() {
  Time sampling_frequency = 4000;

  const Int num_samples = 10;

  MonoBuffer input(num_samples);
  input.SetSample(0, 1.0);

  OmniMic mic(Point(1.23, 2.556, 0.456));

  Source source(Point(1.23, 2.556, 0.856));

  CuboidRoom room(5.23, 4.86, 2.1, dsp::GainFilter(1));

  clock_t launch = clock();
  TdBem tdbem(&room, &source, &mic, sampling_frequency, 0.2, 1000000.5);
  clock_t done = clock();

  std::cout << "TDBEM Load: " << (done - launch) / ((sal::Time)CLOCKS_PER_SEC)
            << " s\n";

  MonoBuffer output_buffer(num_samples);
  launch = clock();
  tdbem.Run(input, output_buffer);
  done = clock();

  std::cout << "TDBEM Run: " << (done - launch) / ((sal::Time)CLOCKS_PER_SEC)
            << " s\n";

  return true;
}

}  // namespace sal
