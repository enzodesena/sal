/*
 stream.
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_stream_h
#define SAL_stream_h

#include "saltypes.h"
#include "mcltypes.h"
#include "mcl.h"
#include <queue>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

namespace sal {
  
class Stream {
public:
  static bool Test();
};


/*
 MonoStream has two modality for inserting samples:
 - Push(sample) which pushes a sample in the stream
 - Add(sample) which adds the sample to the back of the queue (initialises
 to zero if the stream is empty) which is followed by Tick() which moves
 to the next sample in time.
 The second methodology was introduced because given the recording method
 in the `Microphone`, I had to keep track of temporary outputs at some point
 of the chain. It makes sense that this happens in the stream because
 it is the only way to avoid duplication and increases speed.
 */
class MonoStream : public Stream {
public:
  MonoStream() : num_temp_samples_(0) {}
  
  inline void Push(const Sample& sample) {
    if (num_temp_samples_ > 0) { throw_line(); }
    queue_.push_back(sample);
  }
  
  
  void Push(const Signal& signal) {
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(signal.at(i));
    }
  }
  
  
  void PushAll(MonoStream* stream) {
    if (num_temp_samples_ > 0) { throw_line(); }
    while (! stream->IsEmpty()) {
      Push(stream->Pull());
    }
  }
  
  
  void Add(const Sample& sample) { Add(mcl::UnaryVector<Sample>(sample)); }
  
  
  void Add(const Signal& signal) {
    if (num_temp_samples_ == 0) {
      queue_.insert(queue_.end(), signal.begin(), signal.end());
      num_temp_samples_ = signal.size();
    } else {
      if (num_temp_samples_ != signal.size()) { throw_line(); }
      
      const UInt offset = queue_.size()-signal.size();
      for (UInt i=0; i<signal.size(); ++i) { queue_[offset+i] += signal[i]; }
    }
  }
  
  
  inline void Tick() { num_temp_samples_ = 0; }
  
  
  Sample Pull() {
    if (size() == 0) { throw_line(); }
    const Sample output = queue_.front();
    queue_.pop_front();
    return output;
  }
  
  
  Signal Pull(const UInt num_samples) {
    if (num_samples > size()) { throw_line(); }
    Signal output(queue_.begin(), queue_.begin() + num_samples);
    queue_.erase(queue_.begin(), queue_.begin() + num_samples);
    return output;
  }
  
  
  /** Pull all samples until the stream is depleted. */
  Signal PullAll() {
    return Pull(size());
  }
  
  
  void Reset() {
    PullAll();
    Tick();
  }
  
  
  inline UInt size() const {
    Int size = (Int)queue_.size()-(Int)num_temp_samples_;
    assert(size>=0);
    return size;
  }
  
  
  inline bool IsEmpty() const {
    return (size()==0);
  }
  
private:
  std::deque<Sample> queue_;
  
  UInt num_temp_samples_;
};



class StereoStream : public Stream {
public:
  MonoStream* left_stream() { return &stream_left_; }
  
  MonoStream* right_stream() { return &stream_right_; }
  
private:
  MonoStream stream_left_;
  MonoStream stream_right_;
};


class MultichannelStream : public Stream {
public:
  void InitStream(std::vector<MonoStream*> streams) {
    initialised_ = true;
    streams_ = streams;
  }
  
  std::vector<Sample> Pull() {
    assert(initialised_);
    const UInt num_streams = streams_.size();
    std::vector<Sample> output(num_streams);
    for (UInt i=0; i<num_streams; ++i) { output[i] = streams_[i]->Pull(); }
    return output;
  }
  
  inline bool IsEmpty() const {
    return streams_[0]->IsEmpty();
  }
  
  void Reset() {
    for (UInt i=0; i<streams_.size(); ++i) { streams_[i]->Reset(); }
  }
  
  bool initialised() { return initialised_; }
  
private:
  std::vector<MonoStream*> streams_;
  bool initialised_;
};

class BFormatStream : public Stream {
public:
  
  inline void Push(UInt degree, Int order, const Sample& sample) {
    InitOrderDegree(degree, order);
    streams_[degree][order].Push(sample);
  }
  
  void Push(UInt degree, Int order, const Signal& signal) {
    InitOrderDegree(degree, order);
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(degree, order, signal[i]);
    }
  }
  
  void Add(UInt degree, Int order, Sample sample) {
    InitOrderDegree(degree, order);
    streams_[degree][order].Add(sample);
  }
  
  void Tick() {
    for(auto outer_iter = streams_.begin();
        outer_iter != streams_.end();
        ++outer_iter) {
      for(auto inner_iter = outer_iter->second.begin();
          inner_iter != outer_iter->second.end();
          ++inner_iter) {
        inner_iter->second.Tick();
      }
    }
  }
  
  void Reset() {
    for(auto outer_iter = streams_.begin();
        outer_iter != streams_.end();
        ++outer_iter) {
      for(auto inner_iter = outer_iter->second.begin();
          inner_iter != outer_iter->second.end();
          ++inner_iter) {
        inner_iter->second.Reset();
      }
    }
  }
  
  Sample Pull(UInt degree, Int order) {
    assert(IsDefined(degree, order));
    return streams_[degree][order].Pull();
  }
  
  /**
   Returns true if the stream associated to `order` and `degree` is empty.
   Asserts if that stream is not defined.
   */
  inline bool IsEmpty(UInt degree, Int order) const {
    assert(IsDefined(degree, order));
    return streams_.at(degree).at(order).IsEmpty();
  }
  
  virtual bool IsEmpty() const {
    // TODO: think this through
    assert(IsDefined(0, 0));
    // The order 0 and degree 0 always has to be defined.
    return streams_.at(0).at(0).IsEmpty();
  }
  
  bool initialised() { return initialised_; }
  
private:
  inline bool IsDefined(UInt degree, Int order) const {
    if (streams_.count(degree) == 0) { return false; }
    if (streams_.at(degree).count(order) == 0) { return false; }
    return true;
  }
  
  /**
   Returns true if the new stream was initialised correctly. False if
   it already existed.
   */
  bool InitOrderDegree(UInt degree, Int order) {
    assert(order<=((Int)degree) & order>=-((Int)degree));
    // Check whether the stream already exists
    if (IsDefined(degree, order)) { return false; }
    
    // Create new stream
    streams_[degree][order] = MonoStream();
    return true;
  }
  
  std::map<UInt, std::map<Int,MonoStream> > streams_;
  
  bool initialised_;
};
  
} // namespace sal

#endif
