/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "audiobuffer.hpp"
#include "comparisonop.hpp"

#include <iostream>

namespace sal
{
inline bool BufferTest()
{
  using mcl::IsEqual;

  Buffer<double> buffer(2, 3);
  ASSERT(buffer.num_samples() == 3);
  ASSERT(buffer.num_channels() == 2);
  for (Int i = 0; i < 2; ++i)
  {
    for (Int j = 0; j < 3; ++j)
    {
      ASSERT(mcl::IsEqual(buffer.GetSample(i, j), 0.0));
    }
  }

  buffer.SetSample(0, 0, 0.5);
  ASSERT(mcl::IsEqual(buffer.GetSample(0, 0), 0.5));
  buffer.SetSample(0, 2, 0.7);
  ASSERT(mcl::IsEqual(buffer.GetSample(0, 2), 0.7));

  Buffer<double> buffer_b(2, 3);
  buffer_b.AddSamples(buffer);

  ASSERT(mcl::IsEqual(buffer_b.GetSample(0, 0), 0.5));
  ASSERT(mcl::IsEqual(buffer_b.GetSample(0, 2), 0.7));

  Buffer<double> buf(2, 3);

  buf.SetSample(0, 1, 1.0);
  buf.SetSample(1, 2, -0.5);

  Buffer<double> buf2(buf);
  ASSERT(IsEqual(buf2.GetSample(0, 0), 0.0));
  ASSERT(IsEqual(buf2.GetSample(0, 1), 1.0));
  ASSERT(IsEqual(buf2.GetSample(1, 2), -0.5));

  buf2.SetSample(0, 1, 2.0);
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0));
  ASSERT(IsEqual(buf.GetSample(0, 1), 1.0)); // Checking no references
  buf.SetSample(0, 1, 1.5);
  ASSERT(IsEqual(buf.GetSample(0, 1), 1.5)); // Checking no references
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0)); // Checking no references

  buf2 = buf;
  ASSERT(IsEqual(buf2.GetSample(0, 0), 0.0));
  ASSERT(IsEqual(buf2.GetSample(0, 1), 1.5));
  buf2.SetSample(0, 1, 2.0);
  buf.SetSample(0, 1, -1.0);
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0)); // Checking no references
  ASSERT(IsEqual(buf.GetSample(0, 1), -1.0)); // Checking no references

  buf2.ResetSamples();
  ASSERT(buf2.GetSample(0, 0) == 0.0);
  ASSERT(buf2.GetSample(0, 1) == 0.0);
  ASSERT(buf2.GetSample(0, 2) == 0.0);
  ASSERT(buf2.GetSample(1, 0) == 0.0);
  ASSERT(buf2.GetSample(1, 1) == 0.0);
  ASSERT(buf2.GetSample(1, 2) == 0.0);
  buf2.SetSample(0, 1, 2.0);
  ASSERT(buf2.GetSample(0, 1) == 2.0);
  
  Buffer<double> buf3 = MakeBufferReference(buf);

  ASSERT(IsEqual(buf3.GetSample(0,0), 0.0));
  ASSERT(IsEqual(buf3.GetSample(0,1), -1.0));
  buf.SetSample(0,1, 1.0);
  ASSERT(IsEqual(buf.GetSample(0,1), 1.0));
  ASSERT(IsEqual(buf3.GetSample(0,1), 1.0)); // Checking yes referencing
  buf3.SetSample(0,1, -1.0);
  ASSERT(IsEqual(buf3.GetSample(0,1), -1.0));
  ASSERT(IsEqual(buf.GetSample(0,1), -1.0)); // Checking yes referencing
  ASSERT(IsEqual(buf2.GetSample(0,1), 2.0)); // Checking no references
  
  Buffer<double> buf4(3,2);
  ASSERT(IsEqual(buf4.GetSample(0, 1), 0.0));
  
  buf4 = buf;
  ASSERT(IsEqual(buf4.GetSample(0, 1), -1.0));
  
  buf4 = buf3; // buf4 is now a reference to buf, just like buf3
  // Thus, a change to buf4 is also a change to both buf and buf3
  ASSERT(buf4.num_channels() == 2);
  ASSERT(buf4.num_samples() == 3);
  ASSERT(IsEqual(buf3.GetSample(0, 1), -1.0));
  buf4.SetSample(0, 1, 0.3);
  ASSERT(IsEqual(buf4.GetSample(0, 1), 0.3));
  ASSERT(IsEqual(buf3.GetSample(0, 1), 0.3)); // Checking no references

  return true;
}
} // End namespace
