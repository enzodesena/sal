/*
 directivitykemar_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "salutilities.h"
#include "cipicmic.h"
#include "microphone.h"
#include "salconstants.h"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  

  
  
bool CipicMic::Test() {
  using mcl::IsEqual;
  
  const std::string cipic_path = std::string("hrtfs/cipic/subject21");
  
  const Int impulse_response_length = 34;
  
  MonoBuffer impulse(impulse_response_length);
  Sample sample = 0.5;
  impulse.SetSample(0, sample);
  
  Sample normalising_value = sample*1.0/NORMALISING_VALUE_CIPIC;
  
  
  // Testing frontal direction
  CipicMic mic_i(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                 cipic_path, wav);
  StereoBuffer stream_i(impulse_response_length);
  
  mic_i.AddPlaneWave(impulse, Point(1.0,0.0,0.0), stream_i);
  
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
  ASSERT(IsEqual(stream_i.GetLeftReadPointer(), cmp_imp_front_left));
  
  
  CipicMic mic_o(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,0,1,PI/2.0),
                 cipic_path, wav);
  StereoBuffer output_buffer_b(impulse_response_length);
  mic_o.AddPlaneWave(impulse, Point(0.0,1.0,0.0), output_buffer_b);
  ASSERT(IsEqual(output_buffer_b.GetLeftReadPointer(), cmp_imp_front_left));
  
  
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
  StereoBuffer stream_m(impulse_response_length);
  
  mic_m.AddPlaneWave(impulse, Point(-1.0,0.0,0.0), stream_m);
  ASSERT(IsEqual(stream_m.GetLeftReadPointer(), cmp_imp_up_left));
  
  
  
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
  StereoBuffer stream_p(impulse_response_length);
  
  // The cipic database has no entry at 90deg. All sources at 80deg azimuth
  // are equally distant.
  // I put the source slightly forward (in x direction) so that I know
  // that the one with 0 elevation will be selected.
  mic_p.AddPlaneWave(impulse, Point(0.001,-1.0,0.0), stream_p);
  ASSERT(IsEqual(stream_p.GetLeftReadPointer(), cmp_imp_right_left));
  ASSERT(IsEqual(stream_p.GetRightReadPointer(), cmp_imp_right_right));
  
  
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
  StereoBuffer stream_r(impulse_response_length);
  
  mic_r.AddPlaneWave(impulse, Point(0.0,1.0,0.0), stream_r);
  ASSERT(mcl::IsEqual(stream_r.GetLeftReadPointer(), cmp_imp_left_left));
  ASSERT(mcl::IsEqual(stream_r.GetRightReadPointer(), cmp_imp_left_right));
  
  
  
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
  StereoBuffer stream_t(impulse_response_length);
  
  mic_t.AddPlaneWave(impulse, Point(0.0,0.0,-1.0), stream_t);
  
  ASSERT(mcl::IsEqual(cmp_imp_back_left, stream_t.GetLeftReadPointer()));
  ASSERT(mcl::IsEqual(cmp_imp_back_right, stream_t.GetRightReadPointer()));
  
  // Testing reset
  stream_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(1.0), Point(0.0,0.0,-1.0), stream_t);
  ASSERT(! IsEqual(stream_t.GetLeftReadPointer()[0], 0.0));
  ASSERT(! IsEqual(stream_t.GetRightReadPointer()[0], 0.0));
  
  stream_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0,0.0,-1.0), stream_t);
  ASSERT(! IsEqual(stream_t.GetLeftReadPointer()[0], 0.0));
  ASSERT(! IsEqual(stream_t.GetRightReadPointer()[0], 0.0));
  
  stream_t.Reset();
  mic_t.Reset();
  mic_t.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0,0.0,-1.0), stream_t);
  ASSERT(IsEqual(stream_t.GetLeftReadPointer()[0], 0.0));
  ASSERT(IsEqual(stream_t.GetRightReadPointer()[0], 0.0));
  
  // Testing bypass
  stream_t.Reset();
  mic_t.set_bypass(false);
  mic_t.set_bypass(true);
  mic_t.AddPlaneWave(MonoBuffer::Unary(1.2), Point(0.0,0.0,-1.0), stream_t);
  ASSERT(IsEqual(stream_t.GetLeftReadPointer()[0], 1.2));
  ASSERT(IsEqual(stream_t.GetRightReadPointer()[0], 1.2));
  
  stream_t.Reset();
  mic_t.set_bypass(false);
  mic_t.AddPlaneWave(impulse, Point(0.0,0.0,-1.0), stream_t);
  ASSERT(mcl::IsEqual(stream_t.GetLeftReadPointer(), cmp_imp_back_left));
  ASSERT(mcl::IsEqual(stream_t.GetRightReadPointer(), cmp_imp_back_right));
  
  
  return true;
}
  
} // namespace sal

