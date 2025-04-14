/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <vector>

#include "butter.h"
#include "comparisonop.h"
#include "firfilter.h"
#include "iirfilter.h"
#include "maxgradientfilter.h"
#include "dsptypes.h"
#include "randomop.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

bool IirFilter::Test() {
  IirFilter filter_a = IdenticalFilter();
  ASSERT(IsEqual(filter_a.ProcessSample(1.2), 1.2));

  IirFilter filter_b = GainFilter(0.76);
  ASSERT(IsEqual(filter_b.ProcessSample(1.2), 0.912));

  std::vector<Real> B;
  std::vector<Real> A;
  B.push_back(0.76);
  B.push_back(-3.06);
  B.push_back(1.76);
  B.push_back(1.76);
  A.push_back(1.0);
  A.push_back(-0.5);
  A.push_back(0.23);
  A.push_back(0.75);

  IirFilter filter_c(B, A);

  ASSERT(IsEqual(filter_c.ProcessSample(1.2), 0.912));
  ASSERT(IsEqual(filter_c.ProcessSample(0.2), -3.064));
  ASSERT(IsEqual(filter_c.ProcessSample(0.5), 0.13824));
  ASSERT(IsEqual(filter_c.ProcessSample(-1.2), 0.11184));
  ASSERT(
      IsEqual(filter_c.ProcessSample(0.0), 7.226124799999999999999999999999));
  ASSERT(
      IsEqual(filter_c.ProcessSample(0.0), 2.251659199999999999999999999999));

  IirFilter filter_d = WallFilter(kCarpetPile, 44100);
  ASSERT(IsEqual(filter_d.ProcessSample(1.0), 0.562666833756030));
  ASSERT(IsEqual(filter_d.ProcessSample(0.5), 0.315580130841020));

  IirFilter filter_e = WallFilter(kCarpetCotton, 44100);
  ASSERT(IsEqual(filter_e.ProcessSample(1.0), 0.687580695329600));
  ASSERT(IsEqual(filter_e.ProcessSample(0.5), 0.322032066558733));

  IirFilter filter_f = WallFilter(kWallBricks, 44100);
  ASSERT(IsEqual(filter_f.ProcessSample(1.0), 0.978495798553620));
  ASSERT(IsEqual(filter_f.ProcessSample(0.5), 0.490594444043047));

  IirFilter filter_g = WallFilter(kCeilingTile, 44100);
  ASSERT(IsEqual(filter_g.ProcessSample(1.0), 0.168413736374283));
  ASSERT(IsEqual(filter_g.ProcessSample(0.5), 0.151668254946940));

  std::vector<Real> input_a(4);
  input_a[0] = 0.6;
  input_a[1] = -3.5;
  input_a[2] = 5.6;
  input_a[3] = 2.3;

  // Testing pinkifier filter
  IirFilter pinkifier = PinkifierFilter();
  std::vector<Real> output_e(input_a.size());
  pinkifier.ProcessBlock(input_a, output_e);
  std::vector<Real> output_e_cmp = {0.600000000000000, -3.152233220000000,
                                    3.815449359516707, 4.322130531286722};
  ASSERT(IsEqual(output_e, output_e_cmp));

  std::vector<Real> B_d(3);
  B_d[0] = 1.0;
  B_d[1] = -2.0;
  B_d[2] = 1.0;
  std::vector<Real> A_d(3);
  A_d[0] = 1.005844676087000;
  A_d[1] = -1.999977314492666;
  A_d[2] = 0.994178009420333;
  IirFilter filter_l(B_d, A_d);
  ASSERT(IsEqual(B_d, filter_l.numerator_coeffs()));
  ASSERT(IsEqual(A_d, filter_l.denominator_coeffs()));

  std::vector<Real> signal_d = dsp::Zeros<Real>(4);
  signal_d[0] = 0.989949493661167;
  std::vector<Real> signal_d_out_cmp(4);
  signal_d_out_cmp[0] = 0.984197179938686;
  signal_d_out_cmp[1] = -0.011459974617699;
  signal_d_out_cmp[2] = -0.011370929428126;
  signal_d_out_cmp[3] = -0.011282404149780;
  std::vector<Real> output_d(signal_d.size());
  filter_l.IirFilter::ProcessBlock(signal_d, output_d);
  ASSERT(IsEqual(output_d, signal_d_out_cmp));

  // Testing Reset()
  filter_l.ResetState();
  ASSERT(IsEqual(filter_l.ProcessSample(0.0), 0.0));

  std::vector<Real> impulse_resp_2(3);
  impulse_resp_2[0] = 0.2;
  impulse_resp_2[1] = -0.1;
  impulse_resp_2[2] = 2.5;

  FirFilter filter_m(impulse_resp_2);
  ASSERT(!IsEqual(filter_m.ProcessSample(1.0), 0.0));
  filter_m.ResetState();
  ASSERT(IsEqual(filter_m.ProcessSample(0.0), 0.0));

  // Testing butterworth filter
  IirFilter butter_a = Butter(3, 0.2, 0.45);
  std::vector<Real> butter_a_num_cmp = dsp::Zeros<Real>(7);
  butter_a_num_cmp[0] = 0.031689343849711;
  butter_a_num_cmp[2] = -0.095068031549133;
  butter_a_num_cmp[4] = 0.095068031549133;
  butter_a_num_cmp[6] = -0.031689343849711;

  std::vector<Real> butter_a_den_cmp(7);
  butter_a_den_cmp[0] = 1.000000000000000;
  butter_a_den_cmp[1] = -2.521796622886441;
  butter_a_den_cmp[2] = 3.643067063269880;
  butter_a_den_cmp[3] = -3.325285581665978;
  butter_a_den_cmp[4] = 2.149206132889376;
  butter_a_den_cmp[5] = -0.850496842492471;
  butter_a_den_cmp[6] = 0.197825187264320;

  ASSERT(IsEqual(butter_a.numerator_coeffs(), butter_a_num_cmp));
  ASSERT(IsEqual(butter_a.denominator_coeffs(), butter_a_den_cmp));

  IirFilter butter_b = Butter(2, 0.12, 0.79);
  std::vector<Real> butter_b_num_cmp = dsp::Zeros<Real>(5);
  butter_b_num_cmp[0] = 0.469043625796947;
  butter_b_num_cmp[2] = -0.938087251593893;
  butter_b_num_cmp[4] = 0.469043625796947;

  std::vector<Real> butter_b_den_cmp(5);
  butter_b_den_cmp[0] = 1.000000000000000;
  butter_b_den_cmp[1] = -0.388787442245741;
  butter_b_den_cmp[2] = -0.583519141064213;
  butter_b_den_cmp[3] = 0.041607774454425;
  butter_b_den_cmp[4] = 0.243288940651677;

  ASSERT(IsEqual(butter_b.numerator_coeffs(), butter_b_num_cmp));
  ASSERT(IsEqual(butter_b.denominator_coeffs(), butter_b_den_cmp));

  IirFilter filter_i;
  ASSERT(IsEqual(filter_i.ProcessSample(1.2), 1.2));
  ASSERT(IsEqual(filter_i.ProcessSample(-0.2), -0.2));

  // Testing octave filter
  IirFilter octave_a = OctaveFilter(3, 4000.0, 44100.0);
  std::vector<Real> octave_a_num_cmp = dsp::Zeros<Real>(7);
  octave_a_num_cmp[0] = 0.005020133201471;
  octave_a_num_cmp[2] = -0.015060399604412;
  octave_a_num_cmp[4] = 0.015060399604412;
  octave_a_num_cmp[6] = -0.005020133201471;

  std::vector<Real> octave_a_den_cmp(7);
  octave_a_den_cmp[0] = 1.000000000000000;
  octave_a_den_cmp[1] = -4.397041740651781;
  octave_a_den_cmp[2] = 8.729527405676500;
  octave_a_den_cmp[3] = -9.889119467962011;
  octave_a_den_cmp[4] = 6.737413381809715;
  octave_a_den_cmp[5] = -2.619423015108258;
  octave_a_den_cmp[6] = 0.460896610043675;

  ASSERT(IsEqual(octave_a.numerator_coeffs(), octave_a_num_cmp));
  ASSERT(IsEqual(octave_a.denominator_coeffs(), octave_a_den_cmp));

  // Testing series of IIR filters
  octave_a.ResetState();
  butter_b.ResetState();
  IirFilter combined_a = SeriesFilter(octave_a, butter_b);
  std::vector<Real> output_combined(input_a.size());
  combined_a.ProcessBlock(input_a, output_combined);
  std::vector<Real> output_series(input_a.size());
  octave_a.ProcessBlock(input_a, output_series);
  butter_b.ProcessBlock(output_series, output_series);
  assert(IsEqual(output_series, output_combined));

  octave_a.ResetState();
  butter_a.ResetState();
  butter_b.ResetState();
  std::vector<IirFilter> filters;
  filters.push_back(octave_a);
  filters.push_back(butter_b);
  filters.push_back(butter_a);
  IirFilter combined_b = SeriesFilter(filters);
  octave_a.ProcessBlock(input_a, output_series);
  butter_b.ProcessBlock(output_series, output_series);
  butter_a.ProcessBlock(output_series, output_series);
  combined_b.ProcessBlock(input_a, output_combined);
  assert(IsEqual(output_series, output_combined));

  // Testing iir filter bank
  octave_a.ResetState();
  IirFilterBank octave_bank_a = OctaveFilterBank(3, 1, 4000.0, 44100.0);
  std::vector<Real> output_octave_bank_a(octave_bank_a.num_filters());
  octave_bank_a.ProcessSample(1.25, output_octave_bank_a);
  ASSERT(IsEqual(output_octave_bank_a[0], octave_a.ProcessSample(1.25)));
  octave_bank_a.ProcessSample(0.25, output_octave_bank_a);
  ASSERT(IsEqual(output_octave_bank_a[0], octave_a.ProcessSample(0.25)));
  octave_bank_a.ProcessSample(5.0, output_octave_bank_a);
  ASSERT(IsEqual(output_octave_bank_a[0], octave_a.ProcessSample(5.0)));
  ASSERT(octave_bank_a.num_filters() == 1);

  IirFilterBank octave_bank_b = OctaveFilterBank(3, 2, 2000.0, 44100.0);
  octave_a.ResetState();
  std::vector<Real> octave_bank_b_output(octave_bank_b.num_filters());
  octave_bank_b.ProcessSample(1.25, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(1.25)));
  octave_bank_b.ProcessSample(0.25, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(0.25)));
  octave_bank_b.ProcessSample(5.0, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(5.0)));
  ASSERT(octave_bank_b.num_filters() == 2);

  octave_a.ResetState();
  octave_bank_b.ResetState();
  octave_bank_b.ProcessSample(1.25, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(1.25)));
  octave_bank_b.ProcessSample(0.25, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(0.25)));
  octave_bank_b.ProcessSample(5.0, octave_bank_b_output);
  ASSERT(IsEqual(octave_bank_b_output[1], octave_a.ProcessSample(5.0)));

  return true;
}

