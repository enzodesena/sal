/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "ambisonics.hpp"
#include "receiver.hpp"

namespace sal
{
inline bool AmbisonicsMicTest()
{
  // TODO: Only testing for 2nd order

  // Testing Ambisonics encoding
  const size_t N = 2; // Ambisonics order

  Receiver<Sample> mic_a
  (
    AmbisonicsDir<Sample>(N),
    Point(0.0, 0.0, 0.0),
    mcl::AxAng2Quat(0.0, 1.0, 0.0, -PI / 2.0));
  Buffer<Sample> stream_a(AmbisonicsDir<Sample>::GetNumChannels(N), 1);

  Sample sample = 0.3;
  mic_a.ReceiveAdd
  (
    mcl::UnaryVector<Sample>(sample),
    Point(1.0, 0.0, 0.0),
    stream_a);

  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(0, 0), 0), sample * 1.000000000000000));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(1, 1), 0), sample * 1.414213562373095));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(1, -1), 0), sample * 0.0));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(2, 1), 0), sample * 1.414213562373095));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(2, -1), 0), sample * 0.0));
  mic_a.ResetState();
  stream_a.ResetSamples();

  mic_a.ReceiveAdd(mcl::UnaryVector<Sample>(sample), Point(0.0, 1.0, 0.0), stream_a);
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(0, 0), 0), sample * 1.000000000000000));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(1, -1), 0), sample * 1.414213562373095));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(1, 1), 0), sample * 0.0));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(2, 1), 0), sample * (-1.414213562373095)));
  ASSERT(mcl::IsApproximatelyEqual(stream_a.GetSample(AmbisonicsDir<Sample>::GetChannelId(2, -1), 0), sample * 0.0));

#ifdef MCL_LOAD_BOOST
  // Testing Ambisonics encoding
  const Int N_b = 3; // Ambisonics order
  AmbisonicsMic mic_b(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,1,0,-PI/2.0), N_b, N3D);
  BFormatStream* stream_b = mic_b.stream();
  
  
  sample = 0.5;
  mic_b.ReceiveAdd(sample, Point(1.0, 0.0, 0.0));
  
  // theta and phi are the spherical coordinates in the reference system of:
  // http://ambisonics.ch/standards/channels/
  // theta is the azimuth angle (angle formed with x-axis)
  // phi is the elevation angle (angle formed with the xy-plane)
  Angle theta = 0;
  Angle phi = 0;
  
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(0, 0), sample*1.000000000000000));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, 1), sample*mcl::Sqrt(3.0)*cos(phi)*cos(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, 0), sample*sqrt(3.0)*sin(phi)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, -1), sample*cos(phi)*sin(theta)));
         
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 2), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*cos(2.0*theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 1), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*cos(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 0), sample*mcl::Sqrt(5.0)/2.0*(3.0*pow(sin(phi),2.0)-1.0)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, -1), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*sin(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, -2), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*sin(2.0*theta)));
  
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, -2), sample*mcl::Sqrt(105.0)/2.0*(pow(cos(phi),2.0))*sin(phi)*sin(2.0*theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, 1), sample*mcl::Sqrt(21.0/8.0)*(5*pow(sin(phi),2.0)-1.0)*cos(phi)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, 3), sample*mcl::Sqrt(35.0/8.0)*pow(cos(phi),3.0)*cos(3.0*theta)));
  
  sample = 0.2;
  mic_b.ReceiveAdd(sample, Point(0.5, 0.5, 1.0/sqrt(2.0)));
  
  theta = PI/4.0;
  phi = PI/4.0;
  
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(0, 0), sample*1.000000000000000));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, 1), sample*mcl::Sqrt(3.0)*cos(phi)*cos(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, 0), sample*sqrt(3.0)*sin(phi)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(1, -1), sample*sqrt(3.0)*cos(phi)*sin(theta)));
  
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 2), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*cos(2.0*theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 1), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*cos(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, 0), sample*mcl::Sqrt(5.0)/2.0*(3.0*pow(sin(phi),2.0)-1.0)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, -1), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*sin(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(2, -2), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*sin(2.0*theta)));
  
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, -2), sample*mcl::Sqrt(105.0)/2.0*(pow(cos(phi),2.0))*sin(phi)*sin(2.0*theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, 1), sample*mcl::Sqrt(21.0/8.0)*cos(phi)*(5.0*pow(sin(phi),2.0)-1.0)*cos(theta)));
  ASSERT(mcl::IsApproximatelyEqual(stream_b->Pull(3, 3), sample*mcl::Sqrt(35.0/8.0)*pow(cos(phi),3.0)*cos(3.0*theta)));
