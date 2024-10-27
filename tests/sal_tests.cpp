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
#include "matrixop.h"
#include "vectorop.h"
#include "transformop.h"
#include "statisticsop.h"
#include "firfilter.h"
#include "randomop.h"
#include "iirfilter.h"
#include "graphiceq.h"
#include <vector>
#include <iostream>

//#include "cipicmic.h"

int main(int argc, char * const argv[]) {
  
#ifndef NDEBUG
  mcl::FirFilter::Test();
  mcl::Quaternion::Test();
  mcl::ElementaryOpTest();
  mcl::BasicOpTest();
  mcl::VectorOpTest();
  mcl::PointWiseOpTest();
  mcl::TransformOpTest();
  mcl::MatrixOpTest();
  mcl::StatisticsOpTest();
  mcl::ComparisonOpTest();
  mcl::PointTest();
  mcl::IirFilter::Test();
  mcl::IirFilter::PeakingFilterTest();
  mcl::IirFilter::PeakHighShelfTest();
  mcl::IirFilter::PeakLowShelfTest();
  mcl::IirFilter::GraphicEqTest();
  mcl::RandomGenerator::Test();
  std::cout<<"All DSP tests succeded!\n";
  
  sal::Buffer::Test();
  sal::Microphone::Test();
  sal::KemarMic::Test();
  sal::SphericalHeadMic::Test();
  sal::SofaMic::Test();
  sal::MicrophoneArrayTest();
  sal::AmbisonicsMic::Test();
  sal::AmbisonicsHorizDec::Test();
  sal::DelayFilter::Test();
  sal::PropagationLine::Test();
  sal::FreeFieldSim::Test();
  sal::CuboidRoom::Test();
  sal::Ism::Test();
  sal::Fdtd::Test();
  sal::RirAnalysis::Test();
  sal::TripletHandler::Test();

  std::cout<<"All SAL tests succeded!\n";
#else
  std::cout<<"Not running tests since NDEBUG is defined and asserts are ignored.\n";
#endif
  
  mcl::FirFilter::SpeedTests();
  sal::TdBem::SimulationTime();
  std::cout<<"FDTD speed: "<<sal::Fdtd::SimulationTime()<<" s\n";
    
  return 0;
}