bool FirFilter::Test() {
  using sal::dsp::IsEqual;

  std::vector<Real> ir = {0.1, 0.2, 0.3};
  FirFilter filter_lasplita(ir);
  std::vector<Real> input = {1, 2, 3, 4, 5, 6, 7};
  Real output_lasplita_a[3];
  std::vector<Real> cmp_lasplita_a = {0.1, 0.4, 1.0};
#ifdef SAL_DSP_APPLE_ACCELERATE
  filter_lasplita.ProcessBlockAppleDsp(std::span(input.begin(), 3),
                                       output_lasplita_a);
  ASSERT(IsEqual(cmp_lasplita_a, output_lasplita_a));
#endif

  Real output_lasplita_b[4];
  std::vector<Real> cmp_lasplita_b = {1.6, 2.2, 2.8, 3.4};
#ifdef SAL_DSP_APPLE_ACCELERATE
  filter_lasplita.ProcessBlockAppleDsp(std::span(input.begin() + 3, 4),
                                       output_lasplita_b);
  ASSERT(IsEqual(cmp_lasplita_b, output_lasplita_b));
#endif

  filter_lasplita.ResetState();
  filter_lasplita.ProcessBlock(std::span(input.begin(), 3), output_lasplita_a);
  ASSERT(IsEqual(cmp_lasplita_a, output_lasplita_a));
  filter_lasplita.ProcessBlock(std::span(input.begin() + 3, 4),
                               output_lasplita_b);
  ASSERT(IsEqual(cmp_lasplita_b, output_lasplita_b));

  std::vector<Real> impulse_resp(3);
  impulse_resp[0] = 0.2;
  impulse_resp[1] = -0.1;
  impulse_resp[2] = 2.5;

  std::vector<Real> impulse(3);
  impulse[0] = 1.0;
  impulse[1] = 0.0;
  impulse[2] = 0.0;

  FirFilter filter_a;
  filter_a.SetImpulseResponse(impulse_resp);
  std::vector<Real> output_aa_cmp(impulse.size());
  filter_a.ProcessBlock(impulse, output_aa_cmp);
  ASSERT(IsEqual(output_aa_cmp, impulse_resp));

  FirFilter filter_b(impulse_resp);
  std::vector<Real> input_a(4);
  input_a[0] = 0.6;
  input_a[1] = -3.5;
  input_a[2] = 5.6;
  input_a[3] = 2.3;

  std::vector<Real> output_a_cmp(4);
  output_a_cmp[0] = 0.1200;
  output_a_cmp[1] = -0.7600;
  output_a_cmp[2] = 2.9700;
  output_a_cmp[3] = -8.8500;
  std::vector<Real> output_a(input_a.size());
  filter_b.ProcessBlock(input_a, output_a);
  ASSERT(IsEqual(output_a, output_a_cmp));

  FirFilter filter_c(impulse_resp);
  ASSERT(IsEqual(filter_c.ProcessSample(0.6), 0.1200));
  ASSERT(IsEqual(filter_c.ProcessSample(-3.5), -0.7600));

  FirFilter filter_ca(impulse_resp);
#ifdef SAL_DSP_APPLE_ACCELERATE
  ASSERT(IsEqual(filter_ca.ProcessSampleAppleDsp(0.6), 0.1200));
  ASSERT(IsEqual(filter_ca.ProcessSampleAppleDsp(-3.5), -0.7600));
#endif

  FirFilter filter_cb(impulse_resp);
  ASSERT(IsEqual(filter_cb.ProcessSampleStraight(0.6), 0.1200));
  ASSERT(IsEqual(filter_cb.ProcessSampleStraight(-3.5), -0.7600));

  FirFilter filter_cd(impulse_resp);
#ifdef SAL_DSP_APPLE_ACCELERATE
  ASSERT(IsEqual(filter_cd.ProcessSampleAppleDsp(0.6), 0.1200));
  ASSERT(IsEqual(filter_cd.ProcessSampleStraight(-3.5), -0.7600));
#endif

  FirFilter filter_ce(impulse_resp);
  ASSERT(IsEqual(filter_ce.ProcessSampleStraight(0.6), 0.1200));
#ifdef SAL_DSP_APPLE_ACCELERATE
  ASSERT(IsEqual(filter_ce.ProcessSampleAppleDsp(-3.5), -0.7600));
#endif

  // Testing copy constructor
  FirFilter filter_d = filter_c;
  ASSERT(IsEqual(filter_d.ProcessSample(5.6), 2.9700));
  ASSERT(IsEqual(filter_d.ProcessSample(2.3), -8.8500));

  ASSERT(IsEqual(filter_c.ProcessSample(5.6), 2.9700));
  ASSERT(IsEqual(filter_c.ProcessSample(2.3), -8.8500));

  FirFilter filter_i;
  ASSERT(IsEqual(filter_i.ProcessSample(1.2), 1.2));
  ASSERT(IsEqual(filter_i.ProcessSample(-0.2), -0.2));

  std::vector<Real> impulse_resp_b = {0.1,  0.3, -0.2, 1.2,
                                      -4.5, 0.0, -2.1, -1.2};
  FirFilter filter_l(impulse_resp_b);
  std::vector<Real> input_b = {0.3377, 0.9001, 0.3692, 0.1112, 0.7803, 0.3897,
                               0.2417, 0.4039, 0.0965, 0.1320, 0.9421, 0.9561};
  std::vector<Real> output_b_cmp = {
      0.033770000000000,  0.191320000000000,  0.239410000000000,
      0.347100000000000,  -0.401980000000000, -3.356590000000000,
      -2.252110000000000, -1.824530000000000, -4.816670000000000,
      -2.178800000000000, -2.260530000000000, -3.104640000000000};
  std::vector<Real> output_b(input_b.size());
  filter_l.ProcessBlock(input_b, output_b);
  ASSERT(IsEqual(output_b_cmp, output_b));

  filter_l.ResetState();
  for (Int i = 0; i < (Int)input_b.size(); ++i) {
    ASSERT(dsp::IsEqual(filter_l.ProcessSample(input_b[i]), output_b_cmp[i]));
  }

  // #ifdef SAL_DSP_APPLE_ACCELERATE
  //   FirFilter filter_la(impulse_resp_b);
  //   Real cmp_la[input_b.size()];
  //   filter_la.ProcessBlockAppleDsp(input_b, cmp_la);
  //   ASSERT(IsEqual(output_b_cmp, cmp_la));
  // #endif

  FirFilter filter_lasplit(impulse_resp_b);
  Real cmp_lasplit_a[4];
  Real cmp_lasplit_b[8];
#ifdef SAL_DSP_APPLE_ACCELERATE
  filter_lasplit.ProcessBlockAppleDsp(std::span(input_b.begin(), 4),
                                      cmp_lasplit_a);
  ASSERT(IsEqual(&output_b_cmp.data()[0], cmp_lasplit_a, 4));
  filter_lasplit.ProcessBlockAppleDsp(std::span(input_b.begin() + 4, 8),
                                      cmp_lasplit_b);
  ASSERT(IsEqual(&output_b_cmp.data()[4], cmp_lasplit_b, 8));
#endif

  FirFilter filter_lb(impulse_resp_b);
  std::vector<Real> cmp_lb(input_b.size(), 0.0);
  filter_lb.ProcessBlockSerial(input_b, cmp_lb);
  ASSERT(IsEqual(output_b_cmp, cmp_lb));

  std::vector<Real> input_c = {0.8147, 0.9058, 0.1270, 0.9134, 0.6324,
                               0.0975, 0.2785, 0.5469, 0.9575, 0.9649,
                               0.1576, 0.9706, 0.9572, 0.4854, 0.8003,
                               0.1419, 0.4218, 0.9157, 0.7922, 0.9595};
  std::vector<Real> impulse_resp_c = {
      0.6948, 0.3171, 0.9502, 0.0344, 0.4387, 0.3816, 0.7655, 0.7952,
      0.1869, 0.4898, 0.4456, 0.6463, 0.7094, 0.7547, 0.2760, 0.6797};
  FirFilter filter_m(impulse_resp_c);
  std::vector<Real> output_c_cmp = {
      0.566053560000000, 0.887691210000000, 1.149596720000000,
      1.563618860000000, 1.238274470000000, 1.848822500000000,
      1.881767519999999, 2.373108650000000, 2.702443100000000,
      3.155909820000000, 3.544349419999999, 3.760939330000000,
      3.860796740000000, 5.071760400000001, 5.228588220000000,
      5.070855620000001, 5.216075850000000, 4.336750739999999,
      5.636061180000000, 5.665156830000000};
  std::vector<Real> output_c(input_c.size());
  filter_m.ProcessBlock(input_c, output_c);
  ASSERT(IsEqual(output_c_cmp, output_c));

  // Various attempt to check that the batch processing does not mess up
  filter_m.ResetState();
  std::vector<Real> input_c_sub_a(input_c.begin(), input_c.begin() + 16);
  std::vector<Real> output_c_cmp_sub_a(output_c_cmp.begin(),
                                       output_c_cmp.begin() + 16);
  std::vector<Real> filter_m_output(input_c_sub_a.size());
  filter_m.ProcessBlock(input_c_sub_a, filter_m_output);
  ASSERT(dsp::IsEqual(filter_m_output, output_c_cmp_sub_a));

  ASSERT(dsp::IsEqual(filter_m.ProcessSample(input_c[16]), output_c_cmp[16]));

  std::vector<Real> input_c_sub_b(input_c.begin() + 17, input_c.end());
  std::vector<Real> output_c_cmp_sub_b(output_c_cmp.begin() + 17,
                                       output_c_cmp.end());
  std::vector<Real> output_c_sub_b(input_c_sub_b.size());
  filter_m.ProcessBlock(input_c_sub_b, output_c_sub_b);
  ASSERT(dsp::IsEqual(output_c_sub_b, output_c_cmp_sub_b));

  //
  filter_m.ResetState();
  ASSERT(dsp::IsEqual(filter_m.ProcessSample(input_c[0]), output_c_cmp[0]));
  ASSERT(dsp::IsEqual(filter_m.ProcessSample(input_c[1]), output_c_cmp[1]));
  ASSERT(dsp::IsEqual(filter_m.ProcessSample(input_c[2]), output_c_cmp[2]));
  std::vector<Real> input_c_sub_ab(input_c.begin() + 3, input_c.begin() + 19);
  std::vector<Real> output_c_cmp_sub_ab(output_c_cmp.begin() + 3,
                                        output_c_cmp.begin() + 19);

  std::vector<Real> output_c_sub_ab(input_c_sub_ab.size());
  filter_m.ProcessBlock(input_c_sub_ab, output_c_sub_ab);
  ASSERT(dsp::IsEqual(output_c_sub_ab, output_c_cmp_sub_ab));
  ASSERT(dsp::IsEqual(filter_m.ProcessSample(input_c[19]), output_c_cmp[19]));

  //
  std::vector<Real> impulse_response_k = {0.8147, 0.9058, 0.1270, 0.9134,
                                          0.6324};
  FirFilter filter_k(impulse_response_k);
  std::vector<Real> input_k = input_c;
  std::vector<Real> output_k_cmp = {
      0.663736090000000, 1.475910520000000, 1.027407440000000,
      1.718367160000000, 2.701277000000000, 1.457092690000000,
      1.310138610000000, 1.865475550000000, 1.799813030000000,
      2.038904730000000, 1.799667500000000, 2.276484260000000,
      3.165878180000000, 2.139907940000000, 2.199456410000000,
      2.390277390000000, 1.622509220000000, 2.184069510000000,
      2.164136180000000, 2.090582990000000};
  std::vector<Real> output_k(input_k.size());
  filter_k.ProcessBlock(input_k, output_k);
  ASSERT(dsp::IsEqual(output_k, output_k_cmp));

  //
  filter_k.ResetState();
  for (Int i = 0; i < (Int)input_c.size() - 1; ++i) {
    ASSERT(IsEqual(filter_k.ProcessSample(input_k[i]), output_k_cmp[i]));
  }

  //
  filter_k.ResetState();
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[0]), output_k_cmp[0]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[1]), output_k_cmp[1]));
  std::vector<Real> input_k_sub_a =
      std::vector<Real>(input_k.begin() + 2, input_k.begin() + 7);
  std::vector<Real> output_k_cmp_sub_a =
      std::vector<Real>(output_k_cmp.begin() + 2, output_k_cmp.begin() + 7);
  std::vector<Real> output_k_sub_a(input_k_sub_a.size());
  filter_k.ProcessBlock(input_k_sub_a, output_k_sub_a);
  ASSERT(IsEqual(output_k_sub_a, output_k_cmp_sub_a));

  std::vector<Real> filter_k_output(2);
  filter_k.ProcessBlock(std::span<const Real>(input_k.begin() + 7, 2),
                        filter_k_output);

  ASSERT(IsEqual(filter_k_output, std::vector<Real>(output_k_cmp.begin() + 7,
                                                    output_k_cmp.begin() + 9)));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[9]), output_k_cmp[9]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[10]), output_k_cmp[10]));
  std::vector<Real> filter_k_output_2(9);
  filter_k.ProcessBlock(
      std::vector<Real>(input_k.begin() + 11, input_k.begin() + 20),
      filter_k_output_2);
  ASSERT(IsEqual(
      filter_k_output_2,
      std::vector<Real>(output_k_cmp.begin() + 11, output_k_cmp.begin() + 20)));

  //
  filter_k.ResetState();
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[0]), output_k_cmp[0]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[1]), output_k_cmp[1]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[2]), output_k_cmp[2]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[3]), output_k_cmp[3]));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[4]), output_k_cmp[4]));
  std::vector<Real> filter_k_output_3(5);
  filter_k.ProcessBlock(std::span(input_k.begin() + 5, 5), filter_k_output_3);
  ASSERT(IsEqual(
      filter_k_output_3,
      std::vector<Real>(output_k_cmp.begin() + 5, output_k_cmp.begin() + 10)));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[10]), output_k_cmp[10]));
  std::vector<Real> filter_k_output_4(8);
  filter_k.ProcessBlock(std::span<Real>(input_k.begin() + 11, 8),
                        filter_k_output_4);
  ASSERT(IsEqual(
      filter_k_output_4,
      std::vector<Real>(output_k_cmp.begin() + 11, output_k_cmp.begin() + 19)));
  ASSERT(IsEqual(filter_k.ProcessSample(input_k[19]), output_k_cmp[19]));

  // Testing slow ipdate of filter
  FirFilter filter_t(dsp::UnaryVector<Real>(1.0));
  ASSERT(IsEqual(filter_t.ProcessSample(0.76), 0.76));
  ASSERT(IsEqual(filter_t.ProcessSample(1.0), 1.0));
  filter_t.SetImpulseResponse(dsp::UnaryVector<Real>(0.3), 1);
  ASSERT(IsEqual(filter_t.ProcessSample(1.0), 0.5 * 1.0 + 0.5 * 0.3));
  ASSERT(IsEqual(filter_t.ProcessSample(1.0), 0.3));

  //
  MaxGradientFilter filter_y(1.0);
  ASSERT(IsEqual(filter_y.ProcessSample(0.0), 0.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.0), 1.0));
  ASSERT(IsEqual(filter_y.ProcessSample(0.0), 0.0));
  ASSERT(IsEqual(filter_y.ProcessSample(-1.0), -1.0));
  ASSERT(IsEqual(filter_y.ProcessSample(-3.0), -2.0));
  ASSERT(IsEqual(filter_y.ProcessSample(-3.0), -3.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.5), -2.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.5), -1.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.5), 0.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.5), 1.0));
  ASSERT(IsEqual(filter_y.ProcessSample(1.5), 1.5));
  ASSERT(IsEqual(filter_y.ProcessSample(-1.5), 0.5));
  ASSERT(IsEqual(filter_y.ProcessSample(-1.5), -0.5));
  ASSERT(IsEqual(filter_y.ProcessSample(-2.5), -1.5));
  ASSERT(IsEqual(filter_y.ProcessSample(-2.5), -2.5));

  return true;
}

