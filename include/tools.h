/*
 tools.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 150 $
 Last changed date:  $Date: 2012-10-05 06:11:12 +0100 (Fri, 05 Oct 2012) $
 */

// This contains various mathematical tools. 

#ifndef SAT_TOOLS_H
#define SAT_TOOLS_H


#include <cmath>
#include "mcl.h"
#include "sattypes.h"

namespace sat {
  
class Tools {
public:
  // This returns the next power of 2. For instance 5=>8, 12=>16, 16=>16.
  static sat::UInt Next2(UInt input) {
    return (sat::UInt) pow(2, mcl::NextPow2(input));
  }
  
  static bool Test();
};

  
} // namespace sat

#endif
