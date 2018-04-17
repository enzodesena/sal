/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "microphone.h"
#include "microphonearray.h"
#include "sphericalheadmic.h"
#include "kemarmic.h"
#include "ambisonics.h"
#include "delayfilter.h"
#include "propagationline.h"
#include "freefieldsimulation.h"
#include "wavhandler.h"
#include "cipicmic.h"
#include "ism.h"
#include "cuboidroom.h"
#include "fdtd.h"
#include "riranalysis.h"
#include "monomics.h"
#include "tdbem.h"
#include "audiobuffer.h"
#include <vector>

int main(int argc, char * const argv[]) {
  
  sal::Microphone::Test();
  sal::KemarMic::Test();
  sal::CipicMic::Test();
  sal::Stream::Test();
  sal::SphericalHeadMic::Test();
//  sal::MicrophoneArrayTest();
//  sal::AmbisonicsMic::Test();
//  sal::AmbisonicsHorizDec::Test();
  sal::DelayFilter::Test();
  sal::PropagationLine::Test();
  sal::FreeFieldSim::Test();
  sal::CuboidRoom::Test();
  sal::Ism::Test();
  sal::RirAnalysis::Test();
  sal::TripletHandler::Test();
  sal::Buffer::Test();
  
  std::cout<<"All tests succeded!\n";
  
//  sal::Fdtd::Test();
//  sal::TdBem::Test();
//  std::cout<<"FDTD speed: "<<sal::Fdtd::SimulationTime()<<" s\n";
  
    
  return 0;
}
