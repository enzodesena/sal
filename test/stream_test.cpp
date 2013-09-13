/*
 ambisonics_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "stream.h"
#include "equalityop.h"
#include <assert.h>

namespace sal {

bool Stream::Test() {
  using mcl::IsEqual;
  
  // Monostream tests
  MonoStream stream_a;
  assert(stream_a.IsEmpty());
  stream_a.Push(1.0);
  assert(! stream_a.IsEmpty());
  stream_a.Push(2.0);
  assert(IsEqual(stream_a.Pull(),1.0));
  assert(IsEqual(stream_a.Pull(),2.0));
  assert(stream_a.IsEmpty());
  
  stream_a.Push(2.0);
  stream_a.Push(1.0);
  stream_a.Push(0.5);
  Signal output = stream_a.Pull(3);
  assert(IsEqual(output[0],2.0));
  assert(IsEqual(output[1],1.0));
  assert(IsEqual(output[2],0.5));
  
  MonoStream stream_b;
  stream_b.Add(2.0);
  stream_b.Add(-1.0);
  stream_b.Tick();
  
  assert(IsEqual(stream_b.Pull(),1.0));
  assert(stream_b.IsEmpty());
  stream_b.Add(0.5);
  assert(IsEqual(stream_b.Pull(),0.5));
  
  // BFormatStream tests
  BFormatStream stream_c;
  stream_c.Push(0, 0, 0.5);
  assert(IsEqual(stream_c.Pull(0,0), 0.5));
  assert(stream_c.IsEmpty());
  
  stream_c.Add(1,0,-1.0);
  stream_c.Add(1,0,0.2);
  stream_c.Add(1,-1,0.1);
  stream_c.Add(1,-1,-0.2);
  stream_c.Add(0,0,1.0);
  stream_c.Add(0,0,-0.2);
  stream_c.Tick();
  stream_c.Add(1,0,1.0);
  stream_c.Add(1,0,0.2);
  stream_c.Add(1,-1,0.1);
  stream_c.Add(1,-1,0.2);
  stream_c.Add(0,0,-1.0);
  stream_c.Add(0,0,-0.2);
  stream_c.Tick();
  
  assert(IsEqual(stream_c.Pull(1, 0), -0.8));
  assert(IsEqual(stream_c.Pull(1, -1), -0.1));
  assert(IsEqual(stream_c.Pull(0, 0), 0.8));
  assert(IsEqual(stream_c.Pull(1, 0), 1.2));
  assert(IsEqual(stream_c.Pull(0, 0), -1.2));
  assert(IsEqual(stream_c.Pull(1, -1), 0.3));
  
  MonoStream stream_d;
  stream_d.Push(1.0);
  stream_d.Push(2.0);
  stream_d.Push(3.0);
  stream_d.Push(4.0);
  
  MonoStream stream_e;
  stream_e.PushAll(&stream_d);
  assert(stream_d.IsEmpty());
  assert(IsEqual(stream_e.Pull(), 1.0));
  assert(IsEqual(stream_e.Pull(), 2.0));
  assert(IsEqual(stream_e.Pull(), 3.0));
  assert(IsEqual(stream_e.Pull(), 4.0));
  
  return true;
}

} // namespace sal