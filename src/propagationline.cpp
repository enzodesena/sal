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
#include "mcl.h"

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
    air_filter_.set_impulse_response(GetAirFilter(distance),
                                     air_filters_update_step_);
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
      throw_line();
      return std::vector<sal::Sample>(0, 1);
  }

//  switch (filter_index) { // 70% humidity N=8
//    case 0:
//      return {0.98429,0.016707,-0.0011537,0.00021181,-5.8202e-05,-1.3261e-06,9.5946e-08,-1.2899e-08};
//      break;
//    case 1:
//      return {0.98006,0.021174,-0.0014268,0.00026649,-7.2555e-05,-2.1401e-06,1.5173e-07,-2.0477e-08};
//      break;
//    case 2:
//      return {0.97471,0.026796,-0.0017488,0.0003342,-8.9858e-05,-3.4481e-06,2.3804e-07,-3.2295e-08};
//      break;
//    case 3:
//      return {0.96797,0.033847,-0.0021175,0.00041747,-0.00011033,-5.5441e-06,3.6945e-07,-5.049e-08};
//      break;
//    case 4:
//      return {0.95949,0.042652,-0.0025218,0.00051905,-0.00013388,-8.8907e-06,5.6502e-07,-7.8022e-08};
//      break;
//    case 5:
//      return {0.94888,0.053586,-0.0029343,0.0006418,-0.00015988,-1.421e-05,8.4635e-07,-1.1867e-07};
//      break;
//    case 6:
//      return {0.93565,0.067067,-0.0033008,0.00078877,-0.00018663,-2.2618e-05,1.2297e-06,-1.7658e-07};
//      break;
//    case 7:
//      return {0.91927,0.08354,-0.0035244,0.00096336,-0.00021069,-3.5809e-05,1.704e-06,-2.5454e-07};
//      break;
//    case 8:
//      return {0.89912,0.10343,-0.003443,0.0011703,-0.00022563,-5.6309e-05,2.1759e-06,-3.4976e-07};
//      break;
//    case 9:
//      return {0.87458,0.12711,-0.0028013,0.0014183,-0.00022033,-8.7768e-05,2.3456e-06,-4.4445e-07};
//      break;
//    case 10:
//      return {0.84506,0.15474,-0.0012167,0.0017259,-0.00017619,-0.00013521,1.4394e-06,-4.8804e-07};
//      break;
//    case 11:
//      return {0.81007,0.18622,0.00185,0.0021334,-6.3385e-05,-0.00020496,-2.3326e-06,-3.7093e-07};
//      break;
//    case 12:
//      return {0.76936,0.22096,0.0071069,0.0027246,0.00016424,-0.00030357,-1.2773e-05,9.0586e-08};
//      break;
//    case 13:
//      return {0.72309,0.25774,0.015395,0.0036628,0.00057488,-0.00043422,-3.7219e-05,1.0678e-06};
//      break;
//    case 14:
//      return {0.67196,0.29463,0.027576,0.0052392,0.0012629,-0.00058819,-8.789e-05,2.2688e-06};
//      break;
//    case 15:
//      return {0.61729,0.32903,0.044309,0.0079258,0.0023467,-0.00072918,-0.00018011,1.6504e-06};
//      break;
//    case 16:
//      return {0.56097,0.35801,0.065766,0.0124,0.0039582,-0.00077432,-0.00032122,-6.7894e-06};
//      break;
//    case 17:
//      return {0.50503,0.37893,0.091413,0.019493,0.006236,-0.00058739,-0.0004868,-3.3849e-05};
//      break;
//    case 18:
//      return {0.45096,0.39024,0.1201,0.030023,0.0093662,3.4433e-07,-0.00060089,-8.9194e-05};
//      break;
//    case 19:
//      return {0.39892,0.39183,0.15051,0.044596,0.013699,0.0011659,-0.00055422,-0.00016829};
//      break;
//    default:
//      throw_line();
//      return std::vector<sal::Sample>(0, 1);
//  }

