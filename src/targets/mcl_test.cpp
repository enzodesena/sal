/*
 mcl_tests.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 84 $
 Last changed date:  $Date: 2012-03-02 19:33:26 +0000 (Fri, 02 Mar 2012) $
 */


#include <iostream>
#include "mcl.h"
#include "matrixop.h"




int main (int argc, char * const argv[]) {
  using namespace mcl;
  
  BasicOpTest();
  VectorOpTest();
  PointWiseOpTest();
  TransformOpTest();
  ElementaryOpTest();
  MatrixOpTest();
  
  IirFilter::Test();
  FirFilter::Test();
  RandomGenerator::Test();
    
    
  
  
    
  
    
  
  std::cout<<"All tests succeded!\n";
  
  return 0;
}