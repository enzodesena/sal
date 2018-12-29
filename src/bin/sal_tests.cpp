/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include <iostream>

#include "audiobuffer.hpp"
#include "receiver.hpp"
#include "receiverarray.hpp"
#include "sphericalheaddir.hpp"
#include "kemardir.hpp"
#include "ambisonics.hpp"
#include "delayfilter.hpp"
#include "propagationline.hpp"
//#include "freefieldsimulation.hpp"
//#include "wavhandler.hpp"
#include "cipicdir.hpp"
//#include "ism.hpp"
//#include "cuboidroom.hpp"
//#include "fdtd.hpp"
#include "riranalysis.hpp"
#include "directivity.hpp"
//#include "tdbem.hpp"


#include "audiobuffer_test.ipp"
#include "delayfilter_test.ipp"
#include "receiver_test.ipp"
#include "receiverarray_test.ipp"
#include "kemarmic_test.ipp"
#include "cipicdirectivity_test.ipp"
#include "sphericalheadmic_test.ipp"
#include "riranalysis_test.ipp"
#include "propagationline_test.ipp"
#include "ambisonics_test.ipp"

int main(
  int argc,
  char* const argv[])
{
#ifndef NDEBUG
    sal::BufferTest();
    sal::AmbisonicsMicTest();
    sal::AmbisonicsHorizDecTest();
    sal::ReceiverTest();
    sal::KemarMicTest();
    sal::CipicMicTest();
    sal::SphericalHeadMicTest();
    sal::ReceiverArrayTest();
    sal::DelayFilterTest();
    sal::PropagationLineTest();
  //  sal::FreeFieldSim::Test();
  //  sal::CuboidRoom::Test();
  //  sal::Ism::Test();
    sal::RirAnalysisTest();
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
