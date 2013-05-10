/*
 sdnc_tests.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "microphone.h"
#include "microphonearray.h"
#include "sphericalheadmic.h"
#include "kemarmic.h"
#include "mcl.h"
#include "ambisonics.h"
#include "delayfilter.h"
#include "propagationline.h"
#include "freefieldsimulation.h"
#include "wavhandler.h"

int main(int argc, char * const argv[]) {
  sat::KemarMic::Test();
  sat::Point::Test();
  sat::Stream::Test();
  sat::Microphone::Test();
  sat::PSRMic::Test();
  sat::SphericalHeadMic::Test();
  sat::MicrophoneArray::Test();
  sat::AmbisonicsMic::Test();
  sat::AmbisonicsHorizDec::Test();
  sat::DelayFilter::Test();
  sat::PropagationLine::Test();
  sat::FreeFieldSim::Test();
  
  std::cout<<"All tests succeded!\n";
    
  return 0;
}
