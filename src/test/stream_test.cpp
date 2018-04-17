/*
 ambisonics_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "stream.h"
#include "comparisonop.h"
#include <assert.h>

namespace sal {
  
bool Stream::Test() {
  using mcl::IsEqual;
  
  // Monostream tests
  MonoStream stream_a;
  ASSERT(stream_a.IsEmpty());
  stream_a.Push(1.0);
  ASSERT(! stream_a.IsEmpty());
  stream_a.Push(2.0);
  ASSERT(IsEqual(stream_a.Pull(),1.0));
  ASSERT(IsEqual(stream_a.Pull(),2.0));
  ASSERT(stream_a.IsEmpty());
  
  stream_a.Push(2.0);
  stream_a.Push(1.0);
  stream_a.Push(0.5);
  Signal output = stream_a.Pull(3);
  ASSERT(IsEqual(output[0],2.0));
  ASSERT(IsEqual(output[1],1.0));
  ASSERT(IsEqual(output[2],0.5));
  
  MonoStream stream_b;
  stream_b.Add(2.0);
  stream_b.Add(-1.0);
  stream_b.Tick();
  
  ASSERT(IsEqual(stream_b.Pull(),1.0));
  ASSERT(stream_b.IsEmpty());
  stream_b.Add(0.5);
  stream_b.Tick();
  ASSERT(IsEqual(stream_b.Pull(),0.5));
  
  // BFormatStream tests
  BFormatStream stream_c;
  stream_c.Push(0, 0, 0.5);
  ASSERT(IsEqual(stream_c.Pull(0,0), 0.5));
  ASSERT(stream_c.IsEmpty());
  
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
  
  ASSERT(IsEqual(stream_c.Pull(1, 0), -0.8));
  ASSERT(IsEqual(stream_c.Pull(1, -1), -0.1));
  ASSERT(IsEqual(stream_c.Pull(0, 0), 0.8));
  ASSERT(IsEqual(stream_c.Pull(1, 0), 1.2));
  ASSERT(IsEqual(stream_c.Pull(0, 0), -1.2));
  ASSERT(IsEqual(stream_c.Pull(1, -1), 0.3));
  
  MonoStream stream_d;
  stream_d.Push(1.0);
  stream_d.Push(2.0);
  stream_d.Push(3.0);
  stream_d.Push(4.0);
  
  MonoStream stream_e;
  stream_e.PushAll(&stream_d);
  ASSERT(stream_d.IsEmpty());
  ASSERT(IsEqual(stream_e.Pull(), 1.0));
  ASSERT(IsEqual(stream_e.Pull(), 2.0));
  ASSERT(IsEqual(stream_e.Pull(), 3.0));
  ASSERT(IsEqual(stream_e.Pull(), 4.0));
  
  MonoStream stream_f;
  stream_f.Push(mcl::Zeros<sal::Sample>(5));
  stream_f.Push(0.5);
  stream_f.Add(mcl::Ones(2));
  stream_f.Add(mcl::Ones(2));
  stream_f.Tick();
  stream_f.Add(-0.5);
  stream_f.Add(-1);
  stream_f.Tick();
  stream_f.Push(-2);
  std::vector<sal::Sample> output_f = stream_f.PullAll();
  std::vector<sal::Sample> output_f_cmp = {0,0,0,0,0, 0.5, 2, 2, -1.5, -2};
  ASSERT(IsEqual(output_f, output_f_cmp));
  ASSERT(stream_f.IsEmpty());
  
  MonoStream stream_g;
  stream_g.Push(0.5);
  stream_g.Push(0.1);
  stream_g.Push(-0.2);
  Signal output_g = stream_g.Pull(2);
  Signal output_g_cmp = {0.5, 0.1};
  ASSERT(IsEqual(output_g_cmp, output_g));
  ASSERT(stream_g.size() == 1);
  ASSERT(IsEqual(stream_g.Pull(), -0.2));
  
  
  stream_g.Push(-0.5);
  stream_g.Reset();
  stream_g.Push(0.5);
  ASSERT(stream_g.PullAll().size()==1);
  stream_g.Push(-0.5);
  stream_g.Reset();
  stream_g.Push(0.4);
  ASSERT(IsEqual(stream_g.PullAll()[0], 0.4));
  
  
  MonoStream stream_k_a;
  MonoStream stream_k_b;
  
  std::vector<MonoStream*> streams_to_merge;
  streams_to_merge.push_back(&stream_k_a);
  streams_to_merge.push_back(&stream_k_b);
  
  MonoStream stream_k_merge(streams_to_merge);
  
  stream_k_a.Push(1.0);
  stream_k_a.Push(3.0);
  stream_k_a.Push(-1.0);
  
  stream_k_b.Push(0.0);
  stream_k_b.Push(0.5);
  ASSERT(stream_k_merge.size() == 2);
  stream_k_b.Push(-2.0);
  ASSERT(stream_k_merge.size() == 3);
  
  Signal merge_output = stream_k_merge.PullAll();
  Signal merge_output_cmp;
  merge_output_cmp.push_back(1.0+0.0);
  merge_output_cmp.push_back(3.0+0.5);
  merge_output_cmp.push_back(-1.0-2.0);
  ASSERT(IsEqual(merge_output, merge_output_cmp));
  
  
  return true;
}
  
} // namespace sal
