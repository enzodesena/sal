/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "tdbem.h"
#include "monomics.h"
#include "stream.h"


namespace sal {
  
using mcl::IsEqual;
using sal::Time;
using sal::Length;
using sal::Sample;
using mcl::IirFilter;
using sal::UInt;
using sal::Source;
using sal::Microphone;
using mcl::Point;
using sal::Sample;
using sal::OmniMic;
using sal::Length;
  
bool TdBem::Test() {
  
  
  Time sampling_frequency = 4000;
  
  
  sal::Signal input = mcl::Zeros<sal::Sample>(10);
  input[1]=1.0;
  
  OmniMic mic(Point(1.23, 2.556, 0.456));
  
  Source source(Point(1.23, 2.556, 0.856),
                input);
  
  CuboidRoom room(5.23, 4.86, 2.1,
                  mcl::IirFilter::GainFilter(1));
  
  
  clock_t launch=clock();
  TdBem tdbem(&room, &source, &mic, sampling_frequency, 0.2, 1000000.5);
  clock_t done=clock();
  
  std::cout<<"TDBEM Load: "<<(done - launch) / ((sal::Time) CLOCKS_PER_SEC)<<" s\n";
  
  launch=clock();
  tdbem.Run();
  done=clock();
  
  std::cout<<"TDBEM Run: "<<(done - launch) / ((sal::Time) CLOCKS_PER_SEC)<<" s\n";
  
  return true;
}

  
} // namespace sal