//  switch (filter_index) { // 70% humidity N=5
//    case 0:
//      return {0.98968,0.010477,-0.00015333,-2.0147e-06,1.0842e-08};
//      break;
//    case 1:
//      return {0.98689,0.013296,-0.00018291,-3.2688e-06,1.424e-08};
//      break;
//    case 2:
//      return {0.98336,0.016855,-0.00021301,-5.3015e-06,1.5998e-08};
//      break;
//    case 3:
//      return {0.97891,0.021335,-0.00023927,-8.5928e-06,1.0837e-08};
//      break;
//    case 4:
//      return {0.97331,0.026957,-0.00025352,-1.3914e-05,-1.4706e-08};
//      break;
//    case 5:
//      return {0.96628,0.033981,-0.00024136,-2.2495e-05,-9.3008e-08};
//      break;
//    case 6:
//      return {0.95751,0.042709,-0.00017846,-3.6285e-05,-2.9922e-07};
//      break;
//    case 7:
//      return {0.9466,0.05348,-2.5039e-05,-5.8314e-05,-8.0331e-07};
//      break;
//    case 8:
//      return {0.93316,0.06665,0.00028196,-9.3195e-05,-1.9811e-06};
//      break;
//    case 9:
//      return {0.91674,0.082568,0.00083926,-0.00014768,-4.6431e-06};
//      break;
//    case 10:
//      return {0.89693,0.10152,0.0017896,-0.00023102,-1.0484e-05};
//      break;
//    case 11:
//      return {0.87337,0.12367,0.0033336,-0.0003545,-2.2897e-05};
//      break;
//    case 12:
//      return {0.84593,0.14892,0.0057347,-0.00052874,-4.826e-05};
//      break;
//    case 13:
//      return {0.81473,0.17682,0.0093063,-0.00075646,-9.7479e-05};
//      break;
//    case 14:
//      return {0.78034,0.2065,0.014367,-0.0010186,-0.00018667};
//      break;
//    case 15:
//      return {0.74378,0.23665,0.021154,-0.0012551,-0.00033438};
//      break;
//    case 16:
//      return {0.70645,0.26573,0.029729,-0.0013489,-0.00055278};
//      break;
//    case 17:
//      return {0.66967,0.29235,0.039945,-0.0011326,-0.00083525};
//      break;
//    case 18:
//      return {0.63415,0.31585,0.051577,-0.00042462,-0.0011534};
//      break;
//    case 19:
//      return {0.59941,0.33652,0.064613,0.00092966,-0.0014763};
//      break;
//    default:
//      throw_line();
//      return std::vector<sal::Sample>(0, 1);
//  }

    
//  switch (filter_index) { // 50% humidity N = 17
//  case 0:
//    return {0.99069,0.004693,0.0027848,0.00097688,2.7325e-05,-0.00015386,-5.8233e-05,5.4476e-06,1.5427e-05,3.0365e-08,6.0581e-08,2.4651e-08,1.2225e-09,-2.757e-09,-8.9489e-10,8.5836e-11,1.191e-10};
//    break;
//  case 1:
//    return {0.98815,0.0059656,0.0035434,0.0012456,3.7134e-05,-0.00019479,-7.4042e-05,6.7652e-06,1.9541e-05,4.9077e-08,9.8122e-08,3.9993e-08,2.0356e-09,-4.443e-09,-1.4475e-09,1.358e-10,1.9111e-10};
//    break;
//  case 2:
//    return {
//      0.98493,0.0075782,0.0045067,0.0015886,5.1062e-05,-0.00024616,-9.4083e-05,8.3365e-06,2.4712e-05,7.9217e-08,1.5882e-07,6.4869e-08,3.4095e-09,-7.1447e-09,-2.3388e-09,2.1314e-10,3.0561e-10};
//    break;
//  case 3:
//    return {
//      0.98083,0.0096186,0.005729,0.0020264,7.1119e-05,-0.00031033,-0.00011945,1.0166e-05,3.1183e-05,1.2766e-07,2.5683e-07,1.0518e-07,5.7514e-09,-1.1457e-08,-3.7735e-09,3.3095e-10,4.8655e-10};
//    break;
//  case 4:
//    return {
//      0.97564,0.012195,0.007278,0.0025856,0.0001004,-0.00039004,-0.00015149,1.2222e-05,3.9242e-05,2.0528e-07,4.1487e-07,1.7049e-07,9.7829e-09,-1.8306e-08,-6.0771e-09,5.0633e-10,7.7007e-10};
//    break;
//  case 5:
//    return {
//      0.96907,0.015441,0.009238,0.0033001,0.00014368,-0.00048826,-0.00019185,1.4405e-05,4.9207e-05,3.2919e-07,6.6918e-07,2.7621e-07,1.6801e-08,-2.911e-08,-9.7632e-09,7.5867e-10,1.2094e-09};
//    break;
//  case 6:
//    return {
//      0.96076,0.019515,0.011713,0.0042137,0.00020842,-0.00060801,-0.00024249,1.6499e-05,6.1418e-05,5.2604e-07,1.0774e-06,4.4721e-07,2.9167e-08,-4.5997e-08,-1.5635e-08,1.1025e-09,1.8794e-09};
//    break;
//  case 7:
//    return {
//      0.95028,0.02461,0.014831,0.0053825,0.00030624,-0.00075189,-0.0003057,1.8092e-05,7.6198e-05,8.3675e-07,1.7305e-06,7.2349e-07,5.1242e-08,-7.2067e-08,-2.4931e-08,1.5279e-09,2.8791e-09};
//    break;
//  case 8:
//    return {
//      0.9371,0.030947,0.018746,0.0068791,0.00045525,-0.0009212,-0.000384,1.8465e-05,9.379e-05,1.3231e-06,2.7712e-06,1.1693e-06,9.1174e-08,-1.1161e-07,-3.9523e-08,1.9524e-09,4.3224e-09};
//    break;
//  case 9:
//    return {
//      0.92058,0.038774,0.023641,0.0087967,0.00068363,-0.0011145,-0.00047998,1.6429e-05,0.00011424,2.0757e-06,4.4203e-06,1.8873e-06,1.6437e-07,-1.701e-07,-6.2157e-08,2.1151e-09,6.3019e-09};
//    break;
//  case 10:
//    return {
//      0.89996,0.048356,0.02973,0.011255,0.0010351,-0.0013247,-0.00059575,1.0116e-05,0.0001372,3.2229e-06,7.0156e-06,3.0411e-06,3.0021e-07,-2.533e-07,-9.6653e-08,1.3555e-09,8.7844e-09};
//    break;
//  case 11:
//    return {
//      0.87438,0.059954,0.037249,0.014408,0.0015768,-0.0015354,-0.00073197,-3.2414e-06,0.00016166,4.9357e-06,1.1063e-05,4.8897e-06,5.5513e-07,-3.6432e-07,-1.4783e-07,-1.8191e-09,1.1361e-08};
//    break;
//  case 12:
//    return {
//      0.84287,0.073779,0.04645,0.018448,0.0024109,-0.0017132,-0.0008859,-2.7446e-05,0.00018546,7.4195e-06,1.7298e-05,7.8397e-06,1.0378e-06,-4.9543e-07,-2.205e-07,-1.0507e-08,1.2698e-08};
//    break;
//  case 13:
//    return {
//      0.80442,0.089934,0.057575,0.023619,0.0036905,-0.0017975,-0.0010477,-6.7114e-05,0.00020479,1.0872e-05,2.675e-05,1.2522e-05,1.9573e-06,-6.0869e-07,-3.1575e-07,-3.0617e-08,9.4346e-09};
//    break;
//  case 14:
//    return {
//      0.75805,0.10831,0.070812,0.030212,0.0056398,-0.0016844,-0.0011936,-0.00012648,0.00021349,1.5371e-05,4.0771e-05,1.9895e-05,3.7124e-06,-5.924e-07,-4.2036e-07,-7.2159e-08,-5.8469e-09};
//    break;
//  case 15:
//    return {
//      0.70295,0.12846,0.086227,0.038563,0.0085772,-0.0012032,-0.0012744,-0.00020599,0.00020281,2.0643e-05,6.0952e-05,3.1377e-05,7.0517e-06,-1.6976e-07,-4.7952e-07,-1.4861e-07,-4.7768e-08};
//    break;
//  case 16:
//    return {
//      0.63871,0.14943,0.10365,0.049019,0.012936,-8.6034e-05,-0.0011948,-0.00029422,0.00016225,2.5687e-05,8.8783e-05,4.8948e-05,1.3337e-05,1.2812e-06,-3.3095e-07,-2.6709e-07,-1.4134e-07};
//    break;
//  case 17:
//    return {
//      0.56557,0.16959,0.12255,0.06187,0.019266,0.0020681,-0.0007817,-0.00035103,8.4022e-05,2.8454e-05,0.00012483,7.5102e-05,2.4911e-05,5.0499e-06,4.4279e-07,-3.8818e-07,-3.1891e-07};
//    break;
//  case 18:
//    return {
//      0.48478,0.18659,0.14181,0.077208,0.028197,0.0058244,0.00026303,-0.00027551,-2.4457e-05,2.6588e-05,0.00016726,0.00011229,4.5428e-05,1.3575e-05,2.8105e-06,-3.0526e-07,-5.894e-07};
//    break;
//  case 19:
//    return {
//      0.39886,0.19741,0.15959,0.094699,0.040312,0.011914,0.0024213,0.00014861,-0.00011112,2.2663e-05,0.00021023,0.00016125,7.9529e-05,3.0868e-05,8.7653e-06,6.4023e-07,-8.2522e-07};
//    break;
//      default:
//      throw_line();
//      return std::vector<sal::Sample>(0, 1);
//  }
}

} // namespace sal



