/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "audiobuffer.h"
#include "comparisonop.h"



namespace sal {
  
bool Buffer::Test() {
  
  MultichannelBuffer buffer(2, 3);
  assert(buffer.num_samples() == 3);
  assert(buffer.num_channels() == 2);
  for (Int i=0; i<2; ++i) {
    for (Int j=0; j<3; ++j) {
      assert(mcl::IsEqual(buffer.sample(i, j), 0.0));
    }
  }
  
  buffer.set_sample(0, 0, 0.5);
  assert(mcl::IsEqual(buffer.sample(0, 0), 0.5));
  buffer.set_sample(0, 2, 0.7);
  assert(mcl::IsEqual(buffer.sample(0, 2), 0.7));
  
  MultichannelBuffer buffer_b(2, 3);
  buffer_b.AddSamples(buffer);
  
  assert(mcl::IsEqual(buffer_b.sample(0, 0), 0.5));
  assert(mcl::IsEqual(buffer_b.sample(0, 2), 0.7));
  
  return true;
}
  
} // End namespace
