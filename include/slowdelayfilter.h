/*
 delayfilter.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_SLOWDELAYLINE_H
#define SAL_SLOWDELAYLINE_H

#define DEFAULT_MAX_LATENCY 3276800
#define MAX_LATENCY_JUMP 1
#define UPDATE_STEP 5

#include "saltypes.h"
#include "delayfilter.h"

namespace sal {

  // This is like a DelayFilter, but when a change in latency of more than one
  // sample is requested it doesn't update it immediately, but spreads the
  // change overtime so as to avoid audio artefacts.
class SlowDelayFilter {
  
public:
  // Constructs a delay filter object with intial latency given by latency. A maximum 
  // latency has to be given to allocate the maximum amount of memory of the circular
  // memory.
  SlowDelayFilter(const sal::UInt latency, sal::UInt max_latency);
  
  ~SlowDelayFilter() { delete[] start_; }
  
  // This writes the next sample into the filter. If this method is called 2 times 
  // before the Tick() operation, the former value will be overwritten.
  inline void Write(const sal::Sample& sample) { *write_index_ = sample; }
  
  // Returns the current sample from the filter. Between two Tick() operation it will
  // give always the same output.
  inline sal::Sample Read() const { return *read_index_; }
  
  // This causes time to tick by one sample.
  void Tick();
  
  void set_latency(const sal::UInt);
  
  sal::UInt latency() const { return latency_; }
  
  sal::UInt max_latency() const { return max_latency_; }
  
  SlowDelayFilter& operator= (const SlowDelayFilter&);
  SlowDelayFilter (const SlowDelayFilter&);

  
  static bool Test();
private:
  sal::Sample* start_;
  sal::Sample* end_;
  sal::Sample* write_index_;
  sal::Sample* read_index_;
  sal::UInt latency_;
  sal::UInt max_latency_;
  
  // This method actually updates the latency. set_latency first checks
  // whether there was a jump of more than one sample.
  void UpdateLatency(const sal::UInt latency);
  
  // This is the latency that has been set but not yet achieved
  sal::UInt target_latency_;
  sal::UInt chasing_latency_index_;
};

} // namespace sal
  
#endif
