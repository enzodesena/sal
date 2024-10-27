/*
 Spatial Audio Library (SAL)
 Copyright (c) 2018, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <iostream>

#include "audiobuffer.h"
#include "comparisonop.h"

namespace sal {

bool Buffer::Test() {
  using sal::dsp::IsEqual;
  
  Buffer buffer(2, 3);
  ASSERT(buffer.num_samples() == 3);
  ASSERT(buffer.num_channels() == 2);
  for (Int i = 0; i < 2; ++i) {
    for (Int j = 0; j < 3; ++j) {
      ASSERT(dsp::IsEqual(buffer.GetSample(i, j), 0.0));
    }
  }
  
  buffer.SetSample(0, 0, 0.5);
  ASSERT(dsp::IsEqual(buffer.GetSample(0, 0), 0.5));
  buffer.SetSample(0, 2, 0.7);
  ASSERT(dsp::IsEqual(buffer.GetSample(0, 2), 0.7));
  
  Buffer buffer_b(2, 3);
  Buffer::AddBuffers(buffer, buffer_b, buffer_b);
  
  ASSERT(dsp::IsEqual(buffer_b.GetSample(0, 0), 0.5));
  ASSERT(dsp::IsEqual(buffer_b.GetSample(0, 2), 0.7));
  
  Buffer buf(2, 3);
  
  buf.SetSample(0, 1, 1.0);
  buf.SetSample(1, 2, -0.5);
  
  Buffer buf2(buf);
  ASSERT(IsEqual(buf2.GetSample(0, 0), 0.0));
  ASSERT(IsEqual(buf2.GetSample(0, 1), 1.0));
  ASSERT(IsEqual(buf2.GetSample(1, 2), -0.5));
  
  buf2.SetSample(0, 1, 2.0);
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0));
  ASSERT(IsEqual(buf.GetSample(0, 1), 1.0));  // Checking no references
  buf.SetSample(0, 1, 1.5);
  ASSERT(IsEqual(buf.GetSample(0, 1), 1.5));   // Checking no references
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0));  // Checking no references
  
  buf2 = buf;
  ASSERT(IsEqual(buf2.GetSample(0, 0), 0.0));
  ASSERT(IsEqual(buf2.GetSample(0, 1), 1.5));
  buf2.SetSample(0, 1, 2.0);
  buf.SetSample(0, 1, -1.0);
  ASSERT(IsEqual(buf2.GetSample(0, 1), 2.0));  // Checking no references
  ASSERT(IsEqual(buf.GetSample(0, 1), -1.0));  // Checking no references
  
  Buffer buf3(buf.GetWriteViews());
  
  ASSERT(IsEqual(buf3.GetSample(0,0), 0.0));
  ASSERT(IsEqual(buf3.GetSample(0,1), -1.0));
  buf.SetSample(0,1, 1.0);
  ASSERT(IsEqual(buf.GetSample(0,1), 1.0));
  ASSERT(IsEqual(buf3.GetSample(0,1), 1.0)); // Checking yes referencing
  buf3.SetSample(0,1, -1.0);
  ASSERT(IsEqual(buf3.GetSample(0,1), -1.0));
  ASSERT(IsEqual(buf.GetSample(0,1), -1.0)); // Checking yes referencing
  ASSERT(IsEqual(buf2.GetSample(0,1), 2.0)); // Checking no references
  
  Buffer buf4(3,2);
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
  
  buf = buf3; // Trying to assign buf3 onto buf. This has no effect, because buf
  // is already the owner of the data.
  ASSERT(buf.OwnsData());
  ASSERT(!buf3.OwnsData());
  ASSERT(buf.num_channels() == 2);
  ASSERT(buf.num_samples() == 3);
  ASSERT(IsEqual(buf3.GetSample(0, 1), 0.3));
  
  ASSERT(IsEqual(buf.GetSample(0, 0), 0));
  ASSERT(IsEqual(buf.GetSample(0, 1), 0.3));
  ASSERT(IsEqual(buf.GetSample(0, 2), 0.0));
  
  // Test 1: Move Constructor with Owning Buffer
  Buffer owningBuffer(2, 3); // 2 channels, 3 samples per channel
  owningBuffer.SetSample(0, 0, 1.0);
  owningBuffer.SetSample(1, 1, 2.0);

  Buffer movedConstructedBuffer = std::move(owningBuffer);

  ASSERT(movedConstructedBuffer.OwnsData());
  ASSERT(movedConstructedBuffer.num_channels() == 2);
  ASSERT(movedConstructedBuffer.num_samples() == 3);
  ASSERT(movedConstructedBuffer.GetSample(0, 0) == 1.0);
  ASSERT(movedConstructedBuffer.GetSample(1, 1) == 2.0);
  ASSERT(owningBuffer.num_channels() == 0);  // owningBuffer should be empty after the move

  // Test 2: Move Assignment with Owning Buffer
  Buffer owningBuffer2(2, 3);
  owningBuffer2.SetSample(0, 0, 3.0);
  Buffer moveAssignedBuffer;
  moveAssignedBuffer = std::move(owningBuffer2);

  ASSERT(moveAssignedBuffer.OwnsData());
  ASSERT(moveAssignedBuffer.num_channels() == 2);
  ASSERT(moveAssignedBuffer.num_samples() == 3);
  ASSERT(moveAssignedBuffer.GetSample(0, 0) == 3.0);
  ASSERT(owningBuffer2.num_channels() == 0);  // owningBuffer2 should be empty after the move

  // External data for non-owning buffer
  std::vector<double> channel1 = {1.0, 2.0, 3.0};
  std::vector<double> channel2 = {4.0, 5.0, 6.0};
  std::vector<std::span<double>> externalData = {channel1, channel2};

  // Test 3: Move Constructor with Non-Owning Buffer
  Buffer nonOwningBuffer(externalData);
  Buffer movedNonOwningBuffer = std::move(nonOwningBuffer);

  ASSERT(!movedNonOwningBuffer.OwnsData());
  ASSERT(movedNonOwningBuffer.num_channels() == 2);
  ASSERT(movedNonOwningBuffer.num_samples() == 3);
  ASSERT(movedNonOwningBuffer.GetSample(0, 0) == 1.0);
  ASSERT(movedNonOwningBuffer.GetSample(1, 1) == 5.0);
  ASSERT(nonOwningBuffer.num_channels() == 0);  // nonOwningBuffer should be empty after the move

  // Test 4: Move Assignment with Non-Owning Buffer
  Buffer nonOwningBuffer2(externalData);
  Buffer moveAssignedNonOwningBuffer;
  moveAssignedNonOwningBuffer = std::move(nonOwningBuffer2);

  ASSERT(!moveAssignedNonOwningBuffer.OwnsData());
  ASSERT(moveAssignedNonOwningBuffer.num_channels() == 2);
  ASSERT(moveAssignedNonOwningBuffer.num_samples() == 3);
  ASSERT(moveAssignedNonOwningBuffer.GetSample(0, 0) == 1.0);
  ASSERT(moveAssignedNonOwningBuffer.GetSample(1, 1) == 5.0);
  ASSERT(nonOwningBuffer2.num_channels() == 0);  // nonOwningBuffer2 should be empty after the move


  return true;
}

}  // namespace sal
