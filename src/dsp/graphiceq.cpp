/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <algorithm>
#include <limits>

#include "butter.h"
#include "comparisonop.h"
#include "graphiceq.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

constexpr Real EPS = 0.0000001;

GraphicEq::GraphicEq(const std::vector<Real>& gain, const std::vector<Real>& fc,
                     const Real Q, const Real sampling_frequency)
    : num_filters_(((int)fc.size()) + 2),
      low_shelf_((fc[0] / 2.0) * sqrt(fc[0] / (fc[0] / 2.0)), Q,
                 sampling_frequency),
      high_shelf_(fc[num_filters_ - 3] *
                      sqrt((2.0 * fc[num_filters_ - 3]) / fc[num_filters_ - 3]),
                  Q, sampling_frequency),
      db_gain_(num_filters_),
      input_gain_(num_filters_),
      target_gain_(num_filters_ + 1),
      current_gain_(num_filters_ + 1),
      last_input_(fc.size()),
      mat_(num_filters_, num_filters_),
      equal_(false),
      valid_(false) {
  InitFilters(fc, Q, sampling_frequency);
  InitMatrix(fc, sampling_frequency);
  InitParameters(gain);
}

void GraphicEq::InitFilters(const std::vector<Real>& fc, const Real Q,
                            const Real sampling_frequency) {
  for (int i = 0; i < num_filters_ - 2; i++)
    peaking_filters_.push_back(PeakingFilter(fc[i], Q, sampling_frequency));
}

void GraphicEq::InitMatrix(const std::vector<Real>& fc,
                           const Real sampling_frequency) {
  std::vector<Real> f = std::vector<Real>(num_filters_, 0.0);

  f[0] = fc[0] / 2.0;
  for (int i = 1; i < num_filters_ - 1; i++) f[i] = fc[i - 1];
  f[num_filters_ - 1] = 2.0 * fc[num_filters_ - 3];

  Real pdb = 6.0;
  Real p = pow(10.0, pdb / 20.0);

  std::vector<Real> out = std::vector<Real>(f.size(), 0.0);
  int j = 0;

  low_shelf_.UpdateGain(p);
  out = low_shelf_.GetFrequencyResponse(f, sampling_frequency);
  low_shelf_.UpdateGain(0.0);

  for (size_t i = 0; i < out.size(); i++) {
    mat_.SetElement(j, i, mat_.GetElement(j, i) + out[i]);
  }

  j++;

  for (PeakingFilter& filter : peaking_filters_) {
    filter.UpdateGain(p);
    out = filter.GetFrequencyResponse(f, sampling_frequency);
    filter.UpdateGain(1.0);

    for (size_t i = 0; i < out.size(); i++)
      mat_.SetElement(j, i, mat_.GetElement(j, i) + out[i]);
    j++;
  }

  high_shelf_.UpdateGain(p);
  out = high_shelf_.GetFrequencyResponse(f, sampling_frequency);
  high_shelf_.UpdateGain(0.0);

  for (size_t i = 0; i < out.size(); i++) {
    mat_.SetElement(j, i, mat_.GetElement(j, i) + out[i]);
  }

  mat_ = Inverse(mat_);
  mat_ = Multiply(mat_, pdb);
}

void GraphicEq::InitParameters(const std::vector<Real>& gain) {
  SetGain(gain);
  current_gain_ = target_gain_;
  equal_ = true;
  UpdateParameters();
}

