/*
 delayfilter_test.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#include "delayfilter.hpp"
#include "comparisonop.hpp"
#include "vectorop.hpp"


namespace sal {

bool DelayFilter::Test() {
  using mcl::IsEqual;
  
  DelayFilter delay_filter_a = DelayFilter(1, 100);
  delay_filter_a.Write(0.0);
  ASSERT(IsEqual(delay_filter_a.Read(),0.0));
  
  delay_filter_a.Tick();
  delay_filter_a.Write(1.0);
  ASSERT(IsEqual(delay_filter_a.Read(),0.0));
  
  delay_filter_a.Tick();
  delay_filter_a.Write(2.0);
  ASSERT(IsEqual(delay_filter_a.Read(),1.0));
  
  delay_filter_a.Tick();
  delay_filter_a.Write(0.0);
  ASSERT(IsEqual(delay_filter_a.Read(),2.0));
  
  delay_filter_a.Tick();
  delay_filter_a.Write(0.0);
  ASSERT(IsEqual(delay_filter_a.Read(),0.0));
  
  DelayFilter delay_filter_b = DelayFilter(3, 100);
  delay_filter_b.Write(1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),0.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(2.0);
  ASSERT(IsEqual(delay_filter_b.Read(),0.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(3.0);
  ASSERT(IsEqual(delay_filter_b.Read(),0.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(-1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),1.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(-1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),2.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(-1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),3.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(-1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),-1.0));
  
  delay_filter_b.Tick();
  delay_filter_b.Write(-1.0);
  ASSERT(IsEqual(delay_filter_b.Read(),-1.0));
  
  
  /////
  
  DelayFilter delay_filter_c = DelayFilter(2,3);
  delay_filter_c.Write(1.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),1.0));
  
  delay_filter_c.SetLatency(3);
  
  delay_filter_c.Tick();
  delay_filter_c.Write(2.0);
  // It is one because by setting latency to 3, the read pointer will go back 
  // and read again the last sample.
  ASSERT(IsEqual(delay_filter_c.Read(),1.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));

  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),2.0));
  
  delay_filter_c.SetLatency(2);
  
  delay_filter_c.Tick();
  delay_filter_c.Write(3.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),0.0));
  
  delay_filter_c.Tick();
  delay_filter_c.Write(0.0);
  ASSERT(IsEqual(delay_filter_c.Read(),3.0));
  
  
  
  DelayFilter delay_filter_d = DelayFilter(1,5);
  
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),0.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.SetLatency(2);
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.SetLatency(4);
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.SetLatency(3);
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  delay_filter_d.Tick();
  delay_filter_d.Write(1.0);
  ASSERT(IsEqual(delay_filter_d.Read(),1.0));
  
  // Testing DelayLine
  DelayFilter delay_line_a(5,5);
  
  delay_line_a.Write(1.5);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 1.5));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(-1.2);
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(0), -1.2));
  ASSERT(IsEqual(delay_line_a.ReadAt(0), -1.2));
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(1), 1.5));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 1.5));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), -1.2));
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(1.01), -1.173));
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(1.5), 0.15));
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(1.7), 0.69));
  ASSERT(IsEqual(delay_line_a.FractionalReadAt(1.99), 1.473));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 1.5));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(-1.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), -1.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), -1.2));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 1.5));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), -1.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), -1.2));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 1.5));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), -1.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), -1.2));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), -1.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), -1.0));
  delay_line_a.Tick();
  
  delay_line_a.Write(0.0);
  ASSERT(IsEqual(delay_line_a.ReadAt(0), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(1), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(2), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(3), 0.0));
  ASSERT(IsEqual(delay_line_a.ReadAt(4), 0.0));
  
  // Testing DelayLine with Filter
  DelayFilter delay_filter_e = DelayFilter(0, 100);
  ASSERT(IsEqual(delay_filter_e.Filter(0.0), 0.0));
  ASSERT(IsEqual(delay_filter_e.Filter(0.2), 0.2));
  delay_filter_e.SetLatency(1);
  delay_filter_e.Reset();
  ASSERT(IsEqual(delay_filter_e.Filter(1.0), 0.0));
  ASSERT(IsEqual(delay_filter_e.Filter(0.0), 1.0));
  
  DelayFilter delay_filter_f(1, 4);
  delay_filter_f.Write(1.0);
  ASSERT(IsEqual(delay_filter_f.Read(), 0.0));
  delay_filter_f.Tick(1);
  ASSERT(IsEqual(delay_filter_f.Read(), 1.0));
  Sample samples[4];
  samples[0] = 0.0;
  samples[1] = 1.0;
  samples[2] = 2.0;
  samples[3] = 3.0;
  delay_filter_f.Write(samples, 4);
  delay_filter_f.Tick(1);
  ASSERT(IsEqual(delay_filter_f.Read(), 0.0));
  delay_filter_f.Tick(1);
  ASSERT(IsEqual(delay_filter_f.Read(), 1.0));
  delay_filter_f.Tick(1);
  ASSERT(IsEqual(delay_filter_f.Read(), 2.0));
  delay_filter_f.Tick(1); 
  ASSERT(IsEqual(delay_filter_f.Read(), 3.0));
  
  const Int num_samples = 10;
  const Int latency = 3;
  mcl::Vector<Sample> input_samples = mcl::ColonOperator(1.0, 1.0, (Sample) num_samples+1);
  mcl::Vector<Sample> output_samples = mcl::Concatenate(mcl::Zeros<Sample>(latency),
                                                        mcl::Elements(input_samples, 0,
                                                                      num_samples-latency));
  assert(input_samples.size() == output_samples.size());
  
  DelayFilter delay_filter_g(latency, 4);
  
  for (Int i=0; i<num_samples; ++i) {
    delay_filter_g.Write(input_samples[i]);
    ASSERT(IsEqual(delay_filter_g.Read(), output_samples[i]));
    delay_filter_g.Tick();
  }
  
  delay_filter_g.Reset();
  Int stride = 2;
  for (Int i=0; i<num_samples; i+=stride) {
    delay_filter_g.Write(&input_samples[i], stride);
    Sample cmp_samples[stride];
    delay_filter_g.Read(stride, cmp_samples);
    ASSERT(IsEqual(cmp_samples, &output_samples[i], stride));
    delay_filter_g.Tick(stride);
  }
  
  delay_filter_g.Reset();
  DelayFilter delay_filter_h(latency, 5);
  stride = 3;
  for (Int i=0; (i+stride)<num_samples; i+=stride) {
    delay_filter_h.Write(&input_samples[i], stride);
    Sample cmp_samples[stride];
    delay_filter_h.Read(stride, cmp_samples);
    
    ASSERT(IsEqual(cmp_samples, &output_samples[i], stride));
    delay_filter_h.Tick(stride);
  }
  
  
  
  return true;
}

} // namespace sal
