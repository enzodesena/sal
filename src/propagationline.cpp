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
                                 const sal::Length update_step,
                                 const bool air_filters_active,
                                 const UInt air_filters_update_step) :
        delay_filter_(DelayFilter(round(ComputeLatency(distance, sampling_frequency)),
                                  round(ComputeLatency(max_distance, sampling_frequency)))),
        gain_(ComputeGain(distance, sampling_frequency)),
        sampling_frequency_(sampling_frequency), current_distance_(distance),
        update_step_(update_step),
        updating_distance_(false),
        air_filters_active_(air_filters_active),
        air_filter_(mcl::FirFilter(GetAirFilter(distance))),
        air_filters_update_step_(air_filters_update_step) {
  if (air_filters_active_) {
    air_filter_ = mcl::FirFilter(GetAirFilter(distance));
  }
}

Time PropagationLine::latency() const { return delay_filter_.latency(); }

Sample PropagationLine::gain() const { return gain_; }

void PropagationLine::set_gain(Sample gain) { gain_ = gain; }
  
void PropagationLine::set_update_step(const sal::Length update_step) {
  update_step_ = update_step;
}
  
void PropagationLine::set_distance(const Length distance) {
  gain_ = ComputeGain(distance, sampling_frequency_);
  updating_distance_ = true;
  target_distance_ = distance;
  if (air_filters_active_) {
    air_filter_.UpdateFilter(GetAirFilter(distance), air_filters_update_step_);
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
  
void PropagationLine::Tick() {
  if (updating_distance_) {
    sal::Length new_distance;
    if (mcl::IsSmallerOrEqual(current_distance_, target_distance_)) {
      new_distance = current_distance_ + update_step_;
      if (new_distance > target_distance_) {
        new_distance = target_distance_;
        updating_distance_ = false;
      }
    } else {
      new_distance = current_distance_ - update_step_;
      if (new_distance < target_distance_) {
        new_distance = target_distance_;
        updating_distance_ = false;
      }
    }
    current_distance_ = new_distance;
    delay_filter_.set_latency(ComputeLatency(new_distance, sampling_frequency_));
  }
  
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
  
  
void PropagationLine::Write(const sal::Sample &sample) {
  if (air_filters_active_) {
    delay_filter_.Write(air_filter_.Filter(sample*gain_));
  } else {
    delay_filter_.Write(sample*gain_);
  }
}

std::vector<sal::Sample>
  PropagationLine::GetAirFilter(sal::Length distance) {
  
  std::vector<sal::Length> distances = {1,1.2743,1.6238,2.0691,2.6367,3.3598,4.2813,5.4556,6.9519,8.8587,11.288,14.384,18.33,23.357,29.764,37.927,48.329,61.585,78.476,100};
  
  UInt filter_index = mcl::MinIndex(mcl::Abs(mcl::Subtract(distances, std::vector<sal::Length>(distances.size(), distance))));
  
  switch (filter_index) {
  case 0:
    return {0.99069,0.004693,0.0027848,0.00097688,2.7325e-05,-0.00015386,-5.8233e-05,5.4476e-06,1.5427e-05,3.0365e-08,6.0581e-08,2.4651e-08,1.2225e-09,-2.757e-09,-8.9489e-10,8.5836e-11,1.191e-10};
    break;
  case 1:
    return {0.98815,0.0059656,0.0035434,0.0012456,3.7134e-05,-0.00019479,-7.4042e-05,6.7652e-06,1.9541e-05,4.9077e-08,9.8122e-08,3.9993e-08,2.0356e-09,-4.443e-09,-1.4475e-09,1.358e-10,1.9111e-10};
    break;
  case 2:
    return {
      0.98493,0.0075782,0.0045067,0.0015886,5.1062e-05,-0.00024616,-9.4083e-05,8.3365e-06,2.4712e-05,7.9217e-08,1.5882e-07,6.4869e-08,3.4095e-09,-7.1447e-09,-2.3388e-09,2.1314e-10,3.0561e-10};
    break;
  case 3:
    return {
      0.98083,0.0096186,0.005729,0.0020264,7.1119e-05,-0.00031033,-0.00011945,1.0166e-05,3.1183e-05,1.2766e-07,2.5683e-07,1.0518e-07,5.7514e-09,-1.1457e-08,-3.7735e-09,3.3095e-10,4.8655e-10};
    break;
  case 4:
    return {
      0.97564,0.012195,0.007278,0.0025856,0.0001004,-0.00039004,-0.00015149,1.2222e-05,3.9242e-05,2.0528e-07,4.1487e-07,1.7049e-07,9.7829e-09,-1.8306e-08,-6.0771e-09,5.0633e-10,7.7007e-10};
    break;
  case 5:
    return {
      0.96907,0.015441,0.009238,0.0033001,0.00014368,-0.00048826,-0.00019185,1.4405e-05,4.9207e-05,3.2919e-07,6.6918e-07,2.7621e-07,1.6801e-08,-2.911e-08,-9.7632e-09,7.5867e-10,1.2094e-09};
    break;
  case 6:
    return {
      0.96076,0.019515,0.011713,0.0042137,0.00020842,-0.00060801,-0.00024249,1.6499e-05,6.1418e-05,5.2604e-07,1.0774e-06,4.4721e-07,2.9167e-08,-4.5997e-08,-1.5635e-08,1.1025e-09,1.8794e-09};
    break;
  case 7:
    return {
      0.95028,0.02461,0.014831,0.0053825,0.00030624,-0.00075189,-0.0003057,1.8092e-05,7.6198e-05,8.3675e-07,1.7305e-06,7.2349e-07,5.1242e-08,-7.2067e-08,-2.4931e-08,1.5279e-09,2.8791e-09};
    break;
  case 8:
    return {
      0.9371,0.030947,0.018746,0.0068791,0.00045525,-0.0009212,-0.000384,1.8465e-05,9.379e-05,1.3231e-06,2.7712e-06,1.1693e-06,9.1174e-08,-1.1161e-07,-3.9523e-08,1.9524e-09,4.3224e-09};
    break;
  case 9:
    return {
      0.92058,0.038774,0.023641,0.0087967,0.00068363,-0.0011145,-0.00047998,1.6429e-05,0.00011424,2.0757e-06,4.4203e-06,1.8873e-06,1.6437e-07,-1.701e-07,-6.2157e-08,2.1151e-09,6.3019e-09};
    break;
  case 10:
    return {
      0.89996,0.048356,0.02973,0.011255,0.0010351,-0.0013247,-0.00059575,1.0116e-05,0.0001372,3.2229e-06,7.0156e-06,3.0411e-06,3.0021e-07,-2.533e-07,-9.6653e-08,1.3555e-09,8.7844e-09};
    break;
  case 11:
    return {
      0.87438,0.059954,0.037249,0.014408,0.0015768,-0.0015354,-0.00073197,-3.2414e-06,0.00016166,4.9357e-06,1.1063e-05,4.8897e-06,5.5513e-07,-3.6432e-07,-1.4783e-07,-1.8191e-09,1.1361e-08};
    break;
  case 12:
    return {
      0.84287,0.073779,0.04645,0.018448,0.0024109,-0.0017132,-0.0008859,-2.7446e-05,0.00018546,7.4195e-06,1.7298e-05,7.8397e-06,1.0378e-06,-4.9543e-07,-2.205e-07,-1.0507e-08,1.2698e-08};
    break;
  case 13:
    return {
      0.80442,0.089934,0.057575,0.023619,0.0036905,-0.0017975,-0.0010477,-6.7114e-05,0.00020479,1.0872e-05,2.675e-05,1.2522e-05,1.9573e-06,-6.0869e-07,-3.1575e-07,-3.0617e-08,9.4346e-09};
    break;
  case 14:
    return {
      0.75805,0.10831,0.070812,0.030212,0.0056398,-0.0016844,-0.0011936,-0.00012648,0.00021349,1.5371e-05,4.0771e-05,1.9895e-05,3.7124e-06,-5.924e-07,-4.2036e-07,-7.2159e-08,-5.8469e-09};
    break;
  case 15:
    return {
      0.70295,0.12846,0.086227,0.038563,0.0085772,-0.0012032,-0.0012744,-0.00020599,0.00020281,2.0643e-05,6.0952e-05,3.1377e-05,7.0517e-06,-1.6976e-07,-4.7952e-07,-1.4861e-07,-4.7768e-08};
    break;
  case 16:
    return {
      0.63871,0.14943,0.10365,0.049019,0.012936,-8.6034e-05,-0.0011948,-0.00029422,0.00016225,2.5687e-05,8.8783e-05,4.8948e-05,1.3337e-05,1.2812e-06,-3.3095e-07,-2.6709e-07,-1.4134e-07};
    break;
  case 17:
    return {
      0.56557,0.16959,0.12255,0.06187,0.019266,0.0020681,-0.0007817,-0.00035103,8.4022e-05,2.8454e-05,0.00012483,7.5102e-05,2.4911e-05,5.0499e-06,4.4279e-07,-3.8818e-07,-3.1891e-07};
    break;
  case 18:
    return {
      0.48478,0.18659,0.14181,0.077208,0.028197,0.0058244,0.00026303,-0.00027551,-2.4457e-05,2.6588e-05,0.00016726,0.00011229,4.5428e-05,1.3575e-05,2.8105e-06,-3.0526e-07,-5.894e-07};
    break;
  case 19:
    return {
      0.39886,0.19741,0.15959,0.094699,0.040312,0.011914,0.0024213,0.00014861,-0.00011112,2.2663e-05,0.00021023,0.00016125,7.9529e-05,3.0868e-05,8.7653e-06,6.4023e-07,-8.2522e-07};
    break;
      default:
      assert(false);
      return std::vector<sal::Sample>(0, 1);
  }
}

} // namespace sal



