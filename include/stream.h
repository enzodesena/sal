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
#include <queue>
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

namespace sal {
  
class Stream {
public:
  virtual bool IsEmpty() const = 0;
  
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
  MonoStream() : push_new_sample_(true) {}
  
  inline void Push(const Sample& sample) { queue_.push_back(sample); }
  
  
  
  void Push(const Signal& signal) {
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(signal.at(i));
    }
  }
  
  
  
  inline void Add(const Sample& sample) {
    if (push_new_sample_) {
      queue_.push_back(sample);
      push_new_sample_ = false;
    } else {
      if (queue_.empty()) { throw_line(); }
      queue_.back() += sample;
    }
  }
  
  inline void Add(const Signal& signal) {
    if (push_new_sample_) {
      queue_.insert(queue_.end(), signal.begin(), signal.end());
      push_new_sample_ = false;
    } else {
      if (queue_.empty()) { throw_line(); }
      if (queue_.size() < signal.size()) { throw_line(); }
      
      const UInt offset = queue_.size()-signal.size();
      for (UInt i=0; i<signal.size(); ++i) { queue_[offset+i] += signal[i]; }
    }
  }

  inline void Tick() { push_new_sample_ = true; }
  
  Sample Pull() {
    assert(! queue_.empty());
    Sample output = queue_.front();
    queue_.pop_front();
    return output;
  }
  
  Signal Pull(const UInt num_samples) {
    if (num_samples > queue_.size()) { throw_line(); }
    Signal output(queue_.begin(), queue_.begin() + num_samples);
    queue_.erase(queue_.begin(), queue_.begin() + num_samples);
    return output;
  }
  
  /** Pull all samples until the stream is depleted. */
  Signal PullAll() {
    Signal output(queue_.begin(), queue_.end());
    queue_.clear();
    return output;
  }
  
  inline UInt size() const {
    return queue_.size();
  }
  
  inline bool IsEmpty() const {
    return queue_.empty();
  }
  
  void PushAll(MonoStream* stream) {
    while (! stream->IsEmpty()) {
      Push(stream->Pull());
    }
  }
  
private:
  std::deque<Sample> queue_;
  
  /** 
   This bool reminds the method Add() when to create a new sample. This
   happens either because the stream is new or because Tick() has been called.
   */
  bool push_new_sample_;
};
  
  

class StereoStream : public Stream {
public:
  void Push(Sample sample_left, Sample sample_right) {
    stream_left_.Push(sample_left);
    stream_right_.Push(sample_right);
  }
  
  void Add(Sample sample_left, Sample sample_right) {
    stream_left_.Add(sample_left);
    stream_right_.Add(sample_right);
  }
  
  void Add(const Signal& signal_left, const Signal& signal_right) {
    stream_left_.Add(signal_left);
    stream_right_.Add(signal_right);
  }
  
  void Tick() {
    stream_left_.Tick();
    stream_right_.Tick();
  }
  
  inline bool IsEmpty() const {
    return (stream_left_.IsEmpty() | stream_left_.IsEmpty());
  }
  
  /** Returns a vector with two signals. First signal is the left one. */
  std::vector<Signal> Pull(const UInt num_samples) {
    std::vector<Signal> output(2);
    output[0] = stream_left_.Pull(num_samples);
    output[1] = stream_right_.Pull(num_samples);
    return output;
  }
  
  std::vector<Sample> Pull() {
    std::vector<Sample> output(2);
    output[0] = stream_left_.Pull();
    output[1] = stream_right_.Pull();
    return output;
  }
  
  Sample PullLeft() { return stream_left_.Pull(); }
  Sample PullRight() { return stream_right_.Pull(); }
  
  Signal PullAllLeft() { return stream_left_.PullAll(); }
  Signal PullAllRight() { return stream_right_.PullAll(); }
  
  StereoSignal PullAll() {
    StereoSignal output;
    output.left = stream_left_.PullAll();
    output.right = stream_right_.PullAll();
    return output;
  }
  
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
