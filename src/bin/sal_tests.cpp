/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


#include "audiobuffer.h"
#include "microphone.h"
#include "monomics.h"
#include "kemarmic.h"
#include "sphericalheadmic.h"
#include "sofamic.h"
#include "microphonearray.h"
#include "ambisonics.h"
#include "delayfilter.h"
#include "propagationline.h"
#include "freefieldsimulation.h"
#include "cuboidroom.h"
#include "ism.h"
#include "fdtd.h"
#include "riranalysis.h"
#include "tdbem.h"
#include "wavhandler.h"
#include <vector>
#include <iostream>

//#include "cipicmic.h"

int main(int argc, char * const argv[]) {
  
#ifndef NDEBUG
  sal::Buffer::Test();
  sal::Microphone::Test();
  sal::KemarMic::Test();
  sal::SphericalHeadMic::Test();
  sal::SofaMic::Test();
  sal::MicrophoneArrayTest();
  sal::AmbisonicsMic::Test();
  sal::AmbisonicsHorizDec::Test();
  sal::DelayFilter::Test();
////  sal::CipicMic::Test();
  sal::PropagationLine::Test();
  sal::FreeFieldSim::Test();
  sal::CuboidRoom::Test();
  sal::Ism::Test();
  sal::Fdtd::Test();
  sal::RirAnalysis::Test();
  sal::TripletHandler::Test();

  std::cout<<"All tests succeded!\n";
#else
  std::cout<<"Not running tests since NDEBUG is defined and asserts are ignored.\n";
#endif
  
  sal::TdBem::SimulationTime();
  std::cout<<"FDTD speed: "<<sal::Fdtd::SimulationTime()<<" s\n";
    
  return 0;
}
