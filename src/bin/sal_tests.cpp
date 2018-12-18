/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include <iostream>

#include "audiobuffer.hpp"
#include "audiobuffer_test.hpp"
#include "receiver.hpp"
//#include "microphonearray.hpp"
#include "sphericalheadmic.hpp"
#include "kemarmic.hpp"
//#include "ambisonics.hpp"
#include "delayfilter.hpp"
//#include "propagationline.hpp"
//#include "freefieldsimulation.hpp"
//#include "wavhandler.hpp"
#include "cipicdirectivity.hpp"
//#include "ism.hpp"
//#include "cuboidroom.hpp"
//#include "fdtd.hpp"
//#include "riranalysis.hpp"
#include "directivity.hpp"
//#include "tdbem.hpp"
//#include "vector.hpp"


//#include "delayfilter_test.hpp"
#include "microphone_test.hpp"
#include "kemarmic_test.hpp"
#include "cipicdirectivity_test.hpp"
#include "sphericalheadmic_test.hpp"

int main(
  int argc,
  char* const argv[])
{
#ifndef NDEBUG
    sal::BufferTest();
  //  sal::AmbisonicsMic::Test();
  //  sal::AmbisonicsHorizDec::Test();
    sal::ReceiverTest();
    sal::KemarMicTest();
    sal::CipicMicTest();
    sal::SphericalHeadMicTest();
  //  sal::MicrophoneArrayTest();
//    sal::DelayFilterTest();
  //  sal::PropagationLine::Test();
  //  sal::FreeFieldSim::Test();
  //  sal::CuboidRoom::Test();
  //  sal::Ism::Test();
  //  sal::RirAnalysis::Test();
  //  sal::TripletHandler::Test();
  //  sal::Fdtd::Test();

  std::cout << "All tests succeded!\n";
#else
  std::cout<<"Not running tests since NDEBUG is defined and asserts are ignored.\n";
#endif
  
  //  sal::TdBem::SimulationTime();
  //  std::cout<<"FDTD speed: "<<sal::Fdtd::SimulationTime()<<" s\n";

  return 0;
}
