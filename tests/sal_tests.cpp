/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <iostream>
#include <vector>

#include "ambisonics.h"
#include "audiobuffer.h"
#include "cuboidroom.h"
#include "delayfilter.h"
#include "fdtd.h"
#include "firfilter.h"
#include "freefieldsimulation.h"
#include "graphiceq.h"
#include "iirfilter.h"
#include "ism.h"
#include "kemarmic.h"
#include "matrixop.h"
#include "microphone.h"
#include "microphonearray.h"
#include "monomics.h"
#include "propagationline.h"
#include "randomop.h"
#include "riranalysis.h"
#include "sofamic.h"
#include "sphericalheadmic.h"
#include "statisticsop.h"
#include "tdbem.h"
#include "transformop.h"
#include "vectorop.h"
#include "wavhandler.h"

int main(int argc, char* const argv[]) {
#ifndef NDEBUG
  sal::dsp::FirFilter::Test();
  sal::dsp::Quaternion::Test();
  sal::dsp::ElementaryOpTest();
  sal::dsp::BasicOpTest();
  sal::dsp::VectorOpTest();
  sal::dsp::PointWiseOpTest();
  sal::dsp::TransformOpTest();
  sal::dsp::MatrixOpTest();
  sal::dsp::StatisticsOpTest();
  sal::dsp::ComparisonOpTest();
  sal::dsp::PointTest();
  sal::dsp::IirFilter::Test();
  sal::dsp::IirFilter::PeakingFilterTest();
  sal::dsp::IirFilter::PeakHighShelfTest();
  sal::dsp::IirFilter::PeakLowShelfTest();
  sal::dsp::IirFilter::GraphicEqTest();
  sal::dsp::RandomGenerator::Test();
  std::cout << "All DSP tests succeded!\n";

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

  std::cout << "All SAL tests succeded!\n";
#else
  std::cout
      << "Not running tests since NDEBUG is defined and asserts are ignored.\n";
#endif

  sal::dsp::FirFilter::SpeedTests();
  sal::TdBem::SimulationTime();
  std::cout << "FDTD speed: " << sal::Fdtd::SimulationTime() << " s\n";

  return 0;
}
