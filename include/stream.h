/*
 stream.
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 116 $
 Last changed date:  $Date: 2012-08-01 19:28:40 +0100 (Wed, 01 Aug 2012) $
 */

#ifndef SAT_stream_h
#define SAT_stream_h

#include "sattypes.h"
#include <queue>
#include <vector>
#include <map>
#include <assert.h>

namespace sat {
  
// 
class Stream {
public:
  virtual bool IsEmpty() const = 0;
  
public:
  static bool Test();
};


// MonoStream has two modality for inserting samples:
// - Push(sample) which pushes a sample in the stream
// - Add(sample) which adds the sample to the back of the queue (initialises
//   to zero if the stream is empty) which is followed by Tick() which moves
//   to the next sample in time.
//
// The second methodology was introduced because given the recording method
// in the `Microphone`, I had to keep track of temporary outputs at some point
// of the chain. It makes sense that this happens in the stream because
// it is the only way to avoid duplication and increases speed. 
class MonoStream : public Stream {
public:
  MonoStream() : push_new_sample_(true) {}
  
  inline void Push(const Sample& sample) { queue_.push(sample); }
  
  
  
  void Push(const Signal& signal) {
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(signal.at(i));
    }
  }
  
  
  
  inline void Add(const Sample& sample) {
    if (push_new_sample_) {
      queue_.push(sample);
      push_new_sample_ = false;
    } else {
      assert(! queue_.empty());
      queue_.back() += sample;
    }
  }
  
  inline void Tick() { push_new_sample_ = true; }
  
  Sample Pull() {
    assert(! queue_.empty());
    Sample output = queue_.front();
    queue_.pop();
    return output;
  }
  
  Signal Pull(const UInt num_samples) {
    Signal output(num_samples);
    for (UInt i=0; i<num_samples; ++i) {
      assert(! IsEmpty());
      output[i] = Pull();
    }
    return output;
  }
  
  // Pull all samples until the stream is depleted.
  Signal PullAll() {
    Signal output;
    while (! IsEmpty()) {
      output.push_back(Pull());
    }
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
  std::queue<Sample> queue_;
  
  // This bool reminds the method Add() when to create a new sample. This
  // happens either because the stream is new or because Tick() has been called.
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
  
  void Tick() {
    stream_left_.Tick();
    stream_right_.Tick();
  }
  
  inline bool IsEmpty() const {
    return (stream_left_.IsEmpty() & stream_left_.IsEmpty());
  }
  
  // Returns a vector with two signals. First signal is the left one.
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
  
  inline void Push(UInt order, Int degree, const Sample& sample) {
    InitOrderDegree(order, degree);
    streams_[order][degree].Push(sample);
  }
  
  void Push(UInt order, Int degree, const Signal& signal) {
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(order, degree, signal[i]);
    }
  }
  
  void Add(UInt order, Int degree, Sample sample) {
    InitOrderDegree(order, degree);
    streams_[order][degree].Add(sample);
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
  

  
  Sample Pull(UInt order, Int degree) {
    assert(IsDefined(order, degree));
    return streams_[order][degree].Pull();
  }
  
  // Returns true if the stream associated to `order` and `degree` is empty.
  // Asserts if that stream is not defined.
  inline bool IsEmpty(UInt order, Int degree) const {
    assert(IsDefined(order, degree));
    return streams_.at(order).at(degree).IsEmpty();
  }
  
  virtual bool IsEmpty() const {
    // TODO: think this through
    assert(IsDefined(0, 0));
    // The order 0 and degree 0 always has to be defined.
    return streams_.at(0).at(0).IsEmpty();
  }
  
  bool initialised() { return initialised_; }
  
private:
  inline bool IsDefined(UInt order, Int degree) const {
    if (streams_.count(order) == 0) { return false; }
    if (streams_.at(order).count(degree) == 0) { return false; }
    return true;
  }
  
  // Returns true if the new stream was initialised correctly. False if
  // it already existed.
  bool InitOrderDegree(UInt order, Int degree) {
    // Check whether the stream already exists
    if (IsDefined(order, degree)) { return false; }
    
    // Create new stream
    streams_[order][degree] = MonoStream();
    return true;
  }
  
  std::map<UInt, std::map<Int,MonoStream> > streams_;
  
  bool initialised_;
};
  
} // namespace sat

#endif
