/*
 delayfilter.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DELAYLINE_H
#define SAL_DELAYLINE_H

#define DEFAULT_MAX_LATENCY 3276800

#include "saltypes.h"

namespace sal {

class DelayFilter {
  
public:
  // Constructs a delay filter object with intial latency given by latency. A maximum 
  // latency has to be given to allocate the maximum amount of memory of the circular
  // memory.
  DelayFilter(const sal::UInt latency,
              sal::UInt max_latency);
  
  ~DelayFilter() { delete[] start_; }
  
  // This writes the next sample into the filter. If this method is called 2 times 
  // before the Tick() operation, the former value will be overwritten.
  inline void Write(const sal::Sample& sample) { *write_index_ = sample; }
  
  // Returns the current sample from the filter. Between two Tick() operation it will
  // give always the same output.
  inline sal::Sample Read() const { return *read_index_; }
  
  // This causes time to tick by one sample.
  inline void Tick() {
    write_index_ = (write_index_ != end_) ? (write_index_+1) : start_;
    read_index_ = (read_index_ != end_) ? (read_index_+1) : start_;
  }
  
  
  // Resets the latency of the filter. This can introduce artifacts if the latency is 
  // updated too fast.
  void set_latency(const sal::UInt);
  
  sal::UInt latency() const;
  
  sal::UInt max_latency() const;
  
  DelayFilter& operator= (const DelayFilter&);
  DelayFilter (const DelayFilter&);

  
  static bool Test();
private:
  sal::Sample* start_;
  sal::Sample* end_;
  sal::Sample* write_index_;
  sal::Sample* read_index_;
  sal::UInt latency_;
  sal::UInt max_latency_;
};

} // namespace sal
  
#endif
