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

int main(int argc, char * const argv[]) {
  try {
    sal::Microphone::Test();
    sal::KemarMic::Test();
    sal::CipicMic::Test();
    sal::Stream::Test();
    sal::SphericalHeadMic::Test();
    sal::MicrophoneArrayTest();
    sal::AmbisonicsMic::Test();
    sal::AmbisonicsHorizDec::Test();
    sal::DelayFilter::Test();
    sal::PropagationLine::Test();
    sal::FreeFieldSim::Test();
  } catch (mcl::Exception& exception) {
    std::cout<<"An exception occurred: "<<exception.what()<<std::endl;
  }
  
  std::cout<<"All tests succeded!\n";
    
  return 0;
}
