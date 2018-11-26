/*
 directivityspherical_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "sphericalheadmic.hpp"
#include "microphone.hpp"
#include "salconstants.hpp"

using mcl::Point;
using mcl::Quaternion;

namespace sal {
  
bool SphericalHeadMic::Test() {
  using mcl::Complex;
  using mcl::IsEqual;
  
  ASSERT(IsEqual(Sphere(0.09, 2.5, PI/2, 500, 343, 0.0001),
                 Complex(0.948745443360044, -0.077722752497006)));
  
  ASSERT(IsEqual(Sphere(0.19, 2.25, PI/3.0, 550.0, 340.0, 0.00001),
                 Complex(0.605269803039302, - 1.296402786840865)));
  
  ASSERT(IsEqual(Sphere(0.20, 10.0, PI, 13000.0, 341.0, 0.00002),
                 Complex(-0.109475723791322, 0.458127257654756)));
  
  ASSERT(IsEqual(Sphere(0.15, 100.0, PI, 130.0, 330.0, 0.00012),
                 Complex(0.855026629580043, 0.533944979396686)));
  
  mcl::Vector<mcl::Real> hrir_a = GenerateImpulseResponse(0.09, 2, PI/4.0, 343,
                                                          0.0001, 6, 40000.0,
                                                          false);
  
  mcl::Vector<mcl::Real> hrir_a_cmp(6);
  hrir_a_cmp[0] = -0.422751207729102;
  hrir_a_cmp[1] = 1.25968413581477;
  hrir_a_cmp[2] = 0.274433385673422;
  hrir_a_cmp[3] = -0.25262339189358;
  hrir_a_cmp[4] = 0.14831782205568;
  hrir_a_cmp[5] = -0.836932928085671;
  ASSERT(IsEqual(hrir_a_cmp, hrir_a));
  
  Time sampling_frequency(40000);
  
  MonoBuffer impulse(6);
  impulse.SetSample(0, 1.0);
  
  Angle ears_angle(100.0/180.0*PI);
  SphericalHeadMic mic_a(Point(0.0,0.0,0.0), mcl::AxAng2Quat(0,1,0,-PI/2.0),
                         ears_angle, // ears angle
                         0.09, // sphere radius
                         6, // impulse response length
                         sampling_frequency);
  StereoBuffer stream_a(impulse.num_samples());
  
  Length distance(2.0);
  
  // Generate a point on the line of the left ear.
  Point point_line_left(0.0,
                        distance*cos(ears_angle-PI/2.0),
                        -distance*sin(ears_angle-PI/2.0));
  
  mic_a.AddPlaneWave(impulse, point_line_left, stream_a);
  
  
  // The output of the left ear should be the sphere response from angle
  // theta=0 (reference system of Duda's paper).
  
  
  mcl::Vector<Sample> output_ipsilateral(6);
  output_ipsilateral[0] = -0.064768721403534;
  output_ipsilateral[1] = -1.16235892863396;
  output_ipsilateral[2] = 0.0436720375962888;
  output_ipsilateral[3] = 0.0952699695904199;
  output_ipsilateral[4] = 0.0210966838072453;
  output_ipsilateral[5] = 1.2271276500375;
  ASSERT(IsEqual(stream_a.GetLeftReadPointer(), output_ipsilateral));
  
  
  // Trying now contralateral case (right ear output due to the
  // direction of left ear).
  mcl::Vector<Sample> output_contralateral(6);
  output_contralateral[0] = 0.091462139312559;
  output_contralateral[1] = -0.18323871195895;
  output_contralateral[2] = -0.11070828693118;
  output_contralateral[3] = -0.17029879934648;
  output_contralateral[4] = 0.019246147618618;
  output_contralateral[5] = 0.091776572646393;
  ASSERT(IsEqual(stream_a.GetRightReadPointer(), output_contralateral));
  
  // ASSERT(IsEqual(mic_a_left.ImpulseResponse(point_line_left), output_a_cmp));
  
  // Generate a point on the line passing through the centre
  // of the head and the right ear.
  Point point_line_right(0.0,
                         -distance*cos(ears_angle-PI/2.0),
                         -distance*sin(ears_angle-PI/2.0));
  // Since the impulse response has 6 bins, I can reuse the same microphone for
  // this test.
  stream_a.Reset();
  mic_a.AddPlaneWave(impulse, point_line_right, stream_a);
  ASSERT(IsEqual(stream_a.GetRightReadPointer(), output_ipsilateral));
  ASSERT(IsEqual(stream_a.GetLeftReadPointer(), output_contralateral));
  
  
  SphericalHeadMic mic_b(Point(0.0,0.0,0.0), mcl::Quaternion::Identity(),
                         ears_angle, // ears angle
                         0.09, // sphere radius
                         6, // impulse response length
                         sampling_frequency);
  StereoBuffer stream_b(impulse.num_samples());
  
  // Point in front of face.
  Point point_front(distance, 0.0, 0.0);
  
  mic_b.AddPlaneWave(impulse, point_front, stream_b);
  ASSERT(IsEqual(stream_b.GetLeftReadPointer(),
                 stream_b.GetRightReadPointer(), impulse.num_samples()));
  
  
  mcl::Vector<Sample> output_frontal(6);
  output_frontal[0] = 0.5249943567937;
  output_frontal[1] = -0.39587582734155;
  output_frontal[2] = -0.61842280953724;
  output_frontal[3] = -0.13938281540835;
  output_frontal[4] = 0.093428452743537;
  output_frontal[5] = -0.12911852945215;
  
  ASSERT(IsEqual(stream_b.GetLeftReadPointer(), output_frontal));
  
  // Testing reset
  stream_b.Reset();
  mic_b.AddPlaneWave(MonoBuffer::Unary(1.0), Point(0.0,0.0,-1.0), stream_b);
  ASSERT(! IsEqual(stream_b.GetLeftReadPointer()[0], 0.0));
  ASSERT(! IsEqual(stream_b.GetRightReadPointer()[0], 0.0));
  
  stream_b.Reset();
  mic_b.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0,0.0,-1.0), stream_b);
  ASSERT(! IsEqual(stream_b.GetLeftReadPointer()[0], 0.0));
  ASSERT(! IsEqual(stream_b.GetRightReadPointer()[0], 0.0));
  
  stream_b.Reset();
  mic_b.Reset();
  mic_b.AddPlaneWave(MonoBuffer::Unary(0.0), Point(0.0,0.0,-1.0), stream_b);
  ASSERT(IsEqual(stream_b.GetLeftReadPointer()[0], 0.0));
  ASSERT(IsEqual(stream_b.GetRightReadPointer()[0], 0.0));
  
  return true;
}
  
} // namespace sal
