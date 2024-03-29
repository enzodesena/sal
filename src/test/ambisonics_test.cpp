/*
 ambisonics_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "ambisonics.h"
#include "microphone.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
bool AmbisonicsMic::Test() {
  using mcl::IsEqual;
  
  ASSERT(HoaBuffer::GetChannelId(0, 0, HoaOrdering::Fuma) == 0);
  ASSERT(HoaBuffer::GetChannelId(1, 1, HoaOrdering::Fuma) == 1);
  ASSERT(HoaBuffer::GetChannelId(1, -1, HoaOrdering::Fuma) == 2);
  ASSERT(HoaBuffer::GetChannelId(1, 0, HoaOrdering::Fuma) == 3);
  ASSERT(HoaBuffer::GetChannelId(2, 0, HoaOrdering::Fuma) == 4);
  ASSERT(HoaBuffer::GetChannelId(2, 1, HoaOrdering::Fuma) == 5);
  ASSERT(HoaBuffer::GetChannelId(2, -1, HoaOrdering::Fuma) == 6);
  ASSERT(HoaBuffer::GetChannelId(2, 2, HoaOrdering::Fuma) == 7);
  ASSERT(HoaBuffer::GetChannelId(2, -2, HoaOrdering::Fuma) == 8);
  
  ASSERT(HoaBuffer::GetChannelId(0, 0, HoaOrdering::Acn) == 0);
  ASSERT(HoaBuffer::GetChannelId(1, -1, HoaOrdering::Acn) == 1);
  ASSERT(HoaBuffer::GetChannelId(1, 0, HoaOrdering::Acn) == 2);
  ASSERT(HoaBuffer::GetChannelId(1, 1, HoaOrdering::Acn) == 3);
  ASSERT(HoaBuffer::GetChannelId(2, -2, HoaOrdering::Acn) == 4);
  ASSERT(HoaBuffer::GetChannelId(2, -1, HoaOrdering::Acn) == 5);
  ASSERT(HoaBuffer::GetChannelId(2, 0, HoaOrdering::Acn) == 6);
  ASSERT(HoaBuffer::GetChannelId(2, 1, HoaOrdering::Acn) == 7);
  ASSERT(HoaBuffer::GetChannelId(2, 2, HoaOrdering::Acn) == 8);
  
  // TODO: Only testing for 2nd order
  
  // Testing Ambisonics encoding
  const Int N = 2; // Ambisonics order
  
  AmbisonicsMic mic_a(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,1,0,-PI/2.0),
                      N);
  HoaBuffer buffer_a(N, 1);
  
  Sample sample = 0.3;
  mic_a.AddPlaneWave(MonoBuffer::Unary(sample),
                     Point(1.0, 0.0, 0.0), buffer_a);
  
  ASSERT(IsEqual(buffer_a.GetSample(0, 0, 0), sample*1.000000000000000));
  ASSERT(IsEqual(buffer_a.GetSample(1, 1, 0), sample*1.414213562373095));
  ASSERT(IsEqual(buffer_a.GetSample(1, -1, 0), sample*0.0));
  ASSERT(IsEqual(buffer_a.GetSample(2, 1, 0), sample*1.414213562373095));
  ASSERT(IsEqual(buffer_a.GetSample(2, -1, 0), sample*0.0));
  mic_a.Reset();
  buffer_a.Reset();
  
  mic_a.AddPlaneWave(sample, Point(0.0, 1.0, 0.0), buffer_a);
  ASSERT(IsEqual(buffer_a.GetSample(0, 0, 0), sample*1.000000000000000));
  ASSERT(IsEqual(buffer_a.GetSample(1, -1, 0), sample*1.414213562373095));
  ASSERT(IsEqual(buffer_a.GetSample(1, 1, 0), sample*0.0));
  ASSERT(IsEqual(buffer_a.GetSample(2, 1, 0), sample*(-1.414213562373095)));
  ASSERT(IsEqual(buffer_a.GetSample(2, -1, 0), sample*0.0));
  
#if MCL_LOAD_BOOST
  std::cout<<"Running Boost-dependent Ambisonics tests"<<std::endl;
    
  // Testing Ambisonics encoding
  const Int N_b = 3; // Ambisonics order
  AmbisonicsMic mic_b(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), N_b, HoaNormalisation::N3d, HoaOrdering::Acn);
  
  HoaBuffer buffer_b(N_b, 1);
  sample = 0.5;
  mic_b.AddPlaneWave(sample, Point(1.0, 0.0, 0.0), buffer_b);
  
  // theta and phi are the spherical coordinates in the reference system of:
  // http://ambisonics.ch/standards/channels/
  // theta is the azimuth angle (angle formed with x-axis)
  // phi is the elevation angle (angle formed with the xy-plane)
  Angle theta = -PI/2.0;
  Angle phi = 0;
  
  ASSERT(IsEqual(buffer_b.GetSample(0, 0, 0), sample*1.000000000000000));
  ASSERT(IsEqual(buffer_b.GetSample(1, 1, 0), sample*sqrt(3.0)*cos(phi)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(1, 0, 0), sample*sqrt(3.0)*sin(phi)));
  ASSERT(IsEqual(buffer_b.GetSample(1, -1, 0), sample*sqrt(3.0)*cos(phi)*sin(theta)));
         
  ASSERT(IsEqual(buffer_b.GetSample(2, 2, 0), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*cos(2.0*theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, 1, 0), sample*sqrt(15.0)/2.0*sin(2.0*phi)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, 0, 0), sample*sqrt(5.0)/2.0*(3.0*pow(sin(phi),2.0)-1.0)));
  ASSERT(IsEqual(buffer_b.GetSample(2, -1, 0), sample*sqrt(15.0)/2.0*sin(2.0*phi)*sin(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, -2, 0), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*sin(2.0*theta)));
  
  ASSERT(IsEqual(buffer_b.GetSample(3, 3, 0), sample*mcl::Sqrt(35.0/8.0)*pow(cos(phi),3.0)*cos(3.0*theta)));
  ASSERT(IsEqual(buffer_b.GetSample(3, -2, 0), sample*mcl::Sqrt(105.0)/2.0*(pow(cos(phi),2.0))*sin(phi)*sin(2.0*theta)));
  ASSERT(IsEqual(buffer_b.GetSample(3, 1, 0), sample*mcl::Sqrt(21.0/8.0)*(5*pow(sin(phi),2.0)-1.0)*cos(phi)*cos(theta)));
  
  buffer_b.Reset();
  sample = 0.2;
  mic_b.AddPlaneWave(sample, Point(0.5, 0.5, 1.0/sqrt(2.0)), buffer_b);
  
  theta = PI/4.0-PI/2.0; // Here the -PI/2.0 is to consider the head rotation along the y axis.
  phi = PI/4.0;
  
  ASSERT(IsEqual(buffer_b.GetSample(0, 0, 0), sample*1.000000000000000));
  ASSERT(IsEqual(buffer_b.GetSample(1, 1, 0), sample*sqrt(3.0)*cos(phi)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(1, 0, 0), sample*sqrt(3.0)*sin(phi)));
  ASSERT(IsEqual(buffer_b.GetSample(1, -1, 0), sample*sqrt(3.0)*cos(phi)*sin(theta)));
  
  ASSERT(IsEqual(buffer_b.GetSample(2, 2, 0), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*cos(2.0*theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, 1, 0), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, 0, 0), sample*mcl::Sqrt(5.0)/2.0*(3.0*pow(sin(phi),2.0)-1.0)));
  ASSERT(IsEqual(buffer_b.GetSample(2, -1, 0), sample*mcl::Sqrt(15.0)/2.0*sin(2.0*phi)*sin(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(2, -2, 0), sample*mcl::Sqrt(15.0)/2.0*(pow(cos(phi),2.0))*sin(2.0*theta)));
  
  ASSERT(IsEqual(buffer_b.GetSample(3, -2, 0), sample*mcl::Sqrt(105.0)/2.0*(pow(cos(phi),2.0))*sin(phi)*sin(2.0*theta)));
  ASSERT(IsEqual(buffer_b.GetSample(3, 1, 0), sample*mcl::Sqrt(21.0/8.0)*cos(phi)*(5.0*pow(sin(phi),2.0)-1.0)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(3, 3, 0), sample*mcl::Sqrt(35.0/8.0)*pow(cos(phi),3.0)*cos(3.0*theta)));
  
  
  
  
  // SN3D weighting
  buffer_b.Reset();
  AmbisonicsMic mic_c(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0), N_b, HoaNormalisation::Sn3d, HoaOrdering::Acn);
  
  mic_c.AddPlaneWave(sample, Point(0.5, 0.5, 1.0/sqrt(2.0)), buffer_b);
  
  ASSERT(IsEqual(buffer_b.GetSample(0, 0, 0), sample*1.000000000000000));
  ASSERT(IsEqual(buffer_b.GetSample(2, 1, 0), sample*sqrt(15.0)/2.0/sqrt(5.0)*sin(2.0*phi)*cos(theta)));
  ASSERT(IsEqual(buffer_b.GetSample(3, 3, 0), sample*mcl::Sqrt(35.0/8.0)/sqrt(7.0)*pow(cos(phi),3.0)*cos(3.0*theta)));
  
#endif  

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
  ASSERT(IsEqual(filter_low.B(), num_lf_cmp));
  std::vector<Sample> den_lf_cmp(3);
  den_lf_cmp[0] = 1.000000000000000;
  den_lf_cmp[1] = -1.902910910316590;
  den_lf_cmp[2] = 0.905267483150478;
  ASSERT(IsEqual(filter_low.A(), den_lf_cmp));
  
  IirFilter filter_high(CrossoverFilterHigh(380, 48000));
  std::vector<Sample> num_hf_cmp(3);
  num_hf_cmp[0] = -0.952044598366767;
  num_hf_cmp[1] = 1.904089196733534;
  num_hf_cmp[2] = -0.952044598366767;
  ASSERT(IsEqual(filter_high.B(), num_hf_cmp));
  std::vector<Sample> den_hf_cmp(3);
  den_hf_cmp[0] = 1.000000000000000;
  den_hf_cmp[1] = -1.902910910316590;
  den_hf_cmp[2] = 0.905267483150478;
  ASSERT(IsEqual(filter_high.A(), den_hf_cmp));
  
  
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
  ASSERT(IsEqual(AA[0], 1.011712037681334));
  ASSERT(IsEqual(AA[1], -1.999909257970665));
  ASSERT(IsEqual(AA[2], 0.988378704348000));
  
  IirFilter filter_a = NFCFilter(2, R, Fs, c);
  
  ASSERT(IsEqual(filter_a.B(), BB, 1.0E-3));
  ASSERT(IsEqual(filter_a.A(), AA, 1.0E-3));
  
  IirFilter filter_b = NFCFilter(1, R, Fs, c);
  // TODO: I copied this values from my Matlab implementation,
  // but I am not sure whether that implementation was tested for first-order.
  ASSERT(IsEqual(filter_b.B()[0], 1.0));
  ASSERT(IsEqual(filter_b.B()[1], -1.0));
  ASSERT(IsEqual(filter_b.A()[0], 1.003888888888889));
  ASSERT(IsEqual(filter_b.A()[1], -0.996111111111111));

  IirFilter filter_c = NFCFilter(0, R, Fs, c);
  ASSERT(IsEqual(filter_c.B(), mcl::UnaryVector<Sample>(1.0)));
  ASSERT(IsEqual(filter_c.A(), mcl::UnaryVector<Sample>(1.0)));
    
  
  // Testing loudspeaker placement
  
  const Int M = 5; // num loudspeakers
  const Angle phi0 = 2.0*PI/((Angle) M);
  std::vector<Angle> loudspeaker_angles =
          mcl::Multiply(mcl::ColonOperator<Angle>(0, M-1), phi0);
  ASSERT(IsEqual(loudspeaker_angles[0], 0.0));
  ASSERT(IsEqual(loudspeaker_angles[1], 1.256637061435917));
  ASSERT(IsEqual(loudspeaker_angles[4], 4.0*1.256637061435917));
  
  
  // Testing decoding matrix
  
  mcl::Matrix<Sample> decoding_matrix =
          AmbisonicsHorizDec::ModeMatchingDec(2, loudspeaker_angles);
  ASSERT(decoding_matrix.num_columns() == 5);
  ASSERT(decoding_matrix.num_rows() == 5);
  ASSERT(IsEqual(decoding_matrix.GetElement(0, 0), 0.200000000000000));
  ASSERT(IsEqual(decoding_matrix.GetElement(1, 0), 0.200000000000000));
  ASSERT(IsEqual(decoding_matrix.GetElement(4, 0), 0.200000000000000));
  ASSERT(IsEqual(decoding_matrix.GetElement(0, 1), 0.282842712474619));
  ASSERT(IsEqual(decoding_matrix.GetElement(3, 1), -0.228824561127074));
  ASSERT(IsEqual(decoding_matrix.GetElement(0, 2), 0.0));
  ASSERT(IsEqual(decoding_matrix.GetElement(3, 2), -0.166250775110981));
  ASSERT(IsEqual(decoding_matrix.GetElement(2, 3), 0.087403204889764));
  ASSERT(IsEqual(decoding_matrix.GetElement(4, 4), -0.166250775110981));
  
  
  // Testing Ambisonics mode-matching decoding
  
  using mcl::Sum;
  using mcl::Cos;
  using mcl::ColonOperator;
  using mcl::Multiply;
  
  const Int order = 2;
  const Int MM = 5; // num loudspeakers
  const Int num_theta(100);
  std::vector<Angle> thetas = mcl::LinSpace(0.0, 2.0*PI, num_theta);
  
  AmbisonicsMic mic_a(Point(0.0,0.0,0.0), Quaternion::Identity(), order);
  
  AmbisonicsHorizDec decoder_a(order,
                               false, // energy_decoding,
                               1.0, // cut_off_frequency,
                               loudspeaker_angles,
                               false, //near_field_correction,
                               1.0, //loudspeakers_distance,
                               1.0, // sampling_frequency,
                               SOUND_SPEED);
  HoaBuffer bformat_buffer(order, 1);
  Buffer output_buffer(loudspeaker_angles.size(), 1);
  
  for (Int theta_index=0; theta_index<num_theta; ++theta_index) {
    output_buffer.Reset();
    bformat_buffer.Reset();
    mic_a.Reset();
    
    Angle theta(thetas[theta_index]);
    
    mic_a.AddPlaneWave(MonoBuffer::Unary(1.0),
                       Point(cos(theta), sin(theta), 0.0),
                       bformat_buffer);
    decoder_a.Decode(bformat_buffer, output_buffer);
    
    Sample output = output_buffer.GetSample(0, 0);
    // poletti_pan = 1.0/M*(1.0+2.0*sum(cos((1:N)*theta)));
    Sample poletti_pan = 1.0/MM*(1.0+2.0*
            Sum(Cos(Multiply(ColonOperator<Angle>(1, order), (Angle) theta))));
    ASSERT(IsEqual(output, poletti_pan));
  }
  
  
  // Testing ambisonics maximum energy vector weights
  
  ASSERT(IsEqual(MaxEnergyDecWeight(0,2), 1.0));
  ASSERT(IsEqual(MaxEnergyDecWeight(1,2), cos(PI/(6.0))));
  ASSERT(IsEqual(MaxEnergyDecWeight(2,2), cos(2.0*PI/(6.0))));
  
  
  // Testing ambisonics maximum energy vector decoding matrix
  
  mcl::Matrix<Sample> max_re_dec =
          AmbisonicsHorizDec::MaxEnergyDec(2, loudspeaker_angles);
  mcl::Matrix<Sample> max_re_dec_cmp(5,5);
  max_re_dec_cmp.SetElement(0, 0, 1.0);
  max_re_dec_cmp.SetElement(1, 1, cos(PI/(6.0)));
  max_re_dec_cmp.SetElement(2, 2, cos(PI/(6.0)));
  max_re_dec_cmp.SetElement(3, 3, cos(2.0*PI/(6.0)));
  max_re_dec_cmp.SetElement(4, 4, cos(2.0*PI/(6.0)));
  ASSERT(IsEqual(max_re_dec, max_re_dec_cmp));
  
  
  // A complete test comparing with Matlab's tested implementation
  
  AmbisonicsMic mic_b(Point(0.0,0.0,0.0), Quaternion::Identity(), 2);
  const Int num_samples = 4;
  HoaBuffer stream_b(order, num_samples);
  Buffer output_b(loudspeaker_angles.size(), num_samples);
  
  AmbisonicsHorizDec decoder_b(order,
                               true, // energy_decoding,
                               1200, // cut_off_frequency,
                               loudspeaker_angles,
                               true, //near_field_correction,
                               2.0, //loudspeakers_distance,
                               44100, // sampling_frequency,
                               SOUND_SPEED);
  const Angle theta_b = PI/4.0;
  MonoBuffer impulse(num_samples);
  impulse.SetSample(0, 0.7);
  mic_b.AddPlaneWave(impulse, Point(cos(theta_b), sin(theta_b), 0.0), stream_b);
  decoder_b.Decode(stream_b, output_b);
  
  
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
  
  ASSERT(IsEqual(output_b.GetReadPointer(0), output_0_cmp));
  ASSERT(IsEqual(output_b.GetReadPointer(1), output_1_cmp));
  ASSERT(IsEqual(output_b.GetReadPointer(2), output_2_cmp));
  ASSERT(IsEqual(output_b.GetReadPointer(3), output_3_cmp));
  ASSERT(IsEqual(output_b.GetReadPointer(4), output_4_cmp));
  
  return true;
}
  
  
} // namespace sal
