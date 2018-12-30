/*
 ambisonics.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This include contains definition of classes and functions
 related to higer-order ambisonics.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "ambisonics.hpp"
#include "matrixop.hpp"

namespace sal
{
template<typename T>
size_t AmbisonicsDir<T>::GetNumChannels() const noexcept
{
  return AmbisonicsDir<T>::GetNumChannels(order_);
}
  
template<typename T>
void AmbisonicsDir<T>::ReceiveAdd(
  const mcl::Vector<T>& input,
  const Point& point,
  Buffer<T>& output_buffer) noexcept
{
  // Precompute for performance gain
  const Angle phi = point.phi();
  const T sqrt_2 = mcl::Sqrt(2.0);

  switch (convention_)
  {
  case sqrt2:
  {
    // Zero-th component
    mcl::Add
    (
      input.begin(),
      input.end(),
      output_buffer.begin(GetChannelId(0, 0)),
      output_buffer.begin(GetChannelId(0, 0)));
    
    for (size_t i = 1; i <= order_; ++i)
    {
      // TODO: add 3D components
      mcl::MultiplyAdd
      (
        input.begin(),
        input.end(),
        sqrt_2 * cos(((Angle)i) * phi),
        output_buffer.begin(GetChannelId(i, 1)),
        output_buffer.begin(GetChannelId(i, 1)));
      
      mcl::MultiplyAdd
      (
        input.begin(),
        input.end(),
        sqrt_2 * sin(((Angle)i) * phi),
        output_buffer.begin(GetChannelId(i, -1)),
        output_buffer.begin(GetChannelId(i, -1)));
    }
    break;
  }

#ifdef MCL_LOAD_BOOST
    case N3D: {
      // Precompute for performance gain
      const Angle theta = point.theta();
      const T sqrt_4pi = mcl::Sqrt(4.0*PI);
      
      // Zero-th component
      stream_.Add(0, 0, 1.0*sample);
      
      for (Int degree_n=1; degree_n<=order_; ++degree_n) {
        for (Int order_m=0; order_m<=degree_n; ++order_m) {
          mcl::Complex spherical_harmonic =
              mcl::Pow(-1.0, (T) order_m) *
              (order_m==0 ? 1.0 : sqrt_2) *
              sqrt_4pi *
              mcl::SphericalHarmonic(degree_n, mcl::Abs(order_m), theta, phi);
          
          stream_.Add(degree_n, -order_m,
                      Multiply<sal::T>(signal, mcl::ImagPart(spherical_harmonic)));
          stream_.Add(degree_n, order_m,
                      Multiply<sal::T>(signal, mcl::RealPart(spherical_harmonic)));
          // The order of the two statements above is not random, since the case
    // order_m equals zero should give back the non-zero cosine term,
          // i.e. the one corresponding to the real part.
        }
      }
      break;
    }
#endif

  default:
  {
    ASSERT(false);
    break;
  }
  }
}


template<typename T>
mcl::Vector<T> AmbisonicsDir<T>::HorizontalEncoding(
  size_t order,
  Angle theta)
{
  mcl::Vector<T> output(2 * order + 1);
  output[0] = T(1.0);
  size_t k = 1;
  for (size_t i = 1; i <= order; ++i)
  {
    output[k++] = sqrt(2.0) * cos(((Angle)i) * theta);
    output[k++] = sqrt(2.0) * sin(((Angle)i) * theta);
  }
  return output;
}


template<typename T>
AmbisonicsHorizDec<T>::AmbisonicsHorizDec(
  const size_t order,
  const bool energy_decoding,
  const Time cut_off_frequency,
  const mcl::Vector<Angle>& loudspeaker_angles,
  const bool near_field_correction,
  const Length loudspeakers_distance,
  const Time sampling_frequency,
  const Speed sound_speed)
  : energy_decoding_(energy_decoding)
  , loudspeaker_angles_(loudspeaker_angles)
  , num_loudspeakers_(loudspeaker_angles.size())
  , near_field_correction_(near_field_correction)
  , loudspeakers_distance_(loudspeakers_distance)
  , sampling_frequency_(sampling_frequency)
  , order_(order)
  , mode_matching_matrix_
  (
    mcl::Matrix<T>
    (
      2 * order + 1,
      loudspeaker_angles.size()))
  , max_energy_matrix_
  (
    mcl::Matrix<T>
    (
      2 * order + 1,
      loudspeaker_angles.size()))
{
  using mcl::DigitalFilter;
  nfc_filters_ = mcl::Vector<mcl::DigitalFilter<T>>(2 * order + 1);
  if (near_field_correction_)
  {
    // One filter per order per loudspeaker (inner is per loudspeaker, so that
    // they are all equal).
    nfc_filters_[0] =
      NFCFilter
      (
        0,
        loudspeakers_distance_,
        sampling_frequency_,
        sound_speed);
    size_t k = 1;
    for (size_t i = 1; i <= order; ++i)
    {
      // Instanciating two filters because order higher than two have
      // both degrees +1 and -1
      nfc_filters_[k++] =
        NFCFilter
        (
          i,
          loudspeakers_distance_,
          sampling_frequency_,
          sound_speed);
      nfc_filters_[k++] =
        NFCFilter
        (
          i,
          loudspeakers_distance_,
          sampling_frequency_,
          sound_speed);
    }
    ASSERT(nfc_filters_.size() == 2 * order + 1);
  }

  if (energy_decoding_)
  {
    DigitalFilter<T> cx_high = CrossoverFilterHigh
    (
      cut_off_frequency,
      sampling_frequency);
    DigitalFilter<T> cx_low = CrossoverFilterLow
    (
      cut_off_frequency,
      sampling_frequency);
    // One filter per loudspeaker
    crossover_filters_high_ = mcl::Vector<DigitalFilter<T>>
    (
      num_loudspeakers_,
      cx_high);
    crossover_filters_low_ = mcl::Vector<DigitalFilter<T>>
    (
      num_loudspeakers_,
      cx_low);
  }

  mode_matching_matrix_ = ModeMatchingDec(order_, loudspeaker_angles_);
  max_energy_matrix_ = MaxEnergyDec(order_, loudspeaker_angles_);
}


template<typename T>
mcl::Matrix<T> AmbisonicsHorizDec<T>::ModeMatchingDec(
  Int order,
  const mcl::Vector<Angle>& loudspeaker_angles)
{
  using mcl::Matrix;
  using mcl::Multiply;
  const Int num_loudspeakers = loudspeaker_angles.size();

  Matrix<T> temp(2 * order + 1, num_loudspeakers);

  for (Int i = 0; i < num_loudspeakers; ++i)
  {
    temp.SetColumn
    (
      i,
      AmbisonicsDir<T>::HorizontalEncoding
      (
        order,
        loudspeaker_angles[i]));
  }
  // TODO: implement for non-regular loudspeaker arrays.
  // M_d = temp'*(temp*temp')^(-1);
  // Since temp is unitary (except for a value of 2*order+1),we can compute the
  // inverse by simple transposition.
  return Multiply(Transpose(temp), (T)1.0 / ((T)2 * order + 1));
}


template<typename T>
mcl::Matrix<T> AmbisonicsHorizDec<T>::MaxEnergyDec(
  const size_t order,
  const mcl::Vector<Angle>& loudspeaker_angles)
{
  // TODO: Implement for non-regular loudspeaker arrays.
  mcl::Matrix<T> decoding_matrix(2 * order + 1, 2 * order + 1);
  decoding_matrix.SetElement(0, 0, MaxEnergyDecWeight(0, order));
  Int k = 1;
  for (size_t i = 1; i <= order; ++i)
  {
    decoding_matrix.SetElement(k, k, MaxEnergyDecWeight(i, order));
    k++;
    decoding_matrix.SetElement(k, k, MaxEnergyDecWeight(i, order));
    k++;
  }
  return decoding_matrix;
}


template<typename T>
mcl::Vector<T> AmbisonicsHorizDec<T>::GetFrame(
  const size_t order,
  const size_t sample_id,
  const Buffer<T>& buffer)
{
  ASSERT(order >= 0);
  ASSERT(sample_id >= 0 & sample_id < buffer.num_samples());

  mcl::Vector<T> output(2 * order + 1);
  output[0] =
    buffer.GetSample
    (
      AmbisonicsDir<T>::GetChannelId(0, 0),
      sample_id);
  
  size_t k = 1;
  for (size_t i = 1; i <= order; ++i)
  {
    output[k++] =
      buffer.GetSample
      (
        AmbisonicsDir<T>::GetChannelId(i, 1),
        sample_id);
    output[k++] =
      buffer.GetSample
      (
        AmbisonicsDir<T>::GetChannelId(i, -1),
        sample_id);
  }
  return output;
}


/* AMB_NFC Near-Field Corrected Higher Order Ambisonics.
 OUT_NFC = AMB_NFC(X, FS, THETA, N, LOUDSP_ANGLES, LOUDSP_DIST) produces
 the loudspeaker feeds for near field corrected HOA as described in
 "Spatial Sound Encoding Including Near Field Effect: Introducing
 Length Coding Filters and a Viable, New Ambisonic Format" by
 J. Daniel, AES 23rd, Int. Conf., 2003.
 x is the input vecotr signal, Fs is the sampling frequency (in Hz),
 theta is the angle of the input plane wave, N is the HOA order,
 loudspeaker_angles is the vector of the loudspeaker angles, while
 loudspeakers_distance is the distance of the loudspeakers from the
 centre.
 */
