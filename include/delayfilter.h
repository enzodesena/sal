/*
 delayfilter.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAT_DELAYLINE_H
#define SAT_DELAYLINE_H

#define DEFAULT_MAX_LATENCY 3276800

#include "sattypes.h"

namespace sat {

class DelayFilter {
  
public:
  // Constructs a delay filter object with intial latency given by latency. A maximum 
  // latency has to be given to allocate the maximum amount of memory of the circular
  // memory.
  DelayFilter(const sat::UInt latency,
              sat::UInt max_latency);
  
  ~DelayFilter() { delete[] start_; }
  
  // This writes the next sample into the filter. If this method is called 2 times 
  // before the Tick() operation, the former value will be overwritten.
  inline void Write(const sat::Sample& sample) { *write_index_ = sample; }
  
  // Returns the current sample from the filter. Between two Tick() operation it will
  // give always the same output.
  inline sat::Sample Read() const { return *read_index_; }
  
  // This causes time to tick by one sample.
  inline void Tick() {
    write_index_ = (write_index_ != end_) ? (write_index_+1) : start_;
    read_index_ = (read_index_ != end_) ? (read_index_+1) : start_;
  }
  
  
  // Resets the latency of the filter. This can introduce artifacts if the latency is 
  // updated too fast.
  void set_latency(const sat::UInt);
  
  sat::UInt latency() const;
  
  sat::UInt max_latency() const;
  
  DelayFilter& operator= (const DelayFilter&);
  DelayFilter (const DelayFilter&);

  
  static bool Test();
private:
  sat::Sample* start_;
  sat::Sample* end_;
  sat::Sample* write_index_;
  sat::Sample* read_index_;
  sat::UInt latency_;
  sat::UInt max_latency_;
};

} // namespace sat
  
#endif
