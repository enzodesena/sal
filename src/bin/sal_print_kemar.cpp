/*
 sdnc_tests.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
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
  
  //  sal::KemarMic::PrintParsedDatabase(sal::kRightEar, "pss/sal/hrtfs/kemar",
  //                                     sal::KemarMic::kFullBrirLength);
  
  sal::KemarMic::PrintParsedDatabase(sal::kLeftEar, "/Users/enzodesena/repos/sal/hrtfs/kemar_compact",
                                     sal::KemarMic::kFullBrirLength);
  
  return 0;
}
