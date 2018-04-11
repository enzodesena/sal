/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "iirfilter.h"
#include "firfilter.h"
#include <vector>
#include "mcltypes.h"
#include "maxgradientfilter.h"
#include "comparisonop.h"
#include "randomop.h"
#include "butter.h"
#include "vectorop.h"

namespace mcl {

bool IirFilter::Test() {
  IirFilter filter_a = IdenticalFilter();
  assert(IsEqual(filter_a.Filter(1.2), 1.2));
  
  IirFilter filter_b = GainFilter(0.76);
  assert(IsEqual(filter_b.Filter(1.2), 0.912));
  
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
  
  IirFilter filter_c(B,A);
  
  assert(IsEqual(filter_c.Filter(1.2), 0.912));
  assert(IsEqual(filter_c.Filter(0.2), -3.064));
  assert(IsEqual(filter_c.Filter(0.5), 0.13824));
  assert(IsEqual(filter_c.Filter(-1.2), 0.11184));
  assert(IsEqual(filter_c.Filter(0.0), 7.226124799999999999999999999999));
  assert(IsEqual(filter_c.Filter(0.0), 2.251659199999999999999999999999));
  
  IirFilter filter_d = WallFilter(carpet_pile, 44100);
  assert(IsEqual(filter_d.Filter(1.0), 0.562666833756030));
  assert(IsEqual(filter_d.Filter(0.5), 0.315580130841020));
  
  IirFilter filter_e = WallFilter(carpet_cotton, 44100);
  assert(IsEqual(filter_e.Filter(1.0), 0.687580695329600));
  assert(IsEqual(filter_e.Filter(0.5), 0.322032066558733));
  
  IirFilter filter_f = WallFilter(wall_bricks, 44100);
  assert(IsEqual(filter_f.Filter(1.0), 0.978495798553620));
  assert(IsEqual(filter_f.Filter(0.5), 0.490594444043047));
  
  IirFilter filter_g = WallFilter(ceiling_tile, 44100);
  assert(IsEqual(filter_g.Filter(1.0), 0.168413736374283));
  assert(IsEqual(filter_g.Filter(0.5), 0.151668254946940));
  
  
  std::vector<Real> input_a(4);
  input_a[0] = 0.6;
  input_a[1] = -3.5;
  input_a[2] = 5.6;
  input_a[3] = 2.3;
  
  // Testing pinkifier filter
  IirFilter pinkifier = PinkifierFilter();
  std::vector<Real> output_e = pinkifier.Filter(input_a);
  std::vector<Real> output_e_cmp(input_a.size());
  output_e_cmp[0] = 0.600000000000000;
  output_e_cmp[1] = -3.152233220000000;
  output_e_cmp[2] = 3.815449359516707;
  output_e_cmp[3] = 4.322130531286722;
  assert(IsEqual(output_e, output_e_cmp));
  
  
  
  std::vector<Real> B_d(3);
  B_d[0] = 1.0;
  B_d[1] = -2.0;
  B_d[2] = 1.0;
  std::vector<Real> A_d(3);
  A_d[0] = 1.005844676087000;
  A_d[1] = -1.999977314492666;
  A_d[2] = 0.994178009420333;
  IirFilter filter_l(B_d, A_d);
  assert(IsEqual(B_d, filter_l.B()));
  assert(IsEqual(A_d, filter_l.A()));
  
  std::vector<Real> signal_d = mcl::Zeros<Real>(4);
  signal_d[0] = 0.989949493661167;
  std::vector<Real> signal_d_out_cmp(4);
  signal_d_out_cmp[0] = 0.984197179938686;
  signal_d_out_cmp[1] = -0.011459974617699;
  signal_d_out_cmp[2] = -0.011370929428126;
  signal_d_out_cmp[3] = -0.011282404149780;
  std::vector<Real> output_d = filter_l.Filter(signal_d);
  assert(IsEqual(output_d, signal_d_out_cmp));
  
  // Testing Reset()
  filter_l.Reset();
  assert(IsEqual(filter_l.Filter(0.0), 0.0));
  
  std::vector<Real> impulse_resp_2(3);
  impulse_resp_2[0] = 0.2;
  impulse_resp_2[1] = -0.1;
  impulse_resp_2[2] = 2.5;
  
  FirFilter filter_m(impulse_resp_2);
  assert(! IsEqual(filter_m.Filter(1.0), 0.0));
  filter_m.Reset();
  assert(IsEqual(filter_m.Filter(0.0), 0.0));
  
  
  // Testing butterworth filter
  IirFilter butter_a = Butter(3, 0.2, 0.45);
  std::vector<Real> butter_a_num_cmp = mcl::Zeros<Real>(7);
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
  
  assert(IsEqual(butter_a.B(), butter_a_num_cmp));
  assert(IsEqual(butter_a.A(), butter_a_den_cmp));
  
  
  IirFilter butter_b = Butter(2, 0.12, 0.79);
  std::vector<Real> butter_b_num_cmp = mcl::Zeros<Real>(5);
  butter_b_num_cmp[0] = 0.469043625796947;
  butter_b_num_cmp[2] = -0.938087251593893;
  butter_b_num_cmp[4] = 0.469043625796947;
  
  
  std::vector<Real> butter_b_den_cmp(5);
  butter_b_den_cmp[0] = 1.000000000000000;
  butter_b_den_cmp[1] = -0.388787442245741;
  butter_b_den_cmp[2] = -0.583519141064213;
  butter_b_den_cmp[3] = 0.041607774454425;
  butter_b_den_cmp[4] = 0.243288940651677;
  
  assert(IsEqual(butter_b.B(), butter_b_num_cmp));
  assert(IsEqual(butter_b.A(), butter_b_den_cmp));
  
  IirFilter filter_i;
  assert(IsEqual(filter_i.Filter(1.2), 1.2));
  assert(IsEqual(filter_i.Filter(-0.2), -0.2));
  
  // Testing octave filter
  IirFilter octave_a = OctaveFilter(3, 4000.0, 44100.0);
  std::vector<Real> octave_a_num_cmp = mcl::Zeros<Real>(7);
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
  
  assert(IsEqual(octave_a.B(), octave_a_num_cmp));
  assert(IsEqual(octave_a.A(), octave_a_den_cmp));
  
  
  // Testing iir filter bank
  IirFilterBank octave_bank_a = OctaveFilterBank(3, 1, 4000.0, 44100.0);
  assert(IsEqual(octave_bank_a.Filter(1.25)[0], octave_a.Filter(1.25)));
  assert(IsEqual(octave_bank_a.Filter(0.25)[0], octave_a.Filter(0.25)));
  assert(IsEqual(octave_bank_a.Filter(5.0)[0], octave_a.Filter(5.0)));
  assert(octave_bank_a.Filter(1.25).size() == 1);
  assert(octave_bank_a.num_filters() == 1);
  
  IirFilterBank octave_bank_b = OctaveFilterBank(3, 2, 2000.0, 44100.0);
  octave_a.Reset();
  assert(IsEqual(octave_bank_b.Filter(1.25)[1], octave_a.Filter(1.25)));
  assert(IsEqual(octave_bank_b.Filter(0.25)[1], octave_a.Filter(0.25)));
  assert(IsEqual(octave_bank_b.Filter(5.0)[1], octave_a.Filter(5.0)));
  assert(octave_bank_b.Filter(1.25).size() == 2);
  assert(octave_bank_b.num_filters() == 2);
  
  octave_a.Reset();
  octave_bank_b.Reset();
  assert(IsEqual(octave_bank_b.Filter(1.25)[1], octave_a.Filter(1.25)));
  assert(IsEqual(octave_bank_b.Filter(0.25)[1], octave_a.Filter(0.25)));
  assert(IsEqual(octave_bank_b.Filter(5.0)[1], octave_a.Filter(5.0)));
  assert(octave_bank_b.Filter(1.25).size() == 2);

  // Test smoothing filter
  SmoothingFilter smooth_filter(5);
  smooth_filter.Filter(mcl::Ones(100));
  smooth_filter.Filter(mcl::Zeros<Real>(4));
  assert(IsEqual(smooth_filter.Filter(0), 1.0/exp(1.0)));
  
  return true;
}
  
  
bool FirFilter::Test() {
  using mcl::IsEqual;
  
  std::vector<Real> impulse_resp(3);
  impulse_resp[0] = 0.2;
  impulse_resp[1] = -0.1;
  impulse_resp[2] = 2.5;
  
  std::vector<Real> impulse(3);
  impulse[0] = 1.0;
  impulse[1] = 0.0;
  impulse[2] = 0.0;
  
  FirFilter filter_a;
  filter_a.set_impulse_response(impulse_resp);
  std::vector<Real> output_aa_cmp = filter_a.Filter(impulse);
  assert(IsEqual(output_aa_cmp, impulse_resp));
  
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
  std::vector<Real> output_a = filter_b.Filter(input_a);
  
  assert(IsEqual(output_a, output_a_cmp));
  
  FirFilter filter_c(impulse_resp);
  assert(IsEqual(filter_c.Filter(0.6), 0.1200));
  assert(IsEqual(filter_c.Filter(-3.5), -0.7600));
  
  // Testing copy constructor
  FirFilter filter_d = filter_c;
  assert(IsEqual(filter_d.Filter(5.6), 2.9700));
  assert(IsEqual(filter_d.Filter(2.3), -8.8500));
  
  assert(IsEqual(filter_c.Filter(5.6), 2.9700));
  assert(IsEqual(filter_c.Filter(2.3), -8.8500));
  
  
  FirFilter filter_i;
  assert(IsEqual(filter_i.Filter(1.2), 1.2));
  assert(IsEqual(filter_i.Filter(-0.2), -0.2));
  
  std::vector<Real> impulse_resp_b = {0.1, 0.3, -0.2, 1.2, -4.5, 0.0, -2.1, -1.2};
  FirFilter filter_l(impulse_resp_b);
  std::vector<Real> input_b = {0.3377, 0.9001, 0.3692, 0.1112, 0.7803, 0.3897, 0.2417, 0.4039, 0.0965, 0.1320, 0.9421, 0.9561};
  std::vector<Real> output_b_cmp = {0.033770000000000, 0.191320000000000, 0.239410000000000, 0.347100000000000, -0.401980000000000, -3.356590000000000, -2.252110000000000, -1.824530000000000, -4.816670000000000, -2.178800000000000, -2.260530000000000, -3.104640000000000};
  std::vector<Real> output_b = filter_l.Filter(input_b);
  assert(IsEqual(output_b_cmp, output_b));
  
  filter_l.Reset();
  for (UInt i=0; i<input_b.size(); ++i) {
    assert(mcl::IsEqual(filter_l.Filter(input_b[i]), output_b_cmp[i]));
  }
  
  
  std::vector<Real> input_c = {0.8147, 0.9058, 0.1270, 0.9134, 0.6324, 0.0975, 0.2785, 0.5469, 0.9575, 0.9649, 0.1576, 0.9706, 0.9572, 0.4854, 0.8003, 0.1419, 0.4218, 0.9157, 0.7922, 0.9595};
  std::vector<Real> impulse_resp_c = {0.6948, 0.3171, 0.9502, 0.0344, 0.4387, 0.3816, 0.7655, 0.7952, 0.1869, 0.4898, 0.4456, 0.6463, 0.7094, 0.7547, 0.2760, 0.6797};
  FirFilter filter_m(impulse_resp_c);
  std::vector<Real> output_c_cmp = {0.566053560000000, 0.887691210000000, 1.149596720000000, 1.563618860000000, 1.238274470000000, 1.848822500000000, 1.881767519999999, 2.373108650000000, 2.702443100000000, 3.155909820000000, 3.544349419999999, 3.760939330000000, 3.860796740000000, 5.071760400000001, 5.228588220000000, 5.070855620000001, 5.216075850000000, 4.336750739999999, 5.636061180000000,5.665156830000000};
  std::vector<Real> output_c = filter_m.Filter(input_c);
  assert(IsEqual(output_c_cmp, output_c));
  
  
  // Various attempt to check that the batch processing does not mess up
  filter_m.Reset();
  std::vector<Real> input_c_sub_a(input_c.begin(), input_c.begin()+16);
  std::vector<Real> output_c_cmp_sub_a(output_c_cmp.begin(), output_c_cmp.begin()+16);
  assert(mcl::IsEqual(filter_m.Filter(input_c_sub_a), output_c_cmp_sub_a));
  
  assert(mcl::IsEqual(filter_m.Filter(input_c[16]), output_c_cmp[16]));
  
  std::vector<Real> input_c_sub_b(input_c.begin()+17, input_c.end());
  std::vector<Real> output_c_cmp_sub_b(output_c_cmp.begin()+17, output_c_cmp.end());
  std::vector<Real> output_c_sub_b = filter_m.Filter(input_c_sub_b);
  assert(mcl::IsEqual(output_c_sub_b, output_c_cmp_sub_b));
  
  //
  filter_m.Reset();
  assert(mcl::IsEqual(filter_m.Filter(input_c[0]), output_c_cmp[0]));
  assert(mcl::IsEqual(filter_m.Filter(input_c[1]), output_c_cmp[1]));
  assert(mcl::IsEqual(filter_m.Filter(input_c[2]), output_c_cmp[2]));
  std::vector<Real> input_c_sub_ab(input_c.begin()+3, input_c.begin()+19);
  std::vector<Real> output_c_cmp_sub_ab(output_c_cmp.begin()+3, output_c_cmp.begin()+19);
  assert(mcl::IsEqual(filter_m.Filter(input_c_sub_ab), output_c_cmp_sub_ab));
  assert(mcl::IsEqual(filter_m.Filter(input_c[19]), output_c_cmp[19]));
  
  
  //
  std::vector<Real> impulse_response_k = {0.8147, 0.9058, 0.1270, 0.9134, 0.6324};
  FirFilter filter_k(impulse_response_k);
  std::vector<Real> input_k = input_c;
  std::vector<Real> output_k_cmp = {0.663736090000000, 1.475910520000000, 1.027407440000000, 1.718367160000000, 2.701277000000000, 1.457092690000000, 1.310138610000000, 1.865475550000000, 1.799813030000000, 2.038904730000000, 1.799667500000000, 2.276484260000000, 3.165878180000000, 2.139907940000000, 2.199456410000000, 2.390277390000000, 1.622509220000000, 2.184069510000000, 2.164136180000000, 2.090582990000000};
  assert(mcl::IsEqual(filter_k.Filter(input_k), output_k_cmp));
  
  //
  filter_k.Reset();
  for (UInt i=0; i<input_c.size()-1; ++i) {
    assert(IsEqual(filter_k.Filter(input_k[i]), output_k_cmp[i]));
  }
  
  //
  filter_k.Reset();
  assert(IsEqual(filter_k.Filter(input_k[0]), output_k_cmp[0]));
  assert(IsEqual(filter_k.Filter(input_k[1]), output_k_cmp[1]));
  std::vector<Real> input_k_sub_a = std::vector<Real>(input_k.begin()+2,
                                                      input_k.begin()+7);
  std::vector<Real> output_k_cmp_sub_a = std::vector<Real>(output_k_cmp.begin()+2,
                                                           output_k_cmp.begin()+7);
  std::vector<Real> output_k_sub_a = filter_k.Filter(input_k_sub_a);
  assert(IsEqual(output_k_sub_a, output_k_cmp_sub_a));
  
  assert(IsEqual(filter_k.Filter(std::vector<Real>(input_k.begin()+7,
                                                   input_k.begin()+9)),
                 std::vector<Real>(output_k_cmp.begin()+7,
                                   output_k_cmp.begin()+9)));
  assert(IsEqual(filter_k.Filter(input_k[9]), output_k_cmp[9]));
  assert(IsEqual(filter_k.Filter(input_k[10]), output_k_cmp[10]));
  assert(IsEqual(filter_k.Filter(std::vector<Real>(input_k.begin()+11,
                                                   input_k.begin()+20)),
                 std::vector<Real>(output_k_cmp.begin()+11,
                                   output_k_cmp.begin()+20)));
  
  
  //
  filter_k.Reset();
  assert(IsEqual(filter_k.Filter(input_k[0]), output_k_cmp[0]));
  assert(IsEqual(filter_k.Filter(input_k[1]), output_k_cmp[1]));
  assert(IsEqual(filter_k.Filter(input_k[2]), output_k_cmp[2]));
  assert(IsEqual(filter_k.Filter(input_k[3]), output_k_cmp[3]));
  assert(IsEqual(filter_k.Filter(input_k[4]), output_k_cmp[4]));
  assert(IsEqual(filter_k.Filter(std::vector<Real>(input_k.begin()+5,
                                                   input_k.begin()+10)),
                 std::vector<Real>(output_k_cmp.begin()+5,
                                   output_k_cmp.begin()+10)));
  assert(IsEqual(filter_k.Filter(input_k[10]), output_k_cmp[10]));
  assert(IsEqual(filter_k.Filter(std::vector<Real>(input_k.begin()+11,
                                                   input_k.begin()+19)),
                 std::vector<Real>(output_k_cmp.begin()+11,
                                   output_k_cmp.begin()+19)));
  assert(IsEqual(filter_k.Filter(input_k[19]), output_k_cmp[19]));
  
  
  // Testing slow ipdate of filter
  FirFilter filter_t(mcl::UnaryVector<Real>(1.0));
  assert(IsEqual(filter_t.Filter(0.76), 0.76));
  assert(IsEqual(filter_t.Filter(1.0), 1.0));
  filter_t.set_impulse_response(mcl::UnaryVector<Real>(0.3), 1);
  assert(IsEqual(filter_t.Filter(1.0), 0.5*1.0+0.5*0.3));
  assert(IsEqual(filter_t.Filter(1.0), 0.3));

  
  //
  MaxGradientFilter filter_y(1.0);
  assert(IsEqual(filter_y.Filter(0.0), 0.0));
  assert(IsEqual(filter_y.Filter(1.0), 1.0));
  assert(IsEqual(filter_y.Filter(0.0), 0.0));
  assert(IsEqual(filter_y.Filter(-1.0), -1.0));
  assert(IsEqual(filter_y.Filter(-3.0), -2.0));
  assert(IsEqual(filter_y.Filter(-3.0), -3.0));
  assert(IsEqual(filter_y.Filter(1.5), -2.0));
  assert(IsEqual(filter_y.Filter(1.5), -1.0));
  assert(IsEqual(filter_y.Filter(1.5), 0.0));
  assert(IsEqual(filter_y.Filter(1.5), 1.0));
  assert(IsEqual(filter_y.Filter(1.5), 1.5));
  assert(IsEqual(filter_y.Filter(-1.5), 0.5));
  assert(IsEqual(filter_y.Filter(-1.5), -0.5));
  assert(IsEqual(filter_y.Filter(-2.5), -1.5));
  assert(IsEqual(filter_y.Filter(-2.5), -2.5));
  
  
  return true;
}

void FirFilter::SpeedTests() {
  
  mcl::RandomGenerator random_generator;
  
  std::vector<Real> impulse_response = random_generator.Rand(1024);
  FirFilter fir_filter(impulse_response);
  std::vector<Real> input = random_generator.Rand(100000);
  
  clock_t launch=clock();
  std::vector<Real> output = fir_filter.Filter(input);
  clock_t done=clock();
  
  std::cout<<"Fir filter speed (batch; length is power of 2): "<<
  (done - launch) / ((Real) CLOCKS_PER_SEC)<<" s\n";
  
  launch=clock();
  for (UInt i=0; i<input.size(); ++i) {
    output[i] = fir_filter.Filter(input[i]);
  }
  done=clock();
  
  std::cout<<"Fir filter speed (sequential; length is power of 2): "<<
  (done - launch) / ((Real) CLOCKS_PER_SEC)<<" s\n";
  
  FirFilter fir_filter_b(random_generator.Rand(1024));
  
  launch=clock();
  output = fir_filter_b.Filter(input);
  done=clock();
  
  std::cout<<"Fir filter speed (batch; length is not power of 2): "<<
  (done - launch) / ((Real) CLOCKS_PER_SEC)<<" s\n";
  
  launch=clock();
  for (UInt i=0; i<input.size(); ++i) {
    output[i] = fir_filter_b.Filter(input[i]);
  }
  done=clock();
  
  std::cout<<"Fir filter speed (sequential; length is not power of 2): "<<
  (done - launch) / ((Real) CLOCKS_PER_SEC)<<" s\n";
  
}

} // namespace mcl
