/*
 propagationline.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "propagationline.h"
#include "salconstants.h"
#include "delayfilter.h"
#include "matrixop.h"

using sal::Length;
using sal::Time;
using sal::Sample;

namespace sal {

PropagationLine::PropagationLine(const Length distance, 
                                 const Time sampling_frequency, 
                                 const Length max_distance,
                                 const bool fractional_delays,
                                 const bool air_filters_active) :
        delay_filter_(DelayFilter((Int) round(ComputeLatency(distance, sampling_frequency)),
                                  (Int) round(ComputeLatency(max_distance, sampling_frequency)))),
        sampling_frequency_(sampling_frequency),
        target_gain_(ComputeGain(distance, sampling_frequency)),
        current_gain_(ComputeGain(distance, sampling_frequency)),
        previous_gain_(ComputeGain(distance, sampling_frequency)),
        updating_gain_(false),
        current_latency_(ComputeLatency(distance, sampling_frequency)),
        updating_latency_(false),
        air_filters_active_(air_filters_active),
        air_filter_(mcl::FirFilter(GetAirFilter(distance))),
        fractional_delays_(fractional_delays) {
  if (air_filters_active_) {
    air_filter_ = mcl::FirFilter(GetAirFilter(distance));
  }
}

Time PropagationLine::latency() const { return delay_filter_.latency(); }

Sample PropagationLine::gain() const { return current_gain_; }

void PropagationLine::set_gain(Sample gain, const sal::Time ramp_time) {
  updating_gain_ = true;
  previous_gain_ = current_gain_;
  target_gain_ = gain;
  gain_update_counter_ = 0;
  gain_update_length_ = round(ramp_time*sampling_frequency_);
}
  
void PropagationLine::set_distance(const Length distance,
                                   const sal::Time ramp_time) {
  updating_latency_ = true;
  previous_latency_ = current_latency_;
  target_latency_ = ComputeLatency(distance, sampling_frequency_);
  latency_update_counter_ = 0;
  latency_update_length_ = round(ramp_time*sampling_frequency_);
  
  set_gain(ComputeGain(distance, sampling_frequency_), ramp_time);
  
  if (air_filters_active_) {
    air_filter_.set_impulse_response(GetAirFilter(distance),
                                     round(ramp_time*sampling_frequency_));
  }
}

void PropagationLine::set_air_filters_active(const bool air_filters_active) {
  air_filters_active_ = air_filters_active;
  if (air_filters_active_ == false) {
    air_filter_.Reset();
  }
}
  
/** Resets the state of the filter */
void PropagationLine::Reset() {
  delay_filter_.Reset();
}
  
void PropagationLine::Update() {
  if (updating_gain_) {
    if (gain_update_counter_ == gain_update_length_) {
      current_gain_ = target_gain_;
      updating_gain_ = false;
    } else {
      current_gain_ = mcl::LinearInterpolation(0.0, previous_gain_,
                                               gain_update_length_, target_gain_,
                                               gain_update_counter_);
    }
    gain_update_counter_++;
  }
  
  if (updating_latency_) {
    
    if (latency_update_counter_ == latency_update_length_) {
      current_latency_ = target_latency_;
      updating_latency_ = false;
    } else {
      current_latency_ = mcl::LinearInterpolation(0.0, previous_latency_,
                                                  latency_update_length_, target_latency_,
                                                  latency_update_counter_);
    }
    
    latency_update_counter_ ++;
    delay_filter_.set_latency(round(current_latency_));
  }
}
  
void PropagationLine::Tick() {
  Update();
  delay_filter_.Tick();
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
  return (Sample) 1.0 / ComputeLatency(distance, sampling_frequency);
}
  
sal::Length PropagationLine::distance() const {
  return current_latency_/sampling_frequency_*SOUND_SPEED;
}
  
void PropagationLine::Write(const sal::Sample &sample) {
  if (air_filters_active_) {
    delay_filter_.Write(air_filter_.Filter(sample));
  } else {
    delay_filter_.Write(sample);
  }
}
  
sal::Sample PropagationLine::Read() const {
  if (fractional_delays_) {
    return delay_filter_.FractionalRead(current_latency_)*current_gain_;
  } else {
    return delay_filter_.Read() * current_gain_;
  }
}
  

std::vector<sal::Sample>
  PropagationLine::GetAirFilter(sal::Length distance) {
  
  std::vector<sal::Length> distances = {1,1.2743,1.6238,2.0691,2.6367,3.3598,
                                        4.2813,5.4556,6.9519,8.8587,11.288,
                                        14.384,18.33,23.357,29.764,37.927,
                                        48.329,61.585,78.476,100};
  
  UInt filter_index =
  mcl::MinIndex(mcl::Abs(mcl::Subtract(distances,
                                       std::vector<sal::Length>(distances.size(),
                                                                distance))));
  switch (filter_index) { // 70% humidity N=4
    case 0:
      return {0.98968,0.010477,-0.00015333,-2.0147e-06};
      break;
    case 1:
      return {0.98689,0.013296,-0.00018291,-3.2688e-06};
      break;
    case 2:
      return {0.98336,0.016855,-0.00021301,-5.3015e-06};
      break;
    case 3:
      return {0.97891,0.021335,-0.00023927,-8.5928e-06};
      break;
    case 4:
      return {0.97331,0.026957,-0.00025352,-1.3914e-05};
      break;
    case 5:
      return {0.96628,0.033981,-0.00024136,-2.2495e-05};
      break;
    case 6:
      return {0.95751,0.042709,-0.00017846,-3.6285e-05};
      break;
    case 7:
      return {0.9466,0.05348,-2.5039e-05,-5.8314e-05};
      break;
    case 8:
      return {0.93316,0.06665,0.00028196,-9.3195e-05};
      break;
    case 9:
      return {0.91674,0.082568,0.00083926,-0.00014768};
      break;
    case 10:
      return {0.89693,0.10152,0.0017896,-0.00023102};
      break;
    case 11:
      return {0.87337,0.12367,0.0033336,-0.0003545};
      break;
    case 12:
      return {0.84593,0.14892,0.0057347,-0.00052874};
      break;
    case 13:
      return {0.81473,0.17682,0.0093063,-0.00075646};
      break;
    case 14:
      return {0.78034,0.2065,0.014367,-0.0010186};
      break;
    case 15:
      return {0.74378,0.23665,0.021154,-0.0012551};
      break;
    case 16:
      return {0.70645,0.26573,0.029729,-0.0013489};
      break;
    case 17:
      return {0.66967,0.29235,0.039945,-0.0011326};
      break;
    case 18:
      return {0.63415,0.31585,0.051577,-0.00042462};
      break;
    case 19:
      return {0.59941,0.33652,0.064613,0.00092966};
      break;
    default:
      throw_line("");
      return std::vector<sal::Sample>(0, 1);
  }
}

} // namespace sal