void FirFilter::SpeedTests() {
  dsp::RandomGenerator random_generator;

  std::vector<Real> impulse_response = random_generator.Rand(1024);
  FirFilter fir_filter(impulse_response);
  std::vector<Real> input = random_generator.Rand(44100);
  std::vector<Real> output_to_be_ignored(2205);

  clock_t launch = clock();
  for (Int i = 0; i < 20; i++) {
    fir_filter.ProcessBlock(dsp::GetSegment(input, i, 2205),
                            output_to_be_ignored);
  }
  clock_t done = clock();

  std::cout << "Fir filter speed (batch; filter length is not power of 2): "
            << (done - launch) / ((Real)CLOCKS_PER_SEC) * 100 << "% \n";

  launch = clock();
  for (Int i = 0; i < (Int)input.size(); ++i) {
    fir_filter.ProcessSample(input[i]);
  }
  done = clock();

  std::cout
      << "Fir filter speed (sequential; filter length is not power of 2): "
      << (done - launch) / ((Real)CLOCKS_PER_SEC) * 100 << "% \n";

  FirFilter fir_filter_b(random_generator.Rand(1024));

  launch = clock();
  for (Int i = 0; i < 20; i++) {
    fir_filter_b.ProcessBlock(dsp::GetSegment(input, i, 2205),
                              output_to_be_ignored);
  }
  done = clock();

  std::cout << "Fir filter speed (batch; filter length is power of 2): "
            << (done - launch) / ((Real)CLOCKS_PER_SEC) * 100 << "% \n";

  launch = clock();
  for (Int i = 0; i < (Int)input.size(); ++i) {
    fir_filter_b.ProcessSample(input[i]);
  }
  done = clock();

  std::cout << "Fir filter speed (sequential; filter length is power of 2): "
            << (done - launch) / ((Real)CLOCKS_PER_SEC) * 100 << "% \n";
}

} // namespace dsp

} // namespace sal
