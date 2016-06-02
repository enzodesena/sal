/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "cipicmic.h"
#include "mcl.h"
#include "microphone.h"
#include "salconstants.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
bool CipicMic::Test() {
  using mcl::IsEqual;
  
  const std::string cipic_path = std::string("lib/cipic/subject21");
  
  const UInt impulse_response_length = 34;
  
  Signal impulse = mcl::Zeros<Sample>(impulse_response_length);
  impulse[0] = 1.0;
  
  Sample normalising_value = 1.0/NORMALISING_VALUE_CIPIC;
  
  
  // Testing frontal direction
  CipicMic mic_i(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                 cipic_path, wav);
  StereoStream* stream_i = mic_i.stream();
  
  Sample sample = 0.5;
  mic_i.RecordPlaneWave(mcl::Multiply(impulse, sample), Point(1.0,0.0,0.0));
  
  const Sample imp_front_left[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -0.000030517578125, -0.000030517578125, -0.000091552734375,
    -0.000183105468750, -0.000335693359375, -0.000518798828125,
    -0.000549316406250, -0.000762939453125, -0.000213623046875,
    -0.000793457031250, -0.000885009765625, -0.000671386718750,
    -0.000701904296875, -0.000885009765625, -0.000762939453125,
    -0.001708984375000, 0.000183105468750, 0.000732421875000};
  
  Signal cmp_imp_front_left(imp_front_left,
                            imp_front_left + sizeof(imp_front_left) / sizeof(Sample));
  cmp_imp_front_left = mcl::Multiply(cmp_imp_front_left, normalising_value);
  cmp_imp_front_left = mcl::Multiply(cmp_imp_front_left, sample);
  Signal akdk = stream_i->left_stream()->PullAll();
  assert(IsEqual(cmp_imp_front_left, akdk));
  
  
  CipicMic mic_o(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0),
                 cipic_path, wav);
  StereoStream* stream_o = mic_o.stream();
  
  mic_o.RecordPlaneWave(mcl::Multiply(impulse, (Sample) 0.5),
                        Point(0.0,1.0,0.0));
  
  assert(mcl::IsEqual(stream_o->left_stream()->PullAll(), cmp_imp_front_left));
  
  
  // Testing upward direction
  const Sample imp_up_left[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -0.000030517578125, -0.000030517578125, -0.000122070312500,
    -0.000244140625000, -0.000274658203125, -0.000427246093750,
    -0.000579833984375, -0.000579833984375, -0.000488281250000,
    -0.000915527343750, -0.000732421875000, -0.000732421875000,
    -0.000823974609375, -0.000946044921875, -0.000823974609375,
    -0.001037597656250, -0.000946044921875, 0.000366210937500};
  
  Signal cmp_imp_up_left(imp_up_left,
                         imp_up_left + sizeof(imp_up_left) / sizeof(Sample));
  cmp_imp_up_left = mcl::Multiply(cmp_imp_up_left, normalising_value);
  
  
  CipicMic mic_m(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,1,0,-PI/2.0), cipic_path, wav);
  StereoStream* stream_m = mic_m.stream();
  
  mic_m.RecordPlaneWave(impulse, Point(-1.0,0.0,0.0));
  Signal output_up_left = stream_m->left_stream()->Pull(impulse_response_length);
  Signal output_up_right = stream_m->right_stream()->Pull(impulse_response_length);
  
  assert(IsEqual(cmp_imp_up_left, output_up_left));
  
  
  
  // Case for a source to the right (90deg) of the kemar.
  const Sample imp_right_left[] = {0, 0, 0, 0,
    -0.000030517578125,
    -0.000061035156250,
    -0.000122070312500,
    -0.000213623046875,
    -0.000335693359375,
    -0.000518798828125,
    -0.000457763671875,
    -0.000579833984375,
    -0.000793457031250,
    -0.000640869140625,
    -0.000915527343750,
    -0.000915527343750,
    -0.000732421875000,
    -0.001007080078125,
    -0.001068115234375,
    -0.000976562500000,
    -0.000946044921875,
    -0.001037597656250,
    -0.000885009765625,
    -0.000793457031250,
    -0.000976562500000,
    -0.000854492187500,
    -0.001159667968750,
    -0.001098632812500,
    -0.000518798828125,
    -0.001007080078125,
    -0.000946044921875,
    -0.000762939453125,
    -0.000976562500000,
    -0.000793457031250};
  
  Signal cmp_imp_right_left(imp_right_left,
                            imp_right_left + sizeof(imp_right_left) / sizeof(Sample));
  cmp_imp_right_left = mcl::Multiply(cmp_imp_right_left, normalising_value);
  
  const Sample imp_right_right[] = {0, 0, 0, 0,
    -0.000061035156250,
    0.000061035156250,
    -0.000061035156250,
    0,
    0.000061035156250,
    -0.000701904296875,
    -0.000183105468750,
    -0.000335693359375,
    -0.000122070312500,
    -0.000427246093750,
    0,
    -0.000762939453125,
    -0.000976562500000,
    0.000976562500000,
    -0.002807617187500,
    0.002136230468750,
    -0.006683349609375,
    0.014312744140625,
    -0.022705078125000,
    0.291259765625000,
    0.348205566406250,
    0.081146240234375,
    0.010162353515625,
    -0.477874755859375,
    -0.158020019531250,
    -0.050842285156250,
    -0.069702148437500,
    0.299865722656250,
    0.065338134765625,
    -0.128631591796875};
  
  Signal cmp_imp_right_right(imp_right_right,
                             imp_right_right + sizeof(imp_right_right) / sizeof(Sample));
  cmp_imp_right_right = mcl::Multiply(cmp_imp_right_right, normalising_value);
  
  
  CipicMic mic_p(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                 cipic_path, wav);
  StereoStream* stream_p = mic_p.stream();
  
  // The cipic database has no entry at 90deg. All sources at 80deg azimuth
  // are equally distant.
  // I put the source slightly forward (in x direction) so that I know
  // that the one with 0 elevation will be selected.
  mic_p.RecordPlaneWave(impulse, Point(0.001,-1.0,0.0));
  Signal output_right_left = stream_p->left_stream()->Pull(impulse_response_length);
  Signal output_right_right = stream_p->right_stream()->Pull(impulse_response_length);
  
  assert(mcl::IsEqual(output_right_left, cmp_imp_right_left));
  assert(mcl::IsEqual(output_right_right, cmp_imp_right_right));
  
  
  
  
  // Case for a source to the left (-90deg) of the kemar.
  const Sample imp_left_right[] = {0,
    0,
    0,
    0,
    -0.000030517578125,
    0.000091552734375,
    -0.000091552734375,
    -0.000183105468750,
    -0.000305175781250,
    -0.000701904296875,
    -0.000518798828125,
    -0.000732421875000,
    -0.000885009765625,
    -0.000640869140625,
    -0.000915527343750,
    -0.000762939453125,
    -0.000610351562500,
    -0.000946044921875,
    -0.000823974609375,
    -0.000549316406250,
    -0.000976562500000,
    -0.001037597656250,
    -0.000488281250000,
    -0.000457763671875,
    -0.001007080078125,
    -0.000885009765625,
    -0.001007080078125,
    -0.000915527343750,
    -0.000457763671875,
    -0.001037597656250,
    -0.000854492187500,
    -0.000518798828125,
    -0.000854492187500,
    -0.000762939453125};
  
  Signal cmp_imp_left_right(imp_left_right,
                            imp_left_right + sizeof(imp_left_right) / sizeof(Sample));
  cmp_imp_left_right = mcl::Multiply(cmp_imp_left_right, normalising_value);
  
  
  
  const Sample imp_left_left[] = {0,
    0,
    0,
    0,
    -0.000030517578125,
    -0.000030517578125,
    -0.000152587890625,
    -0.000274658203125,
    -0.000396728515625,
    -0.000274658203125,
    -0.000305175781250,
    -0.001007080078125,
    -0.000518798828125,
    -0.000976562500000,
    -0.000732421875000,
    -0.001037597656250,
    -0.001373291015625,
    0.000305175781250,
    -0.002288818359375,
    -0.000274658203125,
    -0.003204345703125,
    0.006408691406250,
    0.010040283203125,
    0.323822021484375,
    0.345489501953125,
    -0.000823974609375,
    -0.072540283203125,
    -0.383178710937500,
    -0.109375000000000,
    -0.019348144531250,
    -0.165466308593750,
    0.306335449218750,
    0.132720947265625,
    -0.196655273437500};
  
  Signal cmp_imp_left_left(imp_left_left,
                           imp_left_left + sizeof(imp_left_left) / sizeof(Sample));
  cmp_imp_left_left = mcl::Multiply(cmp_imp_left_left, normalising_value);
  
  CipicMic mic_r(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                 cipic_path, wav);
  StereoStream* stream_r = mic_r.stream();
  
  mic_r.RecordPlaneWave(impulse, Point(0.0,1.0,0.0));
  Signal output_left_left = stream_r->left_stream()->Pull(impulse_response_length);
  Signal output_left_right = stream_r->right_stream()->Pull(impulse_response_length);
  
  assert(mcl::IsEqual(output_left_left, cmp_imp_left_left));
  assert(mcl::IsEqual(output_left_right, cmp_imp_left_right));
  
  
  
  // Case for a source in the back
  const Sample imp_back_left[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -0.000030517578125,
    -0.000030517578125,
    -0.000152587890625,
    -0.000183105468750,
    -0.000244140625000,
    -0.000396728515625,
    -0.000335693359375,
    -0.000610351562500,
    -0.000671386718750,
    -0.000732421875000,
    -0.000701904296875,
    -0.000610351562500,
    -0.000701904296875,
    -0.000823974609375,
    -0.000976562500000,
    -0.000732421875000,
    -0.001251220703125,
    -0.000854492187500,
    -0.001983642578125};
  
  Signal cmp_imp_back_left(imp_back_left,
                           imp_back_left +
                           sizeof(imp_back_left) / sizeof(Sample));
  cmp_imp_back_left = mcl::Multiply(cmp_imp_back_left, normalising_value);
  
  const Sample imp_back_right[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    -0.000030517578125,
    -0.000061035156250,
    -0.000030517578125,
    -0.000183105468750,
    -0.000091552734375,
    -0.000152587890625,
    -0.000335693359375,
    -0.000152587890625,
    -0.000457763671875,
    0.000030517578125,
    -0.000610351562500,
    -0.000061035156250,
    -0.000946044921875,
    0.000335693359375,
    -0.001434326171875,
    0.000671386718750,
    -0.002593994140625,
    0.003295898437500,
    -0.002899169921875};
  
  Signal cmp_imp_back_right(imp_back_right,
                            imp_back_right +
                            sizeof(imp_back_right) / sizeof(Sample));
  cmp_imp_back_right = mcl::Multiply(cmp_imp_back_right, normalising_value);
  
  CipicMic mic_t(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,1,0,-PI/2.0), cipic_path, wav);
  StereoStream* stream_t = mic_t.stream();
  
  mic_t.RecordPlaneWave(impulse, Point(0.0,0.0,-1.0));
  
  assert(mcl::IsEqual(stream_t->left_stream()->PullAll(), cmp_imp_back_left));
  assert(mcl::IsEqual(stream_t->right_stream()->PullAll(), cmp_imp_back_right));
  
  
  
  // Testing reset
  assert(stream_t->left_stream()->IsEmpty());
  assert(stream_t->right_stream()->IsEmpty());
  mic_t.RecordPlaneWave(1.0, Point(0.0,0.0,-1.0));
  assert(! IsEqual(stream_t->left_stream()->Pull(), 0.0));
  assert(! IsEqual(stream_t->right_stream()->Pull(), 0.0));
  assert(stream_t->left_stream()->IsEmpty());
  mic_t.RecordPlaneWave(0.0, Point(0.0,0.0,-1.0));
  assert(! IsEqual(stream_t->left_stream()->Pull(), 0.0));
  assert(! IsEqual(stream_t->right_stream()->Pull(), 0.0));
  assert(stream_t->left_stream()->IsEmpty());
  mic_t.Reset();
  mic_t.RecordPlaneWave(0.0, Point(0.0,0.0,-1.0));
  assert(IsEqual(stream_t->left_stream()->Pull(), 0.0));
  assert(IsEqual(stream_t->right_stream()->Pull(), 0.0));
  
  // Testing bypass
  mic_t.set_bypass(false);
  mic_t.set_bypass(true);
  mic_t.RecordPlaneWave(1.2, Point(0.0,0.0,-1.0));
  assert(IsEqual(stream_t->left_stream()->Pull(), 1.2));
  assert(IsEqual(stream_t->right_stream()->Pull(), 1.2));
  
  mic_t.set_bypass(false);
  mic_t.RecordPlaneWave(impulse, Point(0.0,0.0,-1.0));
  assert(mcl::IsEqual(stream_t->left_stream()->PullAll(), cmp_imp_back_left));
  assert(mcl::IsEqual(stream_t->right_stream()->PullAll(), cmp_imp_back_right));
  
  
  return true;
}
  
} // namespace sal

