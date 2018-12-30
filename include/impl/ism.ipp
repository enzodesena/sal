/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "randomop.hpp"

namespace sal
{
template<typename T>
Ism<T>::Ism(
  const CuboidRoom<T>& room,
  const Source& source,
  const Receiver<T>& receiver,
  const IsmInterpolation interpolation,
  const size_t rir_length,
  const Time sampling_frequency)
  : interpolation_(interpolation)
  , sampling_frequency_(sampling_frequency)
  , random_distance_(0)
  , peterson_window_(0.004) // Standard value in Peterson's paper
  , rir_(rir_length, T(0.0))
{
  Update(room, source, receiver);
}


template<typename T>
void Ism<T>::Run(
  const mcl::Vector<T> input,
  Receiver<T>& receiver,
  Buffer<T>& output_buffer)
{
  // TODO: I still need to test the spatialised implementation
  mcl::Vector<Sample> temp(input.size(), T(0.0));
  mcl::DigitalFilter<T> fir_filter(rir_);
  fir_filter.Filter(input, temp);
  receiver.ReceiveAdd(
    temp,
    Point(0, 0, 0),
    output_buffer);
}


/** Calculates the Rir. This is called by Run() before filtering. */
template<typename T>
void Ism<T>::CalculateRir(
  const CuboidRoom<T>& room,
  const Source& source,
  const Receiver<T>& receiver)
{
  using mcl::Abs;
  using mcl::Pow;
  
  mcl::Matrix<T> beta(2, 3);
  const mcl::Vector<mcl::DigitalFilter<T>>& filters = room.GetWallFilters();
  beta.SetElement(0, 0, filters[0].GetNumeratorCoeffs()[0]); // beta_{x1}
  beta.SetElement(0, 1, filters[2].GetNumeratorCoeffs()[0]); // beta_{y1}
  beta.SetElement(0, 2, filters[4].GetNumeratorCoeffs()[0]); // beta_{z1}

  beta.SetElement(1, 0, filters[1].GetNumeratorCoeffs()[0]); // beta_{x2}
  beta.SetElement(1, 1, filters[3].GetNumeratorCoeffs()[0]); // beta_{y2}
  beta.SetElement(1, 2, filters[5].GetNumeratorCoeffs()[0]); // beta_{z2}

  Length room_x = room.dimensions().x();
  Length room_y = room.dimensions().y();
  Length room_z = room.dimensions().z();
  mcl::SetToZero(rir_);
  const size_t rir_length = rir_.size();
  
  Time rir_time = ((Time)rir_length) / ((Time)sampling_frequency_);
  Int n1 = (Int)floor(rir_time / (((Length)room_x) * 2.0)) + 1;
  Int n2 = (Int)floor(rir_time / (((Length)room_y) * 2.0)) + 1;
  Int n3 = (Int)floor(rir_time / (((Length)room_z) * 2.0)) + 1;

  Int max_num_images = 8 * (2 * n1 + 1) * (2 * n2 + 1) * (2 * n3 + 1);

  images_delay_ = mcl::Vector<Time>(max_num_images);
  images_position_ = mcl::Vector<Point>(max_num_images);
//  images_int_delay_filter_.reserve(max_num_images);
//  images_frac_delay_filter_.reserve(max_num_images);

  mcl::RandomGenerator randn_gen;
  mcl::Vector<sal::Length> rand_delays;

  bool randomisation = (mcl::IsEqual(random_distance_, 0.0)) ? false : true;
  if (randomisation)
  {
    sal::Time top_limit = random_distance_;
    rand_delays = mcl::Add
    (
      mcl::Multiply<sal::Time>
      (
        randn_gen.Rand(max_num_images),
        2.0 * top_limit),
      -top_limit);
  }

  size_t k = 0;
  for (Int mx = -n1; mx <= n1; ++mx)
  {
    for (Int my = -n2; my <= n2; ++my)
    {
      for (Int mz = -n3; mz <= n3; ++mz)
      {
        for (Int px = 0; px <= 1; ++px)
        {
          for (Int py = 0; py <= 1; ++py)
          {
            for (Int pz = 0; pz <= 1; ++pz)
            {
              Point image_position =
                room.ImageSourcePosition
                (
                  source.position(),
                  mx,
                  my,
                  mz,
                  px,
                  py,
                  pz);

              Time delay =
                mcl::Subtract
                (
                  image_position,
                  receiver.position()).norm() / SOUND_SPEED;

              if (randomisation)
              {
                delay += rand_delays[k];
              }

              if (round(delay * sampling_frequency_) < 0 ||
                round(delay * sampling_frequency_) >= rir_length)
              {
                continue;
              }

              T gid = Pow(beta.GetElement(0, 0), Abs((T)mx - px)) *
                Pow(beta.GetElement(1, 0), Abs((T)mx)) *
                Pow(beta.GetElement(0, 1), Abs((T)my - py)) *
                Pow(beta.GetElement(1, 1), Abs((T)my)) *
                Pow(beta.GetElement(0, 2), Abs((T)mz - pz)) *
                Pow(beta.GetElement(1, 2), Abs((T)mz));

              T attenuation = gid / (delay * sampling_frequency_);

              images_position_[k] = (image_position);
              images_delay_[k] = (delay);

              WriteSample(delay, attenuation);
              
              k++;
            }
          }
        }
      }
    }
  }
}


template<typename T>
void Ism<T>::WriteSample(
  const Time delay,
  const T attenuation)
{
  sal::Time delay_norm = delay * sampling_frequency_;
  Int id_round = mcl::RoundToInt(delay_norm);
  size_t rir_length = rir_.size();

  switch (interpolation_)
  {
  case none:
  {
    rir_[id_round] += attenuation;
//    images_int_delay_filter_.push_back(sal::DelayFilter(id_round, id_round));
//    images_frac_delay_filter_.push_back(
//      mcl::DigitalFilter<T>::GainFilter(attenuation));
    break;
  }
  case peterson:
  {
    // The cutoff frequency is 90% of Nyquist frequency
    sal::Time f_c = 0.9 * (sampling_frequency_ / 2.0);
    sal::Time T_w = peterson_window_;

    sal::Time tau = ((sal::Time)delay_norm) / sampling_frequency_;

    mcl::Vector<T> filter_coefficients;
    sal::Int integer_delay = (Int)floor(
      sampling_frequency_ * (-T_w / 2.0 + tau));
    for (Int n = integer_delay + 1;
         n < floor(sampling_frequency_ * (T_w / 2.0 + tau));
         ++n)
    {
      if (n < 0 || n >= ((Int)rir_length))
      {
        continue;
      }

      sal::Time t = ((sal::Time)n) / sampling_frequency_ - tau;
      T low_pass = 1.0 / 2.0 * (1.0 + cos(2.0 * PI * t / T_w)) *
        sin(2.0 * PI * f_c * t) / (2.0 * PI * f_c * t);

      // If low_pass is nan it means that t=0 and sinc(0)=1
      if (isnan(low_pass))
      {
        low_pass = 1.0;
      }

//      filter_coefficients.push_back(attenuation * low_pass);
      rir_[n] += attenuation * low_pass;
    }

//    sal::Int nneg_integer_delay = (integer_delay < 0) ? 0 : integer_delay;
//    images_int_delay_filter_.push_back
//    (
//      sal::DelayFilter
//      (
//        nneg_integer_delay,
//        nneg_integer_delay));
//    images_frac_delay_filter_.push_back(mcl::DigitalFilter<T>(filter_coefficients));

    break;
  }
  }
}


template<typename T>
void Ism<T>::Update(
  const CuboidRoom<T>& room,
  const Source& source,
  const Receiver<T>& receiver)
{
  CalculateRir(room, source, receiver);
}
} // namespace sal
