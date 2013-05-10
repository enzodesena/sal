/*
 ambisonics_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "ambisonics.h"
#include "microphone.h"
#include "stream.h"
#include "mcl.h"

namespace sat {
  
bool AmbisonicsMic::Test() {
  using mcl::IsEqual;
  
  // TODO: Only testing for 2nd order
  
  // Testing Ambisonics encoding
  const UInt N = 2; // Ambisonics order
  AmbisonicsMic mic_a(Point(0.0,0.0,0.0), 0.0, 0.0, 0.0,
                      N);
  BFormatStream* stream_a = mic_a.stream();
  
  Sample sample = 0.3;
  mic_a.RecordPlaneWave(sample, Point(1.0, 0.0, 0.0));
  
  assert(IsEqual(stream_a->Pull(0, 0), sample*1.000000000000000));
  assert(IsEqual(stream_a->Pull(1, 1), sample*1.414213562373095));
  assert(IsEqual(stream_a->Pull(1, -1), sample*0.0));
  assert(IsEqual(stream_a->Pull(2, 1), sample*1.414213562373095));
  assert(IsEqual(stream_a->Pull(2, -1), sample*0.0));
  
  mic_a.RecordPlaneWave(sample, Point(0.0, 1.0, 0.0));
  assert(IsEqual(stream_a->Pull(0, 0), sample*1.000000000000000));
  assert(IsEqual(stream_a->Pull(1, -1), sample*1.414213562373095));
  assert(IsEqual(stream_a->Pull(1, 1), sample*0.0));
  assert(IsEqual(stream_a->Pull(2, 1), sample*(-1.414213562373095)));
  assert(IsEqual(stream_a->Pull(2, -1), sample*0.0));
  
  

  return true;
}

  
bool AmbisonicsHorizDec::Test() {
  
  using mcl::IsEqual;
  using mcl::IirFilter;
  using mcl::IsEqual;
  using mcl::Complex;
  using mcl::RealPart;
  
  // Testing Crossover filters
  IirFilter filter_low(CrossoverFilterLow(380, 48000));
  std::vector<Sample> num_lf_cmp(3);
  num_lf_cmp[0] = 0.000589143208472;
  num_lf_cmp[1] = 0.001178286416944;
  num_lf_cmp[2] = 0.000589143208472;
  assert(IsEqual(filter_low.B(), num_lf_cmp));
  std::vector<Sample> den_lf_cmp(3);
  den_lf_cmp[0] = 1.000000000000000;
  den_lf_cmp[1] = -1.902910910316590;
  den_lf_cmp[2] = 0.905267483150478;
  assert(IsEqual(filter_low.A(), den_lf_cmp));
  
  IirFilter filter_high(CrossoverFilterHigh(380, 48000));
  std::vector<Sample> num_hf_cmp(3);
  num_hf_cmp[0] = -0.952044598366767;
  num_hf_cmp[1] = 1.904089196733534;
  num_hf_cmp[2] = -0.952044598366767;
  assert(IsEqual(filter_high.B(), num_hf_cmp));
  std::vector<Sample> den_hf_cmp(3);
  den_hf_cmp[0] = 1.000000000000000;
  den_hf_cmp[1] = -1.902910910316590;
  den_hf_cmp[2] = 0.905267483150478;
  assert(IsEqual(filter_high.A(), den_hf_cmp));
  
  
  // Testing near field correction filter by J. Daniel
  
  const Time Fs = 44100.0;
  const Length R = 1.0;
  const Speed c = 343.0;
  Complex x1(-3, 1.7321);
  Complex x2 = conj(x1);
  Complex a(4.0*Fs*R/c, 0.0);
  Complex one(1.0, 0.0);
  std::vector<Sample> BB(3);
  BB[0] = 1;
  BB[1] = -2;
  BB[2] = 1;
  std::vector<Sample> AA(3);
  AA[0] = RealPart((one-x1/a)*(one-x2/a));
  AA[1] = RealPart(-(one-x1/a)*(one+x2/a)-(one+x1/a)*(one-x2/a));
  AA[2] = RealPart((one+x1/a)*(one+x2/a));
  // Checking straight from a tested Matlab implementation:
  assert(IsEqual(AA[0], 1.011712037681334));
  assert(IsEqual(AA[1], -1.999909257970665));
  assert(IsEqual(AA[2], 0.988378704348000));
  
  IirFilter filter_a = NFCFilter(2, R, Fs, c);
  
  assert(IsEqual(filter_a.B(), BB));
  assert(IsEqual(filter_a.A(), AA));
  
  IirFilter filter_b = NFCFilter(1, R, Fs, c);
  // TODO: I copied this values from my Matlab implementation,
  // but I am not sure whether that implementation was tested for first-order.
  assert(IsEqual(filter_b.B()[0], 1.0));
  assert(IsEqual(filter_b.B()[1], -1.0));
  assert(IsEqual(filter_b.A()[0], 1.003888888888889));
  assert(IsEqual(filter_b.A()[1], -0.996111111111111));

  IirFilter filter_c = NFCFilter(0, R, Fs, c);
  assert(IsEqual(filter_c.B(), mcl::UnaryVector<Sample>(1.0)));
  assert(IsEqual(filter_c.A(), mcl::UnaryVector<Sample>(1.0)));
    
  
  // Testing loudspeaker placement
  
  const UInt M = 5; // num loudspeakers
  const Angle phi0 = 2.0*PI/((Angle) M);
  std::vector<Angle> loudspeaker_angles =
          mcl::Multiply(mcl::ColonOperator<Angle>(0, M-1), phi0);
  assert(IsEqual(loudspeaker_angles[0], 0.0));
  assert(IsEqual(loudspeaker_angles[1], 1.256637061435917));
  assert(IsEqual(loudspeaker_angles[4], 4.0*1.256637061435917));
  
  
  // Testing decoding matrix
  
  mcl::Matrix<Sample> decoding_matrix =
          AmbisonicsHorizDec::ModeMatchingDec(2, loudspeaker_angles);
  assert(decoding_matrix.num_columns() == 5);
  assert(decoding_matrix.num_rows() == 5);
  assert(IsEqual(decoding_matrix.element(0, 0), 0.200000000000000));
  assert(IsEqual(decoding_matrix.element(1, 0), 0.200000000000000));
  assert(IsEqual(decoding_matrix.element(4, 0), 0.200000000000000));
  assert(IsEqual(decoding_matrix.element(0, 1), 0.282842712474619));
  assert(IsEqual(decoding_matrix.element(3, 1), -0.228824561127074));
  assert(IsEqual(decoding_matrix.element(0, 2), 0.0));
  assert(IsEqual(decoding_matrix.element(3, 2), -0.166250775110981));
  assert(IsEqual(decoding_matrix.element(2, 3), 0.087403204889764));
  assert(IsEqual(decoding_matrix.element(4, 4), -0.166250775110981));
  
  
  // Testing Ambisonics mode-matching decoding
  
  using mcl::Sum;
  using mcl::Cos;
  using mcl::ColonOperator;
  using mcl::Multiply;
  
  const UInt order = 2;
  const UInt MM = 5; // num loudspeakers
  const UInt num_theta(100);
  std::vector<Angle> thetas = mcl::LinSpace(0.0, 2.0*PI, num_theta);
  
  AmbisonicsMic mic_a(Point(0.0,0.0,0.0), 0.0, 0.0, 0.0, order);
  BFormatStream* stream_a = mic_a.stream();
  
  AmbisonicsHorizDec decoder_a(order,
                               false, // energy_decoding,
                               1.0, // cut_off_frequency,
                               loudspeaker_angles,
                               false, //near_field_correction,
                               1.0, //loudspeakers_distance,
                               1.0, // sampling_frequency,
                               SOUND_SPEED,
                               stream_a);
  MonoStream* loudspeaker_0_stream = decoder_a.stream(0);
  
  for (UInt theta_index=0; theta_index<num_theta; ++theta_index) {
    Angle theta(thetas[theta_index]);
    
    mic_a.RecordPlaneWave(1.0, Point(cos(theta), sin(theta), 0.0));
    decoder_a.Decode();
    
    Sample output = loudspeaker_0_stream->Pull();
    // poletti_pan = 1.0/M*(1.0+2.0*sum(cos((1:N)*theta)));
    Sample poletti_pan = 1.0/MM*(1.0+2.0*
            Sum(Cos(Multiply(ColonOperator<Angle>(1, order), (Angle) theta))));
    assert(IsEqual(output, poletti_pan));
  }
  
  
  // Testing ambisonics maximum energy vector weights
  
  assert(IsEqual(MaxEnergyDecWeight(0,2), 1.0));
  assert(IsEqual(MaxEnergyDecWeight(1,2), cos(PI/(6.0))));
  assert(IsEqual(MaxEnergyDecWeight(2,2), cos(2.0*PI/(6.0))));
  
  
  // Testing ambisonics maximum energy vector decoding matrix
  
  mcl::Matrix<Sample> max_re_dec =
          AmbisonicsHorizDec::MaxEnergyDec(2, loudspeaker_angles);
  mcl::Matrix<Sample> max_re_dec_cmp(5,5);
  max_re_dec_cmp.set_element(0, 0, 1.0);
  max_re_dec_cmp.set_element(1, 1, cos(PI/(6.0)));
  max_re_dec_cmp.set_element(2, 2, cos(PI/(6.0)));
  max_re_dec_cmp.set_element(3, 3, cos(2.0*PI/(6.0)));
  max_re_dec_cmp.set_element(4, 4, cos(2.0*PI/(6.0)));
  assert(IsEqual(max_re_dec, max_re_dec_cmp));
  
  
  // A complete test comparing with Matlab's tested implementation
  
  AmbisonicsMic mic_b(Point(0.0,0.0,0.0), 0.0, 0.0, 0.0, 2);
  BFormatStream* stream_b = mic_b.stream();
  
  AmbisonicsHorizDec decoder_b(order,
                               true, // energy_decoding,
                               1200, // cut_off_frequency,
                               loudspeaker_angles,
                               true, //near_field_correction,
                               2.0, //loudspeakers_distance,
                               44100, // sampling_frequency,
                               SOUND_SPEED,
                               stream_b);
  const Angle theta_b = PI/4.0;
  Signal impulse = mcl::Zeros<Sample>(4);
  impulse[0] = 0.7;
  mic_b.RecordPlaneWave(impulse, Point(cos(theta_b), sin(theta_b), 0.0));
  decoder_b.Decode();
  
  Signal output_0 = decoder_b.stream(0)->Pull(impulse.size());
  Signal output_1 = decoder_b.stream(1)->Pull(impulse.size());
  Signal output_2 = decoder_b.stream(2)->Pull(impulse.size());
  Signal output_3 = decoder_b.stream(3)->Pull(impulse.size());
  Signal output_4 = decoder_b.stream(4)->Pull(impulse.size());
  
  
  std::vector<Sample> output_0_cmp(4);
  output_0_cmp[0] = -0.261851079581371;
  output_0_cmp[1] = 0.091643993620079;
  output_0_cmp[2] = 0.077627205799285;
  output_0_cmp[3] = 0.065733018793426;
  
  Signal output_1_cmp(4);
  output_1_cmp[0] = -0.367676280830305;
  output_1_cmp[1] = 0.131355135297581;
  output_1_cmp[2] =  0.112736846228488;
  output_1_cmp[3] =  0.096652325199862;
  
  Signal output_2_cmp(4);
  output_2_cmp[0] = 0.024600010853482;
  output_2_cmp[1] =  -0.013376971132794;
  output_2_cmp[2] =  -0.013873224419426;
  output_2_cmp[3] =  -0.013799068046006;
  
  Signal output_3_cmp(4);
  output_3_cmp[0] = -0.027480352752649;
  output_3_cmp[1] =  0.012392179960282;
  output_3_cmp[2] =  0.012315424782320;
  output_3_cmp[3] =  0.011904504267406;
  
  Signal output_4_cmp(4);
  output_4_cmp[0] = 0.042910353779761;
  output_4_cmp[1] =  -0.018453086209685;
  output_4_cmp[2] =  -0.017379369470917;
  output_4_cmp[3] =  -0.016125504574487;
  
  assert(IsEqual(output_0, output_0_cmp));
  assert(IsEqual(output_1, output_1_cmp));
  assert(IsEqual(output_2, output_2_cmp));
  assert(IsEqual(output_3, output_3_cmp));
  assert(IsEqual(output_4, output_4_cmp));
  
  return true;
}
  
  
} // namespace sat