void GraphicEq::SetGain(const std::vector<Real>& gain) {
  if (IsEqual(gain, last_input_)) return;

  last_input_ = gain;
  if (IsEqual(gain, 0.0)) {
    std::fill(input_gain_.begin(), input_gain_.end(), 0.0);
    target_gain_[0] = 0;
  } else {
    // when dB is used here. Factors of 20 are cancelled out.
    Real g = (gain[0] + gain[1]) / 2.0;
    db_gain_[0] = std::max(g, EPS);  // Prevent log10(0)

    for (int i = 1; i < num_filters_ - 1; i++)
      db_gain_[i] = std::max(gain[i - 1], EPS);  // Prevent log10(0)

    g = (gain[num_filters_ - 4] + gain[num_filters_ - 3]) / 2.0;
    db_gain_[num_filters_ - 1] = std::max(g, EPS);  // Prevent log10(0)

    std::transform(db_gain_.begin(), db_gain_.end(), db_gain_.begin(),
                   [](double x) { return std::log10(x); });
    Real mean_db_gain = Sum(db_gain_) / ((Real)db_gain_.size());
    target_gain_[0] = dsp::Pow(10.0, mean_db_gain);  // 10 ^ mean(db_gain_);
    std::transform(db_gain_.begin(), db_gain_.end(), db_gain_.begin(),
                   [mean_db_gain](double x) {
                     return x - mean_db_gain;
                   });  // db_gain_ - mean(db_gain_);

    input_gain_ = dsp::Multiply(db_gain_, mat_);
    input_gain_ = dsp::Pow(10.0, input_gain_);
  }

  for (int i = 0; i < num_filters_; i++) target_gain_[i + 1] = input_gain_[i];

  if (target_gain_ != current_gain_) equal_ = false;
}

void GraphicEq::PrintCoefficients() {
  Matrix<Real> num_parameters(num_filters_ + 1, 3);
  num_parameters.SetRow(0, low_shelf_.B());
  num_parameters.SetRow(1, high_shelf_.B());

  Matrix<Real> den_parameters(num_filters_ + 1, 3);
  den_parameters.SetRow(0, low_shelf_.A());
  den_parameters.SetRow(1, high_shelf_.A());

  int i = 2;
  for (PeakingFilter& filter : peaking_filters_) {
    num_parameters.SetRow(i, filter.B());
    den_parameters.SetRow(i++, filter.A());
  }
  num_parameters.SetRow(i, {current_gain_[0], 0.0, 0.0});
  den_parameters.SetRow(i, {1, 0.0, 0.0});
  Print(num_parameters);
  Print(den_parameters);
}

void GraphicEq::UpdateParameters() {
  int i = 1;

  low_shelf_.UpdateGain(current_gain_[i]);
  i++;

  for (PeakingFilter& filter : peaking_filters_) {
    filter.UpdateGain(current_gain_[i]);
    i++;
  }

  high_shelf_.UpdateGain(current_gain_[i]);

  if (current_gain_[0] == 0) {
    valid_ = false;
  } else {
    valid_ = true;
  }
}

Real GraphicEq::ProcessSample(const Real input) noexcept {
  if (valid_) {
    Real out = input;
    out = low_shelf_.ProcessSample(out);
    for (PeakingFilter& filter : peaking_filters_)
      out = filter.ProcessSample(out);
    out = high_shelf_.ProcessSample(out);
    out *= current_gain_[0];
    return out;
  } else {
    return 0.0;
  }
}

void GraphicEq::ProcessBlock(std::span<const Real> input_data,
                             std::span<Real> output_data) noexcept {
  size_t num_samples = input_data.size();
  if (equal_) {
    for (size_t i = 0; i < (size_t)num_samples; i++) {
      output_data[i] = ProcessSample(input_data[i]);
    }
  } else if (IsEqual(current_gain_, target_gain_)) {
    current_gain_ = target_gain_;
    equal_ = true;
    UpdateParameters();
    for (size_t i = 0; i < (size_t)num_samples; i++)
      output_data[i] = ProcessSample(input_data[i]);
  } else {
    for (size_t i = 0; i < (size_t)num_samples; i++) {
      output_data[i] = ProcessSample(input_data[i]);
      // TODO: implement Lerp
      //      Lerp(current_gain_, target_gain_, lerpFactor);
      current_gain_ = target_gain_;
      UpdateParameters();
    }
  }
}

void GraphicEq::Reset() {
  // Implement me!
  ASSERT(false);
}

} // namespace dsp

} // namespace sal
