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
 MonoStream has three modalities:
 - Push(sample) which pushes a sample in the stream
 - Add(sample) which adds the sample to the back of the queue (initialises
 to zero if the stream is empty). This is followed by Tick(), which moves
 to the next sample in time.
 - Merge stream
 The second methodology was introduced because given the recording method
 in the `Microphone`, I had to keep track of temporary outputs at some point
 of the chain. It makes sense that this happens in the stream because
 it is the only way that increases speed by avoiding data duplication.
 */
class MonoStream : public Stream {
public:
  MonoStream() : num_temp_samples_(0) {}
  
  MonoStream(const std::vector<MonoStream*>& streams) :
          merge_streams_(streams) {}
  
  inline void Push(const Sample& sample) {
    if (merge_streams_.size() > 0) { throw_line(""); }
    if (num_temp_samples_ > 0) { throw_line(""); }
    queue_.push_back(sample);
  }
  
  
  void Push(const Signal& signal) {
    if (merge_streams_.size() > 0) { throw_line(""); }
    const UInt num_samples = signal.size();
    for (UInt i=0; i < num_samples; ++i) {
      Push(signal.at(i));
    }
  }
  
  
  void PushAll(MonoStream* stream) {
    if (merge_streams_.size() > 0) { throw_line(""); }
    if (num_temp_samples_ > 0) { throw_line(""); }
    while (! stream->IsEmpty()) {
      Push(stream->Pull());
    }
  }
  
  
  void Add(const Sample& sample) { Add(mcl::UnaryVector<Sample>(sample)); }
  
  
  void Add(const Signal& signal) {
    if (merge_streams_.size() > 0) { throw_line(""); }
    if (num_temp_samples_ == 0) {
      queue_.insert(queue_.end(), signal.begin(), signal.end());
      num_temp_samples_ = signal.size();
    } else {
      if (num_temp_samples_ != signal.size()) { throw_line(""); }
      
      const UInt offset = queue_.size()-signal.size();
      for (UInt i=0; i<signal.size(); ++i) { queue_[offset+i] += signal[i]; }
    }
  }
  
  
  inline void Tick() {
    if (merge_streams_.size() > 0) { throw_line(""); }
    num_temp_samples_ = 0;
  }
  
  
  Sample Pull() {
    if (merge_streams_.size() == 0) {
      if (size() == 0) { throw_line(""); }
      const Sample output = queue_.front();
      queue_.pop_front();
      return output;
    } else {
      // Merging mode
      Sample output = 0.0;
      for (UInt i=0; i<merge_streams_.size(); ++i) {
        output += merge_streams_[i]->Pull();
      }
      return output;
    }
  }
  
  
  Signal Pull(const UInt num_samples) {
    if (num_samples > size()) { throw_line(""); }
    
    if (merge_streams_.size() == 0) {
      Signal output(queue_.begin(), queue_.begin() + num_samples);
      queue_.erase(queue_.begin(), queue_.begin() + num_samples);
      return output;
    } else {
      // Merging mode
      Signal output = mcl::Zeros<sal::Sample>(num_samples);
      for (UInt i=0; i<merge_streams_.size(); ++i) {
        output = mcl::Add(output, merge_streams_[i]->Pull(num_samples));
      }
      return output;
    }
  }
  
  
  /** Pull all samples until the stream is depleted. */
  Signal PullAll() {
    return Pull(size());
  }
  
  
  void Reset() {
    PullAll();
    num_temp_samples_ = 0;
  }
  
  
  inline UInt size() const {
    if (merge_streams_.size() == 0) {
      Int size = (Int)queue_.size()-(Int)num_temp_samples_;
      assert(size>=0);
      return size;
    } else {
      std::vector<UInt> sizes;
      for (UInt i=0; i<merge_streams_.size(); ++i) {
        sizes.push_back(merge_streams_[i]->size());
      }
      return mcl::Min(sizes);
    }
  }
  
  
  inline bool IsEmpty() const {
    return (size()==0);
  }
  
private:
  std::deque<Sample> queue_;
  
  UInt num_temp_samples_;
  
  std::vector<MonoStream*> merge_streams_;
};



class StereoStream : public Stream {
public:
  StereoStream() : left_stream_(MonoStream()), right_stream_(MonoStream()) {}
  
  StereoStream(const std::vector<StereoStream*>& streams) {
    std::vector<MonoStream*> left_streams(streams.size());
    std::vector<MonoStream*> right_streams(streams.size());
    for (UInt i=0; i<streams.size(); ++i) {
      left_streams[i] = streams[i]->left_stream();
      right_streams[i] = streams[i]->right_stream();
    }
    left_stream_ = MonoStream(left_streams);
    right_stream_ = MonoStream(right_streams);
  }
  
  MonoStream* left_stream() { return &left_stream_; }
  MonoStream* right_stream() { return &right_stream_; }
private:
  MonoStream left_stream_;
  MonoStream right_stream_;
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
