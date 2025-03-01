/*
 ShSource.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2025, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <string.h>

#include "ShSource.h"
#include "point.h"
#include "salconstants.h"

using sal::dsp::Point;
using sal::dsp::Quaternion;

namespace sal {

ShSource::ShSource(const Point& position,
                   const Quaternion orientation,
                   const std::vector<dsp::Real> centre_frequencies,
                   const std::vector<std::vector<dsp::Complex>> sh_coefficients,
                   const dsp::Real sampling_frequency) noexcept
    : Source(position, orientation),
      sh_coefficients_(sh_coefficients),
      sampling_frequency_(sampling_frequency) {
  for (int i = 0; i < centre_frequencies.size() - 1; ++i) {
    centre_frequencies_.push_back(centre_frequencies[i] * sqrt(centre_frequencies[i + 1] / centre_frequencies[i]));
  }
  
  for (int i = 0; i < sh_coefficients_.size(); ++i) { // TODO: understand why this is done
    sh_coefficients_[i][0] = sh_coefficients_[i][0] * dsp::SphericalHarmonic(0, 0, 0.0, 0.0);
  }
}

void ShSource::ResetFiltersState() noexcept {
  for (auto iterator = instances_.begin(); iterator != instances_.end();
       ++iterator) {
    iterator->second.filter_.ResetState();
  }
}

dsp::Real ShSource::GetTransferFunction(const dsp::Real f, const dsp::Point& point) const {
  // We need to convert to the front-pole coordinate system of the BRAS dataset // TODO: make this generic to other datasets
  dsp::Real theta = std::acos(point.x() / point.norm()); // angle between the direction and the x axis
  dsp::Real phi = std::atan2(point.y(), point.z()); // angle around the x axis
  
  int freq_index = static_cast<int>(std::upper_bound(centre_frequencies_.begin(), centre_frequencies_.end(), f) - centre_frequencies_.begin());
  dsp::Complex output = sh_coefficients_[freq_index][0];
  int len = std::sqrt(sh_coefficients_[freq_index].size());
  for (int l = 1; l < len; ++l) {
    for (int m = -l; m <= l; ++m) {
      dsp::Complex sh = dsp::SphericalHarmonic(l, m, theta, phi);
      int sh_index = l * l + l + m;
      output += sh_coefficients_[freq_index][sh_index] * sh;
    }
  }
  return output.real();
}


std::vector<dsp::Real> ShSource::GetTransferFunction(const std::vector<dsp::Real>& frequencies, const dsp::Point& point) const {
  std::vector<dsp::Real> output(frequencies.size());
  for (int i = 0; i < frequencies.size(); ++i) {
    output[i] = GetTransferFunction(frequencies[i], point);
  }
  return output;
}

void ShSource::ProcessAudioRelative(std::span<const Sample> input_data,
                                      const dsp::Point& observation_point_relative_to_source_look_direction,
                                      const size_t wave_id,
                                      std::span<Sample> output_data) noexcept {
  CreateInstanceIfNotExist(wave_id);
  instances_.at(wave_id).ProcessAudioRelative(input_data,
                                              observation_point_relative_to_source_look_direction,
                                              output_data);
}

void ShSource::CreateInstanceIfNotExist(const size_t wave_id) noexcept {
  // If there is no instance associated to the given wave_id then create
  if (instances_.count(wave_id) == 0) {
    instances_.insert(std::make_pair(wave_id, ShSourceInstance(this)));
  }
}


void ShSourceInstance::ProcessAudioRelative(std::span<const Sample> input_data,
                                              const dsp::Point& observation_point_relative_to_source_look_direction,
                                              std::span<Sample> output_data) noexcept {
  UpdateFilter(observation_point_relative_to_source_look_direction);
  filter_.ProcessBlock(input_data, output_data);
}

void ShSourceInstance::UpdateFilter(const dsp::Point& point) noexcept {
  if (!IsEqual(point, previous_point_)) {
    if (std::isnan(previous_point_.x())) {
      filter_.InitParameters(base_source_->GetGains(point));
    } else {
      filter_.SetGain(base_source_->GetGains(point)); // TODO: I don't know how fast this updates the filter. Create filter variable to handle this explicitly.
    }
    
    // Update cache variables
    previous_point_ = point;
  }
}

std::vector<dsp::Real> ShSource::GetGains(const dsp::Point& point) noexcept {
  std::vector<dsp::Real> directivity = GetTransferFunction(centre_frequencies_, point);
  return directivity;
}

dsp::GraphicEq ShSource::GetGraphicEq() noexcept {
  return dsp::GraphicEq(GetGains(Point(1, 0, 0)), centre_frequencies_, 1.0, sampling_frequency_);
}


}  // namespace sal
