/*
 delayfilter_test.cpp
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 155 $
 Last changed date:  $Date: 2012-10-07 13:43:12 +0100 (Sun, 07 Oct 2012) $
 */

#include "tools.h"
#include "equalityop.h"

namespace sat {

bool Tools::Test() {
  using mcl::IsEqual;
  
  assert(Next2(3) == 4);
  assert(Next2(3.0) == 4);
  assert(Next2(5) == 8);
  assert(Next2(8) == 8);
  assert(Next2(8.0) == 8);
  assert(Next2(6001) == 8192);
  
  return true;
}
  
} // namespace sat