template<typename T>
void AmbisonicsHorizDec<T>::Decode(
  const Buffer<T>& input_buffer,
  Buffer<T>& output_buffer)
{
  ASSERT(input_buffer.num_samples() == output_buffer.num_samples());

  // Cache for speed
  for (size_t sample_id = 0; sample_id < input_buffer.num_samples(); ++sample_id)
  {
    mcl::Vector<T> bformat_frame = GetFrame
    (
      order_,
      sample_id,
      input_buffer);

    // Near-field correcting
    if (near_field_correction_)
    {
      for (size_t i = 0; i < (2 * order_ + 1); ++i)
      {
        bformat_frame[i] = nfc_filters_[i].Filter(bformat_frame[i]);
      }
    }

    // Mode matching decoding
    //  // Ambisonics decoding (mode-matching)
    //  M_d = amb_decoding(N, loudspeaker_angles);
    //  G_format_low = M_d*amb_nfc_filter(B_format, loudspeakers_distance, Fs, c);
    mcl::Vector<T> output = mcl::Multiply
    (
      mode_matching_matrix_,
      bformat_frame);
    if (energy_decoding_)
    {
      // Maximum energy decoding at high frequency
      //  // Ambisonics decoding (max r_e)
      //  g = amb_re_weights_reg(N)*sqrt(5/3);
      //  display('energy');
      //  G = amb_re_weights_matrix(g);
      //  G_format_high = M_d*G*B_format;
      mcl::Vector<T> output_high =
        mcl::Multiply
        (
          mode_matching_matrix_,
          mcl::Multiply(max_energy_matrix_, bformat_frame));

      // Cross-fading high and low
      //  // Generate output
      //  [B_LF, A_LF, B_HF, A_HF] = amb_crossover_filter(cut_off_frequency, Fs);
      //  out_low = filter(B_LF, A_LF, G_format_low, [], 2);
      //  out_high = filter(B_HF, A_HF, G_format_high, [], 2);
      //  out_nfc = out_low + out_high;
      for (size_t i = 0; i < num_loudspeakers_; ++i)
      {
        output[i] = crossover_filters_low_[i].Filter(output[i]) +
          crossover_filters_high_[i].Filter(output_high[i]);
      }
    }

    // Push into each loudspeaker stream
    for (size_t i = 0; i < num_loudspeakers_; ++i)
    {
      output_buffer.SetSample(i, sample_id, output[i]);
    }
  }
}


template<typename T>
mcl::DigitalFilter<T> AmbisonicsHorizDec<T>::CrossoverFilterLow(
  const Time cut_off_frequency,
  const Time sampling_frequency)
{
  T k = tan(PI * cut_off_frequency / sampling_frequency);

  mcl::Vector<T> b_lf(3);
  // b0_lf = k^2/(k^2+2*k+1);
  b_lf[0] = pow(k, 2.0) / (pow(k, 2.0) + 2.0 * k + 1.0);
  // b1_lf = 2*b0_lf;
  b_lf[1] = 2.0 * b_lf[0];
  // b2_lf = b0_lf;
  b_lf[2] = b_lf[0];

  mcl::Vector<T> a(3);
  a[0] = 1.0;
  // a1 = 2*(k^2-1)/(k^2+2*k+1);
  a[1] = 2. * (pow(k, 2.0) - 1.0) / (pow(k, 2.0) + 2.0 * k + 1.0);
  // a2 = (k^2-2*k+1)/(k^2+2*k+1);
  a[2] = (pow(k, 2.0) - 2.0 * k + 1.0) / (pow(k, 2.0) + 2.0 * k + 1.0);

  return mcl::DigitalFilter(b_lf, a);
}


template<typename T>
mcl::DigitalFilter<T> AmbisonicsHorizDec<T>::CrossoverFilterHigh(
  const Time cut_off_frequency,
  const Time sampling_frequency)
{
  T k = tan(PI * cut_off_frequency / sampling_frequency);

  mcl::Vector<T> b_hf(3);
  // b0_hf = 1/(k^2+2*k+1);
  b_hf[0] = 1.0 / (pow(k, 2.0) + 2.0 * k + 1.0);
  // b1_hf = -2*b0_hf;
  b_hf[1] = -2.0 * b_hf[0];
  b_hf[2] = b_hf[0];

  // I add a minus here so that the output of the two filter will need to be
  // added, rather than subtracted as described in the paper.
  b_hf = mcl::Multiply(b_hf, (T) - 1.0);

  // The denominator of the high frequency filter is the same as the low one.
  mcl::DigitalFilter<T> filter_low
  (
    CrossoverFilterLow
    (
      cut_off_frequency,
      sampling_frequency));

  return mcl::DigitalFilter(b_hf, filter_low.GetDenominatorCoeffs());
}


// Near-field correction filters as described in
// "Spatial Sound Encoding Including Near Field Effect: Introducing
// Length Coding Filters and a Viable, New Ambisonic Format" by
// J. Daniel, AES 23rd, Int. Conf., 2003.
template<typename T>
mcl::DigitalFilter<T> AmbisonicsHorizDec<T>::NFCFilter(
  const Int order,
  const Length loudspeaker_distance,
  const Time sampling_frequency,
  const Length sound_speed)
{
  // TODO: implement for orders higher than 6
  if (order > 6)
  {
    ASSERT(false);
  }
  using mcl::Poly;
  using mcl::Ones;
  using mcl::Complex;
  using mcl::Conj;
  using mcl::Prod;
  using mcl::Multiply;
  using mcl::RealPart;

  mcl::Vector<Complex<T>> X_Mq;
  switch (order)
  {
  case 0:
    X_Mq = mcl::Vector<Complex<T>>(0);
    break;
  case 1:
    X_Mq = mcl::Vector<Complex<T>>(1);
    X_Mq[0] = Complex<T>(-2.0, 0.0);
    break;
  case 2:
    X_Mq = mcl::Vector<Complex<T>>(2);
    X_Mq[0] = Complex<T>(-3.0000, 1.7321);
    X_Mq[1] = Conj(X_Mq[0]);
    break;
  case 3:
    X_Mq = mcl::Vector<Complex<T>>(3);
    X_Mq[0] = Complex<T>(-3.6778, 3.5088);
    X_Mq[1] = Conj(X_Mq[0]);
    X_Mq[2] = Complex<T>(-4.6444, 0.0);
    break;
  case 4:
    X_Mq = mcl::Vector<Complex<T>>(4);
    X_Mq[0] = Complex<T>(-4.2076, 5.3148);
    X_Mq[1] = Conj(X_Mq[0]);
    X_Mq[2] = Complex<T>(-5.7924, 1.7345);
    X_Mq[3] = Conj(X_Mq[2]);
    break;
  case 5:
    X_Mq = mcl::Vector<Complex<T>>(5);
    X_Mq[0] = Complex<T>(-4.6493, 7.1420);
    X_Mq[1] = Conj(X_Mq[0]);
    X_Mq[2] = Complex<T>(-6.7039, 3.4853);
    X_Mq[3] = Conj(X_Mq[2]);
    X_Mq[4] = Complex<T>(-7.2935, 0.0);
    break;
  case 6:
    X_Mq = mcl::Vector<Complex<T>>(6);
    X_Mq[0] = Complex<T>(-5.0319, 8.9853);
    X_Mq[1] = Conj(X_Mq[0]);
    X_Mq[2] = Complex<T>(-7.4714, 5.2525);
    X_Mq[3] = Conj(X_Mq[2]);
    X_Mq[4] = Complex<T>(-8.4967, 1.7350);
    X_Mq[5] = Conj(X_Mq[4]);
    break;
  }

  T a = 4.0 * sampling_frequency * loudspeaker_distance / sound_speed;

  // I need to implement A(poly((1+X_mq/a)./(1-X_mq/a))*prod(1-X_mq./a);
  const Int num_samples = X_Mq.size();
  mcl::Vector<Complex<T>> temp_1(num_samples); // (1+X_mq/a)./(1-X_mq/a))
  mcl::Vector<Complex<T>> temp_2(num_samples); // 1-X_mq./a
  for (Int i = 0; i < num_samples; ++i)
  {
    temp_1[i] = (Complex<T>(1.0, 0.0) + X_Mq[i] / Complex<T>(a, 0.0)) /
      (Complex<T>(1.0, 0.0) - X_Mq[i] / Complex<T>(a, 0.0));
    temp_2[i] = (Complex<T>(1.0, 0.0) - X_Mq[i] / Complex<T>(a, 0.0));
  }

  mcl::Vector<T> B = mcl::RealPart(mcl::Poly(mcl::Ones<T>(order)));
  mcl::Vector<T> A = mcl::RealPart(Multiply(Poly(temp_1), Prod(temp_2)));
  mcl::Multiply(B, T(1.0)/A[0], B); // Normalise
  mcl::Multiply(A, T(1.0)/A[0], A); // Normalise
  return mcl::DigitalFilter(B, A);
}

template<typename T>
size_t AmbisonicsDir<T>::GetChannelId(
  const Int degree,
  const size_t order) noexcept
{
  ASSERT(degree >= 0);
  ASSERT(((Int)order) <= std::abs(degree));
  size_t centre_index = 0;
  for (Int degree_id = 0; degree_id <= degree; ++degree_id)
  {
    centre_index = centre_index + 2 * degree_id;
  }
  // 0 + 2*0 = 0 OK
  // 0 + 2*1 = 2 OK
  // 2 + 2*2 = 6 OK
  // 6 + 2*3 = 12 OK
  ASSERT(centre_index + order >= 0);
  return centre_index + order;
}


template<typename T>
size_t AmbisonicsDir<T>::GetNumChannels(
  const size_t max_degree) noexcept
{
  ASSERT(max_degree > 0);
  return (max_degree + 1) * (max_degree + 1); // (N+1)^2
}

} // namespace sal
