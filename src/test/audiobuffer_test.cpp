/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "audiobuffer.h"
#include "comparisonop.h"

#include <iostream>


namespace sal {
  
bool Buffer::Test() {
  using mcl::IsEqual;
  
  MultichannelBuffer buffer(2, 3);
  assert(buffer.num_samples() == 3);
  assert(buffer.num_channels() == 2);
  for (Int i=0; i<2; ++i) {
    for (Int j=0; j<3; ++j) {
      assert(mcl::IsEqual(buffer.GetSample(i, j), 0.0));
    }
  }
  
  buffer.SetSample(0, 0, 0.5);
  assert(mcl::IsEqual(buffer.GetSample(0, 0), 0.5));
  buffer.SetSample(0, 2, 0.7);
  assert(mcl::IsEqual(buffer.GetSample(0, 2), 0.7));
  
  MultichannelBuffer buffer_b(2, 3);
  buffer_b.AddSamples(buffer);
  
  assert(mcl::IsEqual(buffer_b.GetSample(0, 0), 0.5));
  assert(mcl::IsEqual(buffer_b.GetSample(0, 2), 0.7));
  
  
  MultichannelBuffer buf(2,3);
  
  buf.SetSample(0,1,1.0);
  buf.SetSample(1,2,-0.5);
  
  MultichannelBuffer buf2(buf);
  assert(IsEqual(buf2.GetSample(0,0), 0.0));
  assert(IsEqual(buf2.GetSample(0,1), 1.0));
  assert(IsEqual(buf2.GetSample(1,2), -0.5));
  
  buf2.SetSample(0,1, 2.0);
  assert(IsEqual(buf2.GetSample(0,1), 2.0));
  assert(IsEqual(buf.GetSample(0,1), 1.0)); // Checking no references
  buf.SetSample(0, 1, 1.5);
  assert(IsEqual(buf.GetSample(0,1), 1.5));  // Checking no references
  assert(IsEqual(buf2.GetSample(0,1), 2.0)); // Checking no references
  
  buf2 = buf;
  assert(IsEqual(buf2.GetSample(0,0), 0.0));
  assert(IsEqual(buf2.GetSample(0,1), 1.5));
  buf2.SetSample(0,1, 2.0);
  buf.SetSample(0,1, -1.0);
  assert(IsEqual(buf2.GetSample(0,1), 2.0)); // Checking no references
  assert(IsEqual(buf.GetSample(0,1), -1.0));  // Checking no references
  
  MultichannelBuffer buf3(buf.GetWritePointers(),
                          buf.num_channels(), buf.num_samples());
  
  assert(IsEqual(buf3.GetSample(0,0), 0.0));
  assert(IsEqual(buf3.GetSample(0,1), -1.0));
  buf.SetSample(0,1, 1.0);
  assert(IsEqual(buf.GetSample(0,1), 1.0));
  assert(IsEqual(buf3.GetSample(0,1), 1.0)); // Checking yes referencing
  buf3.SetSample(0,1, -1.0);
  assert(IsEqual(buf3.GetSample(0,1), -1.0));
  assert(IsEqual(buf.GetSample(0,1), -1.0)); // Checking yes referencing
  assert(IsEqual(buf2.GetSample(0,1), 2.0)); // Checking no references
  
  MultichannelBuffer buf4(3,2);
  assert(IsEqual(buf4.GetSample(0, 1), 0.0));
  
  buf4 = buf;
  assert(IsEqual(buf4.GetSample(0, 1), -1.0));
  
  buf4 = buf3; // buf4 is now a reference to buf, just like buf3
  // Thus, a change to buf4 is also a change to both buf and buf3
  assert(buf4.num_channels() == 2);
  assert(buf4.num_samples() == 3);
  assert(IsEqual(buf3.GetSample(0, 1), -1.0));
  buf4.SetSample(0, 1, 0.3);
  assert(IsEqual(buf4.GetSample(0, 1), 0.3));
  assert(IsEqual(buf3.GetSample(0, 1), 0.3)); // Checking no references
  
  buf = buf3; // Trying to assign buf3 onto buf. This has no effect, because buf
              // is already the owner of the data.
  assert(buf.IsDataOwner());
  assert(!buf3.IsDataOwner());
  assert(buf.num_channels() == 2);
  assert(buf.num_samples() == 3);
  assert(IsEqual(buf3.GetSample(0, 1), 0.3));
  
  return true;
}
  
} // End namespace
