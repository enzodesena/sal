/*
 propagationline.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "propagationline.h"
#include "satconstants.h"
#include "delayfilter.h"

using sat::Length;
using sat::Time;
using sat::Sample;

namespace sat {

PropagationLine::PropagationLine(const Length distance, 
                                 const Time sampling_frequency, 
                                 const Length max_distance) :
        delay_filter_(DelayFilter(round(ComputeLatency(distance, sampling_frequency)),
                                  round(ComputeLatency(max_distance, sampling_frequency)))),
        gain_(ComputeGain(distance, sampling_frequency)),
        sampling_frequency_(sampling_frequency) {} 

Time PropagationLine::latency() const { return delay_filter_.latency(); }

Sample PropagationLine::gain() const { return gain_; }

void PropagationLine::set_gain(Sample gain) { gain_ = gain; }

void PropagationLine::set_distance(const Length distance) {
  gain_ = ComputeGain(distance, sampling_frequency_);
  delay_filter_.set_latency(ComputeLatency(distance, sampling_frequency_));
}

Time PropagationLine::ComputeLatency(const Length distance, 
                                         const Time sampling_frequency) {
  return (Time) (distance / SOUND_SPEED * sampling_frequency);
}

Sample PropagationLine::ComputeGain(const Length distance, 
                                    const Time sampling_frequency) {
  // Please observe that this gain is actually 1/r rule. In fact, 1/r rule has to be
  // normalized to some value. In this software we choose 
  // (SPEED_OF_SOUND/Fs_) / (distance) i.e. the minimum distance.
  return (Sample) 1.0 /ComputeLatency(distance, sampling_frequency);
}

} // namespace sat



