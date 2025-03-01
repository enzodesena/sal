/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <limits>

#include "butter.h"
#include "comparisonop.h"
#include "iirfilter.h"
#include "vectorop.h"

namespace sal {

namespace dsp {

void IirFilter::SetNumeratorCoefficient(const size_t coeff_id,
                                        const Real value) noexcept {
  // Store a normalised version
  numerator_coeffs_.at(coeff_id) = value / denominator_coeff_0_;
}

void IirFilter::SetDenominatorCoefficient(const size_t coeff_id,
                                          const Real value) noexcept {
  if (coeff_id == 0) {
    // Store a normalised version
    denominator_coeff_0_ = value;
    denominator_coeffs_[0] = value;
    numerator_coeffs_ = Multiply(numerator_coeffs_, 1.0 / denominator_coeff_0_);
    denominator_coeffs_ = Multiply(denominator_coeffs_, 1.0 / denominator_coeff_0_);
  } else {
    // Store a normalised version
    denominator_coeffs_.at(coeff_id) = value / denominator_coeff_0_;
  }
}

Real IirFilter::GetNumeratorCoefficient(const size_t coeff_id) const noexcept {
  // Return the non-normalised version
  return numerator_coeffs_.at(coeff_id) * denominator_coeff_0_;
}

Real IirFilter::GetDenominatorCoefficient(
    const size_t coeff_id) const noexcept {
  // Return the non-normalised version
  return denominator_coeffs_.at(coeff_id) * denominator_coeff_0_;
}

std::vector<Real> IirFilter::numerator_coeffs() const {
  // Return the non-normalised version
  return Multiply(numerator_coeffs_, denominator_coeff_0_);
}

std::vector<Real> IirFilter::denominator_coeffs() const {
  // Return the non-normalised version
  return Multiply(denominator_coeffs_, denominator_coeff_0_);
}

void IirFilter::SetCoefficients(const std::vector<Real>& B,
                                const std::vector<Real>& A) noexcept {
  // TODO: implement case where length changes.
  ASSERT(numerator_coeffs_.size() == B.size());
  ASSERT(denominator_coeffs_.size() == A.size());

  for (size_t i = 0; i < B.size(); ++i) {
    SetDenominatorCoefficient(i, A[i]);
    SetNumeratorCoefficient(i, B[i]);
  }
}

void IirFilter::SetCoefficients(const IirFilter& other_filter) noexcept {
  const size_t filter_order = order();
  assert(filter_order == other_filter.order());

  for (size_t i = 0; i <= filter_order; ++i) {
    SetNumeratorCoefficient(i, other_filter.GetNumeratorCoefficient(i));
    SetDenominatorCoefficient(i, other_filter.GetDenominatorCoefficient(i));
  }
}

// Constructor
IirFilter::IirFilter()
    : numerator_coeffs_(dsp::UnaryVector<Real>(1.0)),
      denominator_coeffs_(dsp::UnaryVector<Real>(1.0)),
      denominator_coeff_0_(1.0) {
  state_ = std::vector<Real>(numerator_coeffs_.size(), 0.0);
}

IirFilter::IirFilter(std::vector<Real> B, std::vector<Real> A)
    : numerator_coeffs_(B), denominator_coeffs_(A), denominator_coeff_0_(1.0) {
  // TODO: implement also for B.size != A.size
  ASSERT(B.size() == A.size());
  ASSERT(B.size() >= 1);

  SetCoefficients(B, A);
  state_ = std::vector<Real>(B.size(), 0.0);
}

size_t IirFilter::order() const noexcept {
  return Max(numerator_coeffs_.size(), denominator_coeffs_.size()) - 1;
}

void IirFilter::ProcessBlock(std::span<const Real> input_data,
                             std::span<Real> output_data) noexcept {
  if (numerator_coeffs_.size() == 1) {
    Multiply(input_data, numerator_coeffs_[0], output_data);
  } else {
    ProcessBlockSerial(input_data, output_data);
  }
}

Real IirFilter::ProcessSample(Real input) noexcept {
  Real v = input;  // The temporary value in the recursive branch.
  Real output(0.0);

  size_t size = numerator_coeffs_.size();

  // Speed up return for simple gain filters
  if (size == 1) {
    return v * numerator_coeffs_[0];
  }

  // The index i in both loops refers to the branch in the classic plot of a
  // direct form II, with the highest branch (the one multiplied by b(0) only)
  // being i=0.
  for (size_t i = 1; i < size; ++i) {
    v += state_[i - 1] * (-denominator_coeffs_[i]);
    output += state_[i - 1] * numerator_coeffs_[i];
  }

  for (size_t i = (size - 1); i >= 1; --i) {
    state_[i] = state_[i - 1];
  }

  state_[0] = v;

  output += v * numerator_coeffs_[0];

  return output;
}

std::vector<Real> IirFilter::GetFrequencyResponse(
    const std::vector<Real>& frequencies, const Real sampling_frequency) const {
  Real omega;
  Complex e;
  std::vector<Real> magnitudes(frequencies.size(), 0.0);
  for (size_t i = 0; i < frequencies.size(); i++) {
    omega = 2.0 * PI * frequencies[i] / sampling_frequency;
    Complex num = numerator_coeffs_[0];
    Complex den = 1.0;

    for (size_t j = 1; j <= order(); j++) {
      e = std::exp(-((Real)j) * imaginary_unit * omega);
      num += numerator_coeffs_[j] * e;
      den += denominator_coeffs_[j] * e;
    }

    magnitudes[i] = 20 * std::log10(std::abs(num / den));
  }
  return magnitudes;
}

void IirFilter::Reset() {
  const size_t size = numerator_coeffs_.size();
  for (size_t i = 0; i < size; ++i) {
    state_[i] = 0.0;
  }
}

void IirFilter::ProcessBlockSerial(std::span<const Real> input_data,
                                   std::span<Real> output_data) noexcept {
  for (size_t i = 0; i < input_data.size(); ++i) {
    output_data[i] = ProcessSample(input_data[i]);
  }
}

IirFilter GainFilter(Real gain) {
  std::vector<Real> B(1);
  std::vector<Real> A(1);
  B[0] = 1.0 * gain;
  A[0] = 1.0;

  return IirFilter(B, A);
}

IirFilter IdenticalFilter() { return GainFilter(1.0); }

IirFilter WallFilter(WallType wall_type, Real sampling_frequency, Real gain) {
  // TODO: implement for frequencies other than 44100
  if (!IsEqual(sampling_frequency, 44100) && wall_type != kRigid) {
    dsp::Logger::GetInstance().LogError(
        "Attempting to use a wall filter "
        "designed for 44100 Hz sampling frequency with a sampling frequency "
        "of %f Hz. The filter response will be inaccurate.",
        sampling_frequency);
  }

  std::vector<Real> B;
  std::vector<Real> A;

  switch (wall_type) {
    case kRigid: {
      B.push_back(gain);
      A.push_back(1.0);
      break;
    }
    case kCarpetPile: {
      // B_carpet_pile=[0.562666833756030  -1.032627191365576
      // 0.469961155406544]; A_carpet_pile=[1.000000000000000 -1.896102349247713
      // 0.896352947528892];
      B.push_back(0.562666833756030*gain);
      B.push_back(-1.032627191365576*gain);
      B.push_back(0.469961155406544*gain);
      A.push_back(1.000000000000000);
      A.push_back(-1.896102349247713);
      A.push_back(0.896352947528892);
      break;
    }
    case kCarpetCotton: {
      // B_carpet_cotton = [0.687580695329600
      // -1.920746652319969   1.789915765926473  -0.556749690855965];
      // A_carpet_cotton = [1.000000000000000
      // -2.761840732459190   2.536820778736938  -0.774942833868750];
      B.push_back(0.687580695329600*gain);
      B.push_back(-1.920746652319969*gain);
      B.push_back(1.789915765926473*gain);
      B.push_back(-0.556749690855965*gain);
      A.push_back(1.000000000000000);
      A.push_back(-2.761840732459190);
      A.push_back(2.536820778736938);
      A.push_back(-0.774942833868750);
      break;
    }
    case kWallBricks: {
      // B_wall=[0.978495798553620  -1.817487798457697   0.839209660516074];
      // A_wall=[1.000000000000000  -1.858806492488240   0.859035906864860];
      B.push_back(0.978495798553620*gain);
      B.push_back(-1.817487798457697*gain);
      B.push_back(0.839209660516074*gain);
      A.push_back(1.000000000000000);
      A.push_back(-1.858806492488240);
      A.push_back(0.859035906864860);
      break;
    }
    case kCeilingTile: {
      // B_ceiling=[0.168413736374283  -0.243270224986791   0.074863520490536];
      // A_ceiling=[1.000000000000000  -1.845049094190385   0.845565720138466];
      B.push_back(0.168413736374283*gain);
      B.push_back(-0.243270224986791*gain);
      B.push_back(0.074863520490536*gain);
      A.push_back(1.000000000000000);
      A.push_back(-1.845049094190385);
      A.push_back(0.845565720138466);
      break;
    }
    default: {
      dsp::Logger::GetInstance().LogError(
          "Unrecognised type of wall filter. Reverting to a completely "
          "absorptive one.");
      B.push_back(0.0);
      A.push_back(1.0);
      break;
    }
  }

  return IirFilter(B, A);
}

IirFilter PinkifierFilter() {
  std::vector<Real> poles(5);
  poles[0] = 0.9986823;
  poles[1] = 0.9914651;
  poles[2] = 0.9580812;
  poles[3] = 0.8090598;
  poles[4] = 0.2896591;
  std::vector<Real> zeros(5);
  zeros[0] = 0.9963594;
  zeros[1] = 0.9808756;
  zeros[2] = 0.9097290;
  zeros[3] = 0.6128445;
  zeros[4] = -0.0324723;

  std::vector<Complex> num = Poly(zeros);
  std::vector<Complex> den = Poly(poles);

  return IirFilter(RealPart(num), RealPart(den));
}

IirFilter SeriesFilter(const IirFilter& filter_a, const IirFilter& filter_b) {
  return IirFilter(Conv(filter_a.numerator_coeffs(), filter_b.numerator_coeffs()),
                   Conv(filter_a.denominator_coeffs(), filter_b.denominator_coeffs()));
}

IirFilter SeriesFilter(const std::vector<IirFilter>& filters) {
  IirFilter output(filters[0]);
  for (size_t i = 1; i < filters.size(); ++i) {
    output = SeriesFilter(output, filters[i]);
  }
  return output;
}

void IirFilterBank::ProcessSample(const Real input,
                                  std::span<Real> output_data) {
  const size_t N = filters_.size();
  for (size_t i = 0; i < N; ++i) {
    output_data[i] = filters_[i].ProcessSample(input);
  }
}

void IirFilterBank::Reset() {
  const size_t N = filters_.size();
  for (size_t i = 0; i < N; ++i) {
    filters_[i].Reset();
  }
}

} // namespace dsp

} // namespace sal
