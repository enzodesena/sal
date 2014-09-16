/*
 digitalfilter_test.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "iirfilter.h"
#include "firfilter.h"
#include <vector>
#include "mcltypes.h"
#include "mcl.h"

namespace mcl {

bool IirFilter::Test() {
  IirFilter filter_a = IirFilter::IdenticalFilter();
  assert(IsEqual(filter_a.Filter(1.2), 1.2));
  
  IirFilter filter_b = IirFilter::GainFilter(0.76);
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
  
  IirFilter filter_d = IirFilter::WallFilter(carpet_pile, 44100);
  assert(IsEqual(filter_d.Filter(1.0), 0.562666833756030));
  assert(IsEqual(filter_d.Filter(0.5), 0.315580130841020));
  
  IirFilter filter_e = IirFilter::WallFilter(carpet_cotton, 44100);
  assert(IsEqual(filter_e.Filter(1.0), 0.687580695329600));
  assert(IsEqual(filter_e.Filter(0.5), 0.322032066558733));
  
  IirFilter filter_f = IirFilter::WallFilter(wall_bricks, 44100);
  assert(IsEqual(filter_f.Filter(1.0), 0.978495798553620));
  assert(IsEqual(filter_f.Filter(0.5), 0.490594444043047));
  
  IirFilter filter_g = IirFilter::WallFilter(ceiling_tile, 44100);
  assert(IsEqual(filter_g.Filter(1.0), 0.168413736374283));
  assert(IsEqual(filter_g.Filter(0.5), 0.151668254946940));
  
  
  std::vector<Real> input_a(4);
  input_a[0] = 0.6;
  input_a[1] = -3.5;
  input_a[2] = 5.6;
  input_a[3] = 2.3;
  
  // Testing pinkifier filter
  IirFilter pinkifier = IirFilter::PinkifierFilter();
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
  IirFilter butter_a = IirFilter::Butter(3, 0.2, 0.45);
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
  
  
  IirFilter butter_b = IirFilter::Butter(2, 0.12, 0.79);
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
  IirFilter octave_a = IirFilter::OctaveFilter(3, 4000.0, 44100.0);
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
  IirFilterBank
  octave_bank_a = IirFilterBank::OctaveFilterBank(3, 1, 4000.0, 44100.0);
  assert(IsEqual(octave_bank_a.Filter(1.25)[0], octave_a.Filter(1.25)));
  assert(IsEqual(octave_bank_a.Filter(0.25)[0], octave_a.Filter(0.25)));
  assert(IsEqual(octave_bank_a.Filter(5.0)[0], octave_a.Filter(5.0)));
  assert(octave_bank_a.Filter(1.25).size() == 1);
  assert(octave_bank_a.num_filters() == 1);
  
  IirFilterBank
  octave_bank_b = IirFilterBank::OctaveFilterBank(3, 2, 2000.0, 44100.0);
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
  
  FirFilter filter_a(impulse_resp);
  assert(IsEqual(filter_a.Filter(impulse), impulse_resp));
  
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
  
  assert(IsEqual(filter_b.Filter(input_a), output_a_cmp));
  
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
  
  return true;
}


} // namespace mcl
