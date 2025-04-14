/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_DIGITALFILTER_H
#define SAL_DSP_DIGITALFILTER_H

#include <span>
#include <vector>

#include "dsptypes.h"

namespace sal {

namespace dsp {

class Filter {
 public:
  template <typename FilterType>
  Filter(FilterType x)
      : self_(std::make_unique<FilterModel<FilterType>>(std::move(x))) {}

  Filter(const Filter& x) : self_(x.self_->copy_()) {}

  Filter(Filter&& x) noexcept = default;

  Filter& operator=(const Filter& x) noexcept {
    Filter tmp(x);
    *this = std::move(tmp);  // Using move assignment operator
    return *this;
  }

  /** Move assignment operator */
  Filter& operator=(Filter&& x) noexcept = default;

  // Const version of ProcessBlock for read-only operations
  void ProcessBlock(std::span<const Real> input, std::span<Real> output) const {
    self_->ProcessBlock_(input, output);
  }

  // Non-const version of ProcessBlock for mutable operations
  void ProcessBlock(std::span<const Real> input, std::span<Real> output) {
    const_cast<FilterConcept&>(*self_).ProcessBlock_(input, output);
  }
  
  void ResetState() {
    self_->ResetState_();
  }

 private:
  struct FilterConcept {
    virtual ~FilterConcept() = default;
    virtual void ProcessBlock_(std::span<const Real> input,
                               std::span<Real> output) = 0;
    virtual void ResetState_() = 0;
    virtual std::unique_ptr<FilterConcept> copy_() const = 0;
  };

  template <typename FilterType>
  struct FilterModel final : FilterConcept {
    FilterModel(FilterType x) : data_(std::move(x)) {}

    std::unique_ptr<FilterConcept> copy_() const override {
      return std::make_unique<FilterModel>(*this);
    }

    void ProcessBlock_(std::span<const Real> input,
                       std::span<Real> output) override {
      data_.ProcessBlock(input, output);
    }
    
    void ResetState_() override {
      data_.ResetState();
    }

    FilterType data_;
  };

  std::unique_ptr<FilterConcept> self_;  // Concept is drawable object
};


/** Filter bank abstract class */
class FilterBank {
 public:
  /** Returns the output of the filter bank for an input equal to `input`. Hello
   * world! */
  virtual void ProcessSample(const Real input_data,
                             std::span<Real> output_data) = 0;

  /** Returns the output of the filter bank for a given input. */
  virtual void ProcessBlock(std::span<const Real> input_data,
                            std::span<std::span<Real> > output_data) {
    for (size_t i = 0; i < input_data.size(); ++i) {
      ProcessSample(input_data[i], output_data[i]);
    }
  }

  /** Resets the state of the filter */
  virtual void ResetState() = 0;

  virtual Int num_filters() = 0;
};

} // namespace dsp

} // namespace sal

#endif
