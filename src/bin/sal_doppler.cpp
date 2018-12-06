/*
 sdnc_sound_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "pawrapper.hpp"
#include "matrixop.hpp"
#include "propagationline.hpp"
#include <unistd.h>

using namespace sal;

Signal GenerateSine(
  Time length,
  Time sine_frequency,
  Time sampling_frequency,
  Sample amplitude,
  Time trailing_length);

int main(
  int argc,
  char* const argv[])
{
  const Time sampling_frequency = 44100;
  const Int samples_per_buffer = 1024;
  // Please notice that with samples_per_buffer = 1000, I observed a quirky effect
  // whereby the samples align in such a way that the latency are very close
  // to integers values. However, very small changes (e.g. the difference between
  // repeated summation and multiplication in the the RampSmoother object)
  // can make the latency fluctuate when taking the floor(value). This
  // results in some very faint clicks. It is next to impossible to observe something
  // like this, unless in a very carefully designed example like this.

  const Time sine_frequency = 1000.0;
  const Sample sine_amplitude = 0.1;

  const Time sine_length = 2.0;

  const Length speed = 171.5; //[m/s];

  Signal sine = GenerateSine(sine_length, sine_frequency, sampling_frequency, sine_amplitude, 0);
  mcl::Matrix<sal::Sample> original_sine(sine.size(), 1);
  mcl::Matrix<sal::Sample> doppler_sine(sine.size(), 1);
  mcl::Matrix<sal::Time> current_latency(sine.size(), 1);

  Length current_distance = SOUND_SPEED / sampling_frequency;
  PropagationLine line_doppler
  (current_distance,
   sampling_frequency,
   1000.0,
   InterpolationType::kRounding);

  PropagationLine line
  (current_distance,
   sampling_frequency,
   1000.0,
   InterpolationType::kRounding);
  line.SetAttenuation(1.0, 0.0);

  Sample line_tmp[samples_per_buffer];
  Sample line_doppler_tmp[samples_per_buffer];

  Int index = 0;
  while ((index + samples_per_buffer) < ceil(sine_length * sampling_frequency))
  {
    Time update_period = (Time)samples_per_buffer / sampling_frequency; // In seconds
    Length space_covered = update_period * speed;
    current_distance += space_covered;
    line_doppler.SetDistance(current_distance, update_period);

    line_doppler.SetAttenuation(1.0, 0.0); // Sets gain to 1 instantly, to bypass the slow update of set_distance

    line_doppler.Write(&sine[index], samples_per_buffer);
    line_doppler.Read(samples_per_buffer, line_doppler_tmp);
    line_doppler.Tick(samples_per_buffer);

    line.Write(&sine[index], samples_per_buffer);
    line.Read(samples_per_buffer, line_tmp);
    line.Tick(samples_per_buffer);

    for (Int i = 0; i < samples_per_buffer; ++i)
    {
      doppler_sine.SetElement(index + i, 0, line_doppler_tmp[i]);
      original_sine.SetElement(index + i, 0, line_tmp[i]);
    }

    index += samples_per_buffer;
  }

  original_sine.Save("/Users/enzodesena/repos/sal/src/bin/sine_original.txt", 5);
  doppler_sine.Save("/Users/enzodesena/repos/sal/src/bin/sine_doppler.txt", 5);
  current_latency.Save("/Users/enzodesena/repos/sal/src/bin/current_latency.txt", 10);

  return 0;
}

Signal GenerateSine(
  const Time length,
  const Time sine_frequency,
  const Time sampling_frequency,
  const Sample amplitude,
  const Time trailing_length)
{
  Signal t = mcl::ColonOperator(0, 1.0 / sampling_frequency, length);
  Signal sine_wave = mcl::Sin(mcl::Multiply(t, 2.0 * PI * sine_frequency));
  sine_wave = mcl::Multiply(sine_wave, amplitude);
  sine_wave = mcl::Concatenate
  (sine_wave,
   mcl::Zeros<mcl::Real>(trailing_length * sampling_frequency));
  return sine_wave;
}