#endif

  return true;
}


inline bool AmbisonicsHorizDecTest()
{
  using mcl::IsApproximatelyEqual;
  using mcl::DigitalFilter;
  using mcl::Complex;
  using mcl::RealPart;

  // Testing Crossover filters
  DigitalFilter<Sample> filter_low(AmbisonicsHorizDec<Sample>::CrossoverFilterLow(380, 48000));
  mcl::Vector<Sample> num_lf_cmp(3);
  num_lf_cmp[0] = 0.000589143208472;
  num_lf_cmp[1] = 0.001178286416944;
  num_lf_cmp[2] = 0.000589143208472;
  ASSERT(mcl::IsApproximatelyEqual(filter_low.GetNumeratorCoeffs(), num_lf_cmp));
  mcl::Vector<Sample> den_lf_cmp(3);
  den_lf_cmp[0] = 1.000000000000000;
  den_lf_cmp[1] = -1.902910910316590;
  den_lf_cmp[2] = 0.905267483150478;
  ASSERT(mcl::IsApproximatelyEqual(filter_low.GetDenominatorCoeffs(), den_lf_cmp));

  DigitalFilter<Sample> filter_high(AmbisonicsHorizDec<Sample>::CrossoverFilterHigh(380, 48000));
  mcl::Vector<Sample> num_hf_cmp(3);
  num_hf_cmp[0] = -0.952044598366767;
  num_hf_cmp[1] = 1.904089196733534;
  num_hf_cmp[2] = -0.952044598366767;
  ASSERT(mcl::IsApproximatelyEqual(filter_high.GetNumeratorCoeffs(), num_hf_cmp));
  mcl::Vector<Sample> den_hf_cmp(3);
  den_hf_cmp[0] = 1.000000000000000;
  den_hf_cmp[1] = -1.902910910316590;
  den_hf_cmp[2] = 0.905267483150478;
  ASSERT(mcl::IsApproximatelyEqual(filter_high.GetDenominatorCoeffs(), den_hf_cmp));

  // Testing near field correction filter by J. Daniel

  const Time Fs = 44100.0;
  const Length R = 1.0;
  const Speed c = 343.0;
  Complex<Sample> x1(-3, 1.7321);
  Complex<Sample> x2 = conj(x1);
  Complex<Sample> a(4.0 * Fs * R / c, 0.0);
  Complex<Sample> one(1.0, 0.0);
  mcl::Vector<Sample> BB(3);
  BB[0] = 1;
  BB[1] = -2;
  BB[2] = 1;
  mcl::Vector<Sample> AA(3);
  AA[0] = RealPart((one - x1 / a) * (one - x2 / a));
  AA[1] = RealPart(
    -(one - x1 / a) * (one + x2 / a) - (one + x1 / a) * (one - x2 / a));
  AA[2] = RealPart((one + x1 / a) * (one + x2 / a));
  // Checking straight from a tested Matlab implementation:
  ASSERT(mcl::IsApproximatelyEqual(AA[0], 1.011712037681334));
  ASSERT(mcl::IsApproximatelyEqual(AA[1], -1.999909257970665));
  ASSERT(mcl::IsApproximatelyEqual(AA[2], 0.988378704348000));
  mcl::Multiply(BB, Sample(1.0)/AA[0], BB); // Normalise
  mcl::Multiply(AA, Sample(1.0)/AA[0], AA); // Normalise

  DigitalFilter<Sample> filter_a = AmbisonicsHorizDec<Sample>::NFCFilter(2, R, Fs, c);

  ASSERT(mcl::IsApproximatelyEqual(filter_a.GetNumeratorCoeffs(), BB, 1.0E-3));
  ASSERT(mcl::IsApproximatelyEqual(filter_a.GetDenominatorCoeffs(), AA, 1.0E-3));

  DigitalFilter<Sample> filter_b = AmbisonicsHorizDec<Sample>::NFCFilter(1, R, Fs, c);
  // TODO: I copied this values from my Matlab implementation,
  // but I am not sure whether that implementation was tested for first-order.
  ASSERT(mcl::IsApproximatelyEqual(filter_b.GetNumeratorCoeffs()[0], 1.0/1.003888888888889)); // Unnormalised : 1.0
  ASSERT(mcl::IsApproximatelyEqual(filter_b.GetNumeratorCoeffs()[1], -1.0/1.003888888888889)); // Unnormalised : 1.0
  ASSERT(mcl::IsApproximatelyEqual(filter_b.GetDenominatorCoeffs()[0], 1.0)); // Unnormalised : 1.003888888888889
  ASSERT(mcl::IsApproximatelyEqual(filter_b.GetDenominatorCoeffs()[1], -0.996111111111111/1.003888888888889)); // Unnormalised : -0.996111111111111

  DigitalFilter<Sample> filter_c = AmbisonicsHorizDec<Sample>::NFCFilter(0, R, Fs, c);
  ASSERT(mcl::IsApproximatelyEqual(filter_c.GetNumeratorCoeffs(), mcl::UnaryVector<Sample>(1.0)));
  ASSERT(mcl::IsApproximatelyEqual(filter_c.GetDenominatorCoeffs(), mcl::UnaryVector<Sample>(1.0)));

  // Testing loudspeaker placement

  const Int M = 5; // num loudspeakers
  const Angle phi0 = 2.0 * PI / ((Angle)M);
  mcl::Vector<Angle> loudspeaker_angles =
    mcl::Multiply(mcl::ColonOperator<Angle>(0, M - 1), phi0);
  ASSERT(mcl::IsApproximatelyEqual(loudspeaker_angles[0], 0.0));
  ASSERT(mcl::IsApproximatelyEqual(loudspeaker_angles[1], 1.256637061435917));
  ASSERT(mcl::IsApproximatelyEqual(loudspeaker_angles[4], 4.0 * 1.256637061435917));

  // Testing decoding matrix

  mcl::Matrix<Sample> decoding_matrix =
    AmbisonicsHorizDec<Sample>::ModeMatchingDec(2, loudspeaker_angles);
  ASSERT(decoding_matrix.num_columns() == 5);
  ASSERT(decoding_matrix.num_rows() == 5);
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(0, 0), 0.200000000000000));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(1, 0), 0.200000000000000));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(4, 0), 0.200000000000000));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(0, 1), 0.282842712474619));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(3, 1), -0.228824561127074));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(0, 2), 0.0));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(3, 2), -0.166250775110981));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(2, 3), 0.087403204889764));
  ASSERT(mcl::IsApproximatelyEqual(decoding_matrix.GetElement(4, 4), -0.166250775110981));

  // Testing Ambisonics mode-matching decoding

  using mcl::Sum;
  using mcl::Cos;
  using mcl::ColonOperator;
  using mcl::Multiply;

  const Int order = 2;
  const Int MM = 5; // num loudspeakers
  const Int num_theta(100);
  mcl::Vector<Angle> thetas = mcl::LinSpace(0.0, 2.0 * PI, num_theta);

  Receiver<Sample> mic_a
  (
    AmbisonicsDir<Sample>(order),
    Point(0.0, 0.0, 0.0),
    Quaternion::Identity());
  
  AmbisonicsHorizDec<Sample> decoder_a
  (
    order,
    false, // energy_decoding,
    1.0, // cut_off_frequency,
    loudspeaker_angles,
    false, //near_field_correction,
    1.0, //loudspeakers_distance,
    1.0, // sampling_frequency,
    SOUND_SPEED);
  Buffer<Sample> bformat_buffer(AmbisonicsDir<Sample>::GetNumChannels(order), 1);
  Buffer<Sample> output_buffer(loudspeaker_angles.size(), 1);

  for (size_t theta_index = 0; theta_index < num_theta; ++theta_index)
  {
    output_buffer.ResetSamples();
    bformat_buffer.ResetSamples();
    mic_a.ResetState();

    Angle theta(thetas[theta_index]);

    mic_a.ReceiveAdd
    (
      mcl::UnaryVector<Sample>(1.0),
      Point(cos(theta), sin(theta), 0.0),
      bformat_buffer);
    decoder_a.Decode(bformat_buffer, output_buffer);

    Sample output = output_buffer.GetSample(0, 0);
    // poletti_pan = 1.0/M*(1.0+2.0*sum(cos((1:N)*theta)));
    Sample poletti_pan = 1.0 / MM * (1.0 + 2.0 *
      Sum(Cos(Multiply(ColonOperator<Angle>(1, order), (Angle)theta))));
    ASSERT(mcl::IsApproximatelyEqual(output, poletti_pan));
  }

  // Testing ambisonics maximum energy vector weights

  ASSERT(mcl::IsApproximatelyEqual(AmbisonicsHorizDec<Sample>::MaxEnergyDecWeight(0, 2), 1.0));
  ASSERT(mcl::IsApproximatelyEqual(AmbisonicsHorizDec<Sample>::MaxEnergyDecWeight(1, 2), cos(PI / (6.0))));
  ASSERT(mcl::IsApproximatelyEqual(AmbisonicsHorizDec<Sample>::MaxEnergyDecWeight(2, 2), cos(2.0 * PI / (6.0))));

  // Testing ambisonics maximum energy vector decoding matrix

  mcl::Matrix<Sample> max_re_dec =
    AmbisonicsHorizDec<Sample>::MaxEnergyDec(2, loudspeaker_angles);
  mcl::Matrix<Sample> max_re_dec_cmp(5, 5);
  max_re_dec_cmp.SetElement(0, 0, 1.0);
  max_re_dec_cmp.SetElement(1, 1, cos(PI / (6.0)));
  max_re_dec_cmp.SetElement(2, 2, cos(PI / (6.0)));
  max_re_dec_cmp.SetElement(3, 3, cos(2.0 * PI / (6.0)));
  max_re_dec_cmp.SetElement(4, 4, cos(2.0 * PI / (6.0)));
  ASSERT(mcl::IsApproximatelyEqual(max_re_dec, max_re_dec_cmp));

  // A complete test comparing with Matlab's tested implementation
  Receiver<Sample> mic_b(mic_a);
  mic_b.ResetState();
  const size_t num_samples = 4;
  Buffer<Sample> stream_b(AmbisonicsDir<Sample>::GetNumChannels(order), num_samples);
  Buffer<Sample> output_b(loudspeaker_angles.size(), num_samples);

  AmbisonicsHorizDec<Sample> decoder_b
  (
    order,
    true, // energy_decoding,
    1200, // cut_off_frequency,
    loudspeaker_angles,
    true, //near_field_correction,
    2.0, //loudspeakers_distance,
    44100, // sampling_frequency,
    SOUND_SPEED);
  const Angle theta_b = PI / 4.0;
  mic_b.ReceiveAdd(
    mcl::UnaryVector<Sample>(0.7),
    Point(cos(theta_b), sin(theta_b), 0.0),
    stream_b);
  decoder_b.Decode(stream_b, output_b);

  mcl::Vector<Sample> output_0_cmp(4);
  output_0_cmp[0] = -0.261851079581371;
  output_0_cmp[1] = 0.091643993620079;
  output_0_cmp[2] = 0.077627205799285;
  output_0_cmp[3] = 0.065733018793426;

  Signal<Sample> output_1_cmp(4);
  output_1_cmp[0] = -0.367676280830305;
  output_1_cmp[1] = 0.131355135297581;
  output_1_cmp[2] = 0.112736846228488;
  output_1_cmp[3] = 0.096652325199862;

  Signal<Sample> output_2_cmp(4);
  output_2_cmp[0] = 0.024600010853482;
  output_2_cmp[1] = -0.013376971132794;
  output_2_cmp[2] = -0.013873224419426;
  output_2_cmp[3] = -0.013799068046006;

  Signal<Sample> output_3_cmp(4);
  output_3_cmp[0] = -0.027480352752649;
  output_3_cmp[1] = 0.012392179960282;
  output_3_cmp[2] = 0.012315424782320;
  output_3_cmp[3] = 0.011904504267406;

  Signal<Sample> output_4_cmp(4);
  output_4_cmp[0] = 0.042910353779761;
  output_4_cmp[1] = -0.018453086209685;
  output_4_cmp[2] = -0.017379369470917;
  output_4_cmp[3] = -0.016125504574487;

  ASSERT(mcl::IsApproximatelyEqual(output_b.GetChannelReference(0), output_0_cmp));
  ASSERT(mcl::IsApproximatelyEqual(output_b.GetChannelReference(1), output_1_cmp));
  ASSERT(mcl::IsApproximatelyEqual(output_b.GetChannelReference(2), output_2_cmp));
  ASSERT(mcl::IsApproximatelyEqual(output_b.GetChannelReference(3), output_3_cmp));
  ASSERT(mcl::IsApproximatelyEqual(output_b.GetChannelReference(4), output_4_cmp));

  return true;
}
} // namespace sal
