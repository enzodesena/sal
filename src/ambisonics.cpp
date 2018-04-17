/*
 ambisonics.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This include contains definition of classes and functions
 related to higer-order ambisonics.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "ambisonics.h"
#include "matrixop.h"

using mcl::Point;
using mcl::Multiply;

namespace sal {

void AmbisonicsMic::AddPlaneWaveRelative(const Sample* input_data,
                                         const Int num_samples,
                                         const mcl::Point& point,
                                         const Int wave_id,
                                         Buffer& output_buffer) noexcept {
  BFormatBuffer& bformat_buffer = dynamic_cast<BFormatBuffer&>(output_buffer);
  
  // Precompute for performance gain
  const Angle phi = point.phi();
  const Sample sqrt_2 = mcl::Sqrt(2.0);
  
  switch (convention_) {
    case sqrt2: {
      
      // Zero-th component
      bformat_buffer.AddSamples(0, 0, 0, num_samples, input_data);
      
      for (Int i=1; i<=order_; ++i) {
        // TODO: add 3D components
        Sample temp_data_sin[num_samples];
        mcl::Multiply(input_data, num_samples, sqrt_2*sin(((Angle) i)*phi), temp_data_sin);
        
        Sample temp_data_cos[num_samples];
        mcl::Multiply(input_data, num_samples, sqrt_2*cos(((Angle) i)*phi), temp_data_cos);
        bformat_buffer.AddSamples(i, 1, 0, num_samples, temp_data_cos);
        bformat_buffer.AddSamples(i, -1, 0, num_samples, temp_data_sin);
      }
      break;
    }
      
#ifdef MCL_LOAD_BOOST
    case N3D: {
      // Precompute for performance gain
      const Angle theta = point.theta();
      const Sample sqrt_4pi = mcl::Sqrt(4.0*PI);
      
      // Zero-th component
      stream_.Add(0, 0, 1.0*sample);
      
      for (Int degree_n=1; degree_n<=order_; ++degree_n) {
        for (Int order_m=0; order_m<=degree_n; ++order_m) {
          mcl::Complex spherical_harmonic =
              mcl::Pow(-1.0, (mcl::Real) order_m) *
              (order_m==0 ? 1.0 : sqrt_2) *
              sqrt_4pi *
              mcl::SphericalHarmonic(degree_n, mcl::Abs(order_m), theta, phi);
          
          stream_.Add(degree_n, -order_m,
                      Multiply<sal::Sample>(signal, mcl::ImagPart(spherical_harmonic)));
          stream_.Add(degree_n, order_m,
                      Multiply<sal::Sample>(signal, mcl::RealPart(spherical_harmonic)));
          // The order of the two statements above is not random, since the case
          // order_m equals zero should give back the non-zero cosine term,
          // i.e. the one corresponding to the real part.
        }
      }
      break;
    }
#endif
      
    default: {
      ASSERT(false);
      break;
    }
  }
}

std::vector<mcl::Real> AmbisonicsMic::HorizontalEncoding(Int order,
                                                         Angle theta) {
  std::vector<mcl::Real> output;
  output.reserve(2*order+1);
  output.push_back(1.0);
  for (Int i=1; i<=order; ++i) {
    output.push_back(sqrt(2.0)*cos(((Angle) i)*theta));
    output.push_back(sqrt(2.0)*sin(((Angle) i)*theta));
  }
  return output;
}
  
AmbisonicsHorizDec::AmbisonicsHorizDec(const Int order,
                                       const bool energy_decoding,
                                       const Time cut_off_frequency,
                                       const std::vector<Angle>& loudspeaker_angles,
                                       const bool near_field_correction,
                                       const Length loudspeakers_distance,
                                       const Time sampling_frequency,
                                       const Speed sound_speed) :
          energy_decoding_(energy_decoding),
          loudspeaker_angles_(loudspeaker_angles),
          num_loudspeakers_(loudspeaker_angles.size()),
          near_field_correction_(near_field_correction),
          loudspeakers_distance_(loudspeakers_distance),
          sampling_frequency_(sampling_frequency),
          order_(order),
          mode_matching_matrix_(mcl::Matrix<Sample>(2*order+1,
                                                    loudspeaker_angles.size())),
          max_energy_matrix_(mcl::Matrix<Sample>(2*order+1,
                                                 loudspeaker_angles.size())) {
  
  using mcl::IirFilter;
            
  if (near_field_correction_) {
    // One filter per order per loudspeaker (inner is per loudspeaker, so that
    // they are all equal).
    nfc_filters_.reserve(2*order+1);
    nfc_filters_.push_back(NFCFilter(0, loudspeakers_distance_,
                                     sampling_frequency_,
                                     sound_speed));
    for (Int i=1; i<=order; ++i) {
      // Instanciating two filters because order higher than two have
      // both degrees +1 and -1
      nfc_filters_.push_back(NFCFilter(i, loudspeakers_distance_,
                                       sampling_frequency_,
                                       sound_speed));
      nfc_filters_.push_back(NFCFilter(i, loudspeakers_distance_,
                                       sampling_frequency_,
                                       sound_speed));
    }
    ASSERT(nfc_filters_.size() == 2*order+1);
  }
  
  if (energy_decoding_) {
    IirFilter cx_high = CrossoverFilterHigh(cut_off_frequency,
                                            sampling_frequency);
    IirFilter cx_low = CrossoverFilterLow(cut_off_frequency,
                                          sampling_frequency);
    // One filter per loudspeaker
    crossover_filters_high_ = std::vector<IirFilter>(num_loudspeakers_,
                                                     cx_high);
    crossover_filters_low_ = std::vector<IirFilter>(num_loudspeakers_,
                                                    cx_low);
  }
  
  mode_matching_matrix_ = ModeMatchingDec(order_, loudspeaker_angles_);
  max_energy_matrix_ = MaxEnergyDec(order_, loudspeaker_angles_);
}
  
  
mcl::Matrix<Sample> AmbisonicsHorizDec::ModeMatchingDec(Int order,
                const std::vector<Angle>& loudspeaker_angles) {
  using mcl::Matrix;
  using mcl::Multiply;
  const Int num_loudspeakers = loudspeaker_angles.size();
  
  Matrix<Sample> temp(2*order+1, num_loudspeakers);
  
  for (Int i=0; i<num_loudspeakers; ++i) {
    temp.set_column(i, AmbisonicsMic::HorizontalEncoding(order,
                                                         loudspeaker_angles[i]));
  }
  // TODO: implement for non-regular loudspeaker arrays.
  // M_d = temp'*(temp*temp')^(-1);
  // Since temp is unitary (except for a value of 2*order+1),we can compute the
  // inverse by simple transposition.
  //return Multiply(Transpose(temp), 1.0/((Sample) 2*order+1));
  return Multiply(Transpose(temp), (Sample) 1.0/((Sample) 2*order+1));
}
  
mcl::Matrix<Sample> AmbisonicsHorizDec::MaxEnergyDec(Int order,
                 const std::vector<Angle>& loudspeaker_angles) {
  // TODO: Implement for non-regular loudspeaker arrays.
  mcl::Matrix<Sample> decoding_matrix(2*order+1, 2*order+1);
  decoding_matrix.set_element(0, 0, MaxEnergyDecWeight(0, order));
  Int k=1;
  for (Int i=1; i<=order; ++i) {
    decoding_matrix.set_element(k, k, MaxEnergyDecWeight(i, order));
    k++;
    decoding_matrix.set_element(k, k, MaxEnergyDecWeight(i, order));
    k++;
  }
  return decoding_matrix;
}
  
std::vector<Sample>
AmbisonicsHorizDec::GetFrame(const Int order, const Int sample_id,
                             const BFormatBuffer& buffer) {
  ASSERT(order>=0);
  ASSERT(sample_id>=0 & sample_id<buffer.num_samples());
  
  std::vector<Sample> output;
  output.reserve(2*order+1);
  output.push_back(buffer.GetSample(0, 0, sample_id));
  for (Int i=1; i<=order; ++i) {
    output.push_back(buffer.GetSample(i, +1, sample_id));
    output.push_back(buffer.GetSample(i, -1, sample_id));
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
void AmbisonicsHorizDec::Decode(const Buffer& input_buffer,
                                Buffer& output_buffer) {
  ASSERT(input_buffer.num_samples() == output_buffer.num_samples());
  const BFormatBuffer& input_bformat_buffer =
      dynamic_cast<const BFormatBuffer&>(input_buffer);
  MultichannelBuffer& output_multi_buffer =
      dynamic_cast<MultichannelBuffer&>(output_buffer);
  
  // Cache for speed
  for (Int sample_id = 0; sample_id<input_buffer.num_samples(); ++sample_id) {
    std::vector<Sample> bformat_frame = GetFrame(order_, sample_id,
                                                 input_bformat_buffer);
    
    // Near-field correcting
    if (near_field_correction_) {
      for (Int i=0; i<(2*order_+1); ++i) {
        bformat_frame[i] = nfc_filters_[i].Filter(bformat_frame[i]);
      }
    }
    
    // Mode matching decoding
    //  // Ambisonics decoding (mode-matching)
    //  M_d = amb_decoding(N, loudspeaker_angles);
    //  G_format_low = M_d*amb_nfc_filter(B_format, loudspeakers_distance, Fs, c);
    std::vector<Sample> output = mcl::Multiply(mode_matching_matrix_,
                                               bformat_frame);
    if (energy_decoding_) {
      // Maximum energy decoding at high frequency
      //  // Ambisonics decoding (max r_e)
      //  g = amb_re_weights_reg(N)*sqrt(5/3);
      //  display('energy');
      //  G = amb_re_weights_matrix(g);
      //  G_format_high = M_d*G*B_format;
      std::vector<Sample> output_high =
                mcl::Multiply(mode_matching_matrix_,
                              mcl::Multiply(max_energy_matrix_, bformat_frame));
      
      // Cross-fading high and low
      //  // Generate output
      //  [B_LF, A_LF, B_HF, A_HF] = amb_crossover_filter(cut_off_frequency, Fs);
      //  out_low = filter(B_LF, A_LF, G_format_low, [], 2);
      //  out_high = filter(B_HF, A_HF, G_format_high, [], 2);
      //  out_nfc = out_low + out_high;
      for (Int i=0; i<num_loudspeakers_; ++i) {
        output[i] = crossover_filters_low_[i].Filter(output[i]) +
                    crossover_filters_high_[i].Filter(output_high[i]);
      }
    }
    
    // Push into each loudspeaker stream
    for (Int i=0; i<num_loudspeakers_; ++i) {
      output_multi_buffer.SetSample(i, sample_id, output[i]);
    }
  }
}
  
    
                                    
mcl::IirFilter AmbisonicsHorizDec::CrossoverFilterLow(
        const Time cut_off_frequency, const Time sampling_frequency) {
  Sample k = tan(PI*cut_off_frequency/sampling_frequency);
  
  std::vector<Sample> b_lf(3);
  // b0_lf = k^2/(k^2+2*k+1);
  b_lf[0] = pow(k,2.0)/(pow(k,2.0)+2.0*k+1.0);
  // b1_lf = 2*b0_lf;
  b_lf[1] = 2.0 * b_lf[0];
  // b2_lf = b0_lf;
  b_lf[2] = b_lf[0];
  
  std::vector<Sample> a(3);
  a[0] = 1.0;
  // a1 = 2*(k^2-1)/(k^2+2*k+1);
  a[1] = 2.*(pow(k,2.0)-1.0)/(pow(k,2.0)+2.0*k+1.0);
  // a2 = (k^2-2*k+1)/(k^2+2*k+1);
  a[2] = (pow(k,2.0)-2.0*k+1.0)/(pow(k,2.0)+2.0*k+1.0);
  
  return mcl::IirFilter(b_lf, a);
}

mcl::IirFilter AmbisonicsHorizDec::CrossoverFilterHigh(
        const Time cut_off_frequency, const Time sampling_frequency) {
  Sample k = tan(PI*cut_off_frequency/sampling_frequency);
  
  std::vector<Sample> b_hf(3);
  // b0_hf = 1/(k^2+2*k+1);
  b_hf[0] = 1.0/(pow(k,2.0)+2.0*k+1.0);
  // b1_hf = -2*b0_hf;
  b_hf[1] = -2.0*b_hf[0];
  b_hf[2] = b_hf[0];
  
  // I add a minus here so that the output of the two filter will need to be
  // added, rather than subtracted as described in the paper.
  b_hf = mcl::Multiply(b_hf, (Sample) -1.0);
  
  // The denominator of the high frequency filter is the same as the low one.
  mcl::IirFilter filter_low(CrossoverFilterLow(cut_off_frequency,
                                               sampling_frequency));
  
  
  return mcl::IirFilter(b_hf, filter_low.A());
}
  

// Near-field correction filters as described in
// "Spatial Sound Encoding Including Near Field Effect: Introducing
// Length Coding Filters and a Viable, New Ambisonic Format" by
// J. Daniel, AES 23rd, Int. Conf., 2003.
mcl::IirFilter AmbisonicsHorizDec::NFCFilter(const Int order,
                                             const Length loudspeaker_distance,
                                             const Time sampling_frequency,
                                             const Length sound_speed) {
  // TODO: implement for orders higher than 6
  if (order > 6) { ASSERT(false); }
  using mcl::Real;
  using mcl::Poly;
  using mcl::Ones;
  using mcl::Complex;
  using mcl::Conj;
  using mcl::Prod;
  using mcl::Multiply;
  using mcl::RealPart;
  
  std::vector<Complex> X_Mq;
  switch(order) {
    case 0:
      X_Mq = std::vector<Complex>(0);
      break;
    case 1:
      X_Mq = std::vector<Complex>(1);
      X_Mq[0] = Complex(-2.0, 0.0);
      break;
    case 2:
      X_Mq = std::vector<Complex>(2);
      X_Mq[0] = Complex(-3.0000, 1.7321);
      X_Mq[1] = Conj(X_Mq[0]);
      break;
    case 3:
      X_Mq = std::vector<Complex>(3);
      X_Mq[0] = Complex(-3.6778, 3.5088);
      X_Mq[1] = Conj(X_Mq[0]);
      X_Mq[2] = Complex(-4.6444, 0.0);
      break;
    case 4:
      X_Mq = std::vector<Complex>(4);
      X_Mq[0] = Complex(-4.2076, 5.3148);
      X_Mq[1] = Conj(X_Mq[0]);
      X_Mq[2] = Complex(-5.7924, 1.7345);
      X_Mq[3] = Conj(X_Mq[2]);
      break;
    case 5:
      X_Mq = std::vector<Complex>(5);
      X_Mq[0] = Complex(-4.6493, 7.1420);
      X_Mq[1] = Conj(X_Mq[0]);
      X_Mq[2] = Complex(-6.7039, 3.4853);
      X_Mq[3] = Conj(X_Mq[2]);
      X_Mq[4] = Complex(-7.2935, 0.0);
      break;
    case 6:
      X_Mq = std::vector<Complex>(6);
      X_Mq[0] = Complex(-5.0319, 8.9853);
      X_Mq[1] = Conj(X_Mq[0]);
      X_Mq[2] = Complex(-7.4714, 5.2525);
      X_Mq[3] = Conj(X_Mq[2]);
      X_Mq[4] = Complex(-8.4967, 1.7350);
      X_Mq[5] = Conj(X_Mq[4]);
      break;
  }
  
  Real a = 4.0*sampling_frequency*loudspeaker_distance/sound_speed;
  
  // I need to implement A(poly((1+X_mq/a)./(1-X_mq/a))*prod(1-X_mq./a);
  const Int num_samples = X_Mq.size();
  std::vector<Complex> temp_1(num_samples); // (1+X_mq/a)./(1-X_mq/a))
  std::vector<Complex> temp_2(num_samples); // 1-X_mq./a
  for (Int i=0; i<num_samples; ++i) {
    temp_1[i] = (Complex(1.0, 0.0)+X_Mq[i]/Complex(a, 0.0))/
                (Complex(1.0,0.0)-X_Mq[i]/Complex(a, 0.0));
    temp_2[i] = (Complex(1.0,0.0)-X_Mq[i]/Complex(a, 0.0));
  }
  
  std::vector<Real> B = mcl::RealPart(mcl::Poly(mcl::Ones(order)));
  std::vector<Real> A = RealPart(Multiply(Poly(temp_1), Prod(temp_2)));
  
  
  return mcl::IirFilter(B, A);
}

} // namespace sal
