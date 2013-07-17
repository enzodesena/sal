/*
 mcl_tests.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include <iostream>
#include "mcl.h"
#include "matrixop.h"



int main (int argc, char * const argv[]) {
  using namespace mcl;
  
  ElementaryOpTest();
  BasicOpTest();
  VectorOpTest();
  PointWiseOpTest();
  TransformOpTest();
  MatrixOpTest();
  StatisticsOpTest();
  
  IirFilter::Test();
  FirFilter::Test();
  RandomGenerator::Test();
  
  std::cout<<"All tests succeded!\n";
  
  return 0;
}