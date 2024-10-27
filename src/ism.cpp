/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */
#include "ism.h"
#include "randomop.h"
#include "salconstants.h"

using mcl::Abs;
using mcl::IirFilter;
using mcl::Point;
using mcl::Pow;
using sal::Int;
using sal::Length;
using sal::Microphone;
using sal::Sample;
using sal::Source;
using sal::Time;

namespace sal {

Ism::Ism(Room* const room, Source* const source, Microphone* const microphone,
         IsmInterpolation interpolation, Int rir_length,
         const Time sampling_frequency)
    : rir_length_(rir_length),
      interpolation_(interpolation),
      room_(room),
      source_(source),
      microphone_(microphone),
      sampling_frequency_(sampling_frequency),
      random_distance_(0),
      peterson_window_(0.004),  // Standard value in Peterson's paper
      modified_(true) {}

void Ism::ProcessBlock(std::span<const Sample> input_data,
                       Buffer& output_buffer) {
  if (modified_) {
    Ism::CalculateRir();
  }

  // TODO: I still need to test the spatialised implementation
  if (microphone_->IsOmni()) {
    mcl::FirFilter filter(rir_);
    std::vector<Sample> temp(input_data.size());
    filter.ProcessBlock(input_data, temp);
    microphone_->AddPlaneWave(temp, mcl::Point(0, 0, 0), output_buffer);
  } else {
    ASSERT(false);
  }
}

/** Calculates the Rir. This is called by Run() before filtering. */
void Ism::CalculateRir() {
  mcl::Matrix<Sample> beta(2, 3);
  std::vector<mcl::IirFilter> filters = room_->wall_filters();
  beta.SetElement(0, 0, filters[0].B()[0]);  // beta_{x1}
  beta.SetElement(0, 1, filters[2].B()[0]);  // beta_{y1}
  beta.SetElement(0, 2, filters[4].B()[0]);  // beta_{z1}

  beta.SetElement(1, 0, filters[1].B()[0]);  // beta_{x2}
  beta.SetElement(1, 1, filters[3].B()[0]);  // beta_{y2}
  beta.SetElement(1, 2, filters[5].B()[0]);  // beta_{z2}

  Length room_x = ((CuboidRoom*)room_)->dimensions().x();
  Length room_y = ((CuboidRoom*)room_)->dimensions().y();
  Length room_z = ((CuboidRoom*)room_)->dimensions().z();

  rir_ = mcl::Zeros<sal::Sample>(rir_length_);

  Time rir_time = ((Time)rir_length_) / ((Time)sampling_frequency_);
  Int n1 = (Int)floor(rir_time / (((Length)room_x) * 2.0)) + 1;
  Int n2 = (Int)floor(rir_time / (((Length)room_y) * 2.0)) + 1;
  Int n3 = (Int)floor(rir_time / (((Length)room_z) * 2.0)) + 1;

  Int max_num_images = 8 * (2 * n1 + 1) * (2 * n2 + 1) * (2 * n3 + 1);

  images_delay_.reserve(max_num_images);
  images_position_.reserve(max_num_images);
  images_int_delay_filter_.reserve(max_num_images);
  images_frac_delay_filter_.reserve(max_num_images);

  mcl::RandomGenerator randn_gen;
  std::vector<sal::Length> rand_delays;

  Int k = 0;
  bool randomisation = (mcl::IsEqual(random_distance_, 0.0)) ? false : true;
  if (randomisation) {
    sal::Time top_limit = random_distance_;
    rand_delays = mcl::Add(mcl::Multiply<sal::Time>(
                               randn_gen.Rand(max_num_images), 2.0 * top_limit),
                           -top_limit);
  }

  for (Int mx = -n1; mx <= n1; ++mx) {
    for (Int my = -n2; my <= n2; ++my) {
      for (Int mz = -n3; mz <= n3; ++mz) {
        for (Int px = 0; px <= 1; ++px) {
          for (Int py = 0; py <= 1; ++py) {
            for (Int pz = 0; pz <= 1; ++pz) {
              Point image_position =
                  ((CuboidRoom*)room_)
                      ->ImageSourcePosition(source_->position(), mx, my, mz, px,
                                            py, pz);

              Time delay =
                  mcl::Subtract(image_position, microphone_->position())
                      .norm() /
                  SOUND_SPEED;

              if (randomisation) {
                delay += rand_delays.at(k++);
              }

              if (round(delay * sampling_frequency_) < 0 ||
                  round(delay * sampling_frequency_) >= rir_length_) {
                continue;
              }

              Sample gid = Pow(beta.GetElement(0, 0), Abs((Sample)mx - px)) *
                           Pow(beta.GetElement(1, 0), Abs((Sample)mx)) *
                           Pow(beta.GetElement(0, 1), Abs((Sample)my - py)) *
                           Pow(beta.GetElement(1, 1), Abs((Sample)my)) *
                           Pow(beta.GetElement(0, 2), Abs((Sample)mz - pz)) *
                           Pow(beta.GetElement(1, 2), Abs((Sample)mz));

              Sample attenuation = gid / (delay * sampling_frequency_);

              images_position_.push_back(image_position);
              images_delay_.push_back(delay);

              WriteSample(delay, attenuation);
            }
          }
        }
      }
    }
  }
}

void Ism::WriteSample(const sal::Time& delay, const sal::Sample& attenuation) {
  sal::Time delay_norm = delay * sampling_frequency_;
  Int id_round = mcl::RoundToInt(delay_norm);
  Int rir_length = rir_.size();

  switch (interpolation_) {
    case none: {
      rir_.at(id_round) += attenuation;
      images_int_delay_filter_.push_back(sal::DelayFilter(id_round, id_round));
      images_frac_delay_filter_.push_back(
          mcl::FirFilter::GainFilter(attenuation));
      break;
    }
    case peterson: {
      // The cutoff frequency is 90% of Nyquist frequency
      sal::Time f_c = 0.9 * (sampling_frequency_ / 2.0);
      sal::Time T_w = peterson_window_;

      sal::Time tau = ((sal::Time)delay_norm) / sampling_frequency_;

      std::vector<sal::Sample> filter_coefficients;
      sal::Int integer_delay =
          (Int)floor(sampling_frequency_ * (-T_w / 2.0 + tau));
      for (Int n = integer_delay + 1;
           n < floor(sampling_frequency_ * (T_w / 2.0 + tau)); ++n) {
        if (n < 0 || n >= ((Int)rir_length)) {
          continue;
        }

        sal::Time t = ((sal::Time)n) / sampling_frequency_ - tau;
        sal::Sample low_pass = 1.0 / 2.0 * (1.0 + cos(2.0 * PI * t / T_w)) *
                               sin(2.0 * PI * f_c * t) / (2.0 * PI * f_c * t);

        // If low_pass is nan it means that t=0 and sinc(0)=1
        if (isnan(low_pass)) {
          low_pass = 1.0;
        }

        filter_coefficients.push_back(attenuation * low_pass);
        rir_.at(n) += attenuation * low_pass;
      }

      sal::Int nneg_integer_delay = (integer_delay < 0) ? 0 : integer_delay;
      images_int_delay_filter_.push_back(
          sal::DelayFilter(nneg_integer_delay, nneg_integer_delay));
      images_frac_delay_filter_.push_back(mcl::FirFilter(filter_coefficients));

      break;
    }
  }
}

void Ism::Update() {
  modified_ = true;
  rir_.clear();
  images_delay_.clear();
  images_position_.clear();
  images_int_delay_filter_.clear();
  images_frac_delay_filter_.clear();
}

}  // namespace sal
