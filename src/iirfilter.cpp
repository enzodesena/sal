/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "iirfilter.h"
#include "comparisonop.h"
#include "vectorop.h"
#include "butter.h"
#include <limits>

namespace mcl {

void IirFilter::SetNumeratorCoefficient(const size_t coeff_id, const Real value) noexcept {
  // Store a normalised version
  B_.at(coeff_id) = value/A0_;
}

void IirFilter::SetDenominatorCoefficient(const size_t coeff_id, const Real value) noexcept {
  if (coeff_id == 0) {
    // Store a normalised version
    A0_ = value;
    A_[0] = value;
    B_ = Multiply(B_, 1.0/A0_);
    A_ = Multiply(A_, 1.0/A0_);
  } else {
    // Store a normalised version
    A_.at(coeff_id) = value/A0_;
  }
}

Real IirFilter::GetNumeratorCoefficient(const size_t coeff_id) const noexcept {
  // Return the non-normalised version
  return B_.at(coeff_id) * A0_;
}

Real IirFilter::GetDenominatorCoefficient(const size_t coeff_id) const noexcept {
  // Return the non-normalised version
  return A_.at(coeff_id) * A0_;
}

std::vector<Real> IirFilter::B() const {
  // Return the non-normalised version
  return Multiply(B_, A0_);
}

std::vector<Real> IirFilter::A() const {
  // Return the non-normalised version
  return Multiply(A_, A0_);
}


void IirFilter::SetCoefficients(const std::vector<Real>& B, const std::vector<Real>& A) noexcept {
  // TODO: implement case where length changes.
  ASSERT(B_.size() == B.size());
  ASSERT(A_.size() == A.size());

  for (size_t i=0; i < B.size(); ++i) {
    SetDenominatorCoefficient(i, A[i]);
    SetNumeratorCoefficient(i, B[i]);
  }
}

void IirFilter::SetCoefficients(const IirFilter& other_filter) noexcept {
  const size_t filter_order = order();
  assert(filter_order == other_filter.order());

  for (size_t i=0; i<=filter_order; ++i) {
    SetNumeratorCoefficient(i, other_filter.GetNumeratorCoefficient(i));
    SetDenominatorCoefficient(i, other_filter.GetDenominatorCoefficient(i));
  }
}

// Constructor
IirFilter::IirFilter() :
  B_(mcl::UnaryVector<Real>(1.0)), A_(mcl::UnaryVector<Real>(1.0)), A0_(1.0) {
  state_ = std::vector<Real>(B_.size(), 0.0);
}
  
IirFilter::IirFilter(std::vector<Real> B, std::vector<Real> A) : 
          B_(B), A_(A), A0_(1.0) {
  // TODO: implement also for B.size != A.size
  ASSERT(B.size() == A.size());
  ASSERT(B.size() >= 1);
            
  SetCoefficients(B, A);
  state_ = std::vector<Real>(B.size(), 0.0);
}


size_t IirFilter::order() const noexcept {
  return Max(B_.size(), A_.size())-1;
}


void IirFilter::ProcessBlock(const Real* input_data, const size_t num_samples,
                       Real* output_data) noexcept {
  if (B_.size() == 1) {
    Multiply(input_data, num_samples, B_[0], output_data);
  } else {
    ProcessBlockSerial(input_data, num_samples, output_data);
  }
}
  
  
Real IirFilter::ProcessSample(Real input) noexcept {
  Real v = input; // The temporary value in the recursive branch.
  Real output(0.0);
  
  size_t size = B_.size();
  
  // Speed up return for simple gain filters
  if (size == 1) { return v*B_[0]; }
  
  // The index i in both loops refers to the branch in the classic plot of a 
  // direct form II, with the highest branch (the one multiplied by b(0) only)
  // being i=0.
  for (size_t i=1; i<size; ++i) { 
    v += state_[i-1]*(-A_[i]);
    output += state_[i-1]*B_[i];
  }
  
  for (size_t i=(size-1); i>=1; --i) {
    state_[i] = state_[i-1];
  }
  
  state_[0] = v;
  
  output += v*B_[0];
  
  return output;
}


std::vector<Real> IirFilter::GetFrequencyResponse(const std::vector<Real>& frequencies,
                                                  const Real sampling_frequency) const {
  Real omega;
  Complex e;
  std::vector<Real> magnitudes(frequencies.size(), 0.0);
  for (size_t i = 0; i < frequencies.size(); i++) {
    omega = 2.0 * PI * frequencies[i] / sampling_frequency;
    Complex num = B_[0];
    Complex den = 1.0;

    for (size_t j = 1; j <= order(); j++) {
      e = std::exp(-((Real)j) * imaginary_unit * omega);
      num += B_[j] * e;
      den += A_[j] * e;
    }

    magnitudes[i] = 20 * std::log10(std::abs(num / den));
  }
  return magnitudes;
}
  
void IirFilter::Reset() {
  const size_t size = B_.size();
  for (size_t i=0; i<size; ++i) { state_[i] = 0.0; }
}



IirFilter GainFilter(Real gain) {
  std::vector<Real> B(1);
  std::vector<Real> A(1);
  B[0] = 1.0*gain;
  A[0] = 1.0;
  
  return IirFilter(B,A);
}

IirFilter IdenticalFilter() { return GainFilter(1.0); }

IirFilter WallFilter(WallType wall_type, Real sampling_frequency) {
  // TODO: implement for frequencies other than 44100
  if (! IsEqual(sampling_frequency, 44100) && wall_type != kRigid) {
    mcl::Logger::GetInstance().LogError("Attempting to use a wall filter "
        "designed for 44100 Hz sampling frequency with a sampling frequency "
        "of %f Hz. The filter response will be inaccurate.", sampling_frequency);
  }
  
  std::vector<Real> B;
  std::vector<Real> A;
  
  switch (wall_type) {
    case kRigid: {
      B.push_back(1.0);
      A.push_back(1.0);
      break;
    }
    case kCarpetPile: {
      // B_carpet_pile=[0.562666833756030  -1.032627191365576   0.469961155406544];
      // A_carpet_pile=[1.000000000000000  -1.896102349247713   0.896352947528892];
      B.push_back(0.562666833756030);
      B.push_back(-1.032627191365576);
      B.push_back(0.469961155406544);
      A.push_back(1.000000000000000);
      A.push_back(-1.896102349247713);
      A.push_back(0.896352947528892);
      break;
    }
    case kCarpetCotton: {
      // B_carpet_cotton = [0.687580695329600  -1.920746652319969   1.789915765926473  -0.556749690855965];
      // A_carpet_cotton = [1.000000000000000  -2.761840732459190   2.536820778736938  -0.774942833868750];
      B.push_back(0.687580695329600);
      B.push_back(-1.920746652319969);
      B.push_back(1.789915765926473);
      B.push_back(-0.556749690855965);
      A.push_back(1.000000000000000);
      A.push_back(-2.761840732459190);
      A.push_back(2.536820778736938);
      A.push_back(-0.774942833868750);
      break;
    }
    case kWallBricks: {
      // B_wall=[0.978495798553620  -1.817487798457697   0.839209660516074];
      // A_wall=[1.000000000000000  -1.858806492488240   0.859035906864860];
      B.push_back(0.978495798553620);
      B.push_back(-1.817487798457697);
      B.push_back(0.839209660516074);
      A.push_back(1.000000000000000);
      A.push_back(-1.858806492488240);
      A.push_back(0.859035906864860);
      break;
    }
    case kCeilingTile: {
      // B_ceiling=[0.168413736374283  -0.243270224986791   0.074863520490536];
      // A_ceiling=[1.000000000000000  -1.845049094190385   0.845565720138466];
      B.push_back(0.168413736374283);
      B.push_back(-0.243270224986791);
      B.push_back(0.074863520490536);
      A.push_back(1.000000000000000);
      A.push_back(-1.845049094190385);
      A.push_back(0.845565720138466);
      break;
    }
    default: {
      mcl::Logger::GetInstance().LogError("Unrecognised type of wall filter. Reverting to a completely absorptive one.");
      B.push_back(0.0);
      A.push_back(1.0);
      break;
    }
  }
  
  return IirFilter(B,A);
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
  
  return IirFilter(RealPart(num),RealPart(den));
}



//IirFilter PeakHighShelfFilter(const Real fc, const Real g, const Real Q, const int sample_rate) {
//  Real omega = 2.0 * PI * fc / ((Real) sample_rate);
//  Real cosOmega = cos(omega);
//  Real alpha = sin(omega) / Q;
//  Real Avalue = sqrt(g);
//  Real v1 = Avalue + 1.0;
//  Real v2 = Avalue - 1.0;
//  Real v3 = v1 * cosOmega;
//  Real v4 = v2 * cosOmega;
//  Real v5 = sqrt(Avalue) * alpha; // 2 * sqrt(A) * alpha
//
//  Real a0 = 1.0 / (v1 - v4 + v5); // a[0] isn't used in GetOutput
//  Real a1 = (2.0 * (v2 - v3)) * a0;
//  Real a2 = (v1 - v4 - v5) * a0;
//
//  Real b0 = Avalue * (v1 + v4 + v5) * a0;
//  Real b1 = -2.0 * Avalue * (v2 + v3) * a0;
//  Real b2 = Avalue * (v1 + v4 - v5) * a0;
//  
//  std::vector<Real> A = {1.0, a1, a2};
//  std::vector<Real> B = {b0, b1, b2};
//  return IirFilter(B, A);
//}
//
//IirFilter PeakLowShelfFilter(const Real fc, const Real g, const Real Q, const int sample_rate) {
//  Real omega = 2.0 * PI * fc / sample_rate;
//  Real cosOmega = cos(omega);
//  Real alpha = sin(omega) / Q; // sin(omega) / (2 * Q) (factor of two cancelled out)
//  
//  Real Avalue = sqrt(g);
//  Real v1 = Avalue + 1.0;
//  Real v2 = Avalue - 1.0;
//  Real v3 = v1 * cosOmega;
//  Real v4 = v2 * cosOmega;
//  Real v5 = sqrt(Avalue) * alpha; // 2 * sqrt(A) * alpha
//
//  Real a0 = 1.0 / (v1 + v4 + v5); // a[0] isn't used in GetOutput
//  Real a1 = (-2.0 * (v2 + v3)) * a0;
//  Real a2 = (v1 + v4 - v5) * a0;
//
//  Real b0 = Avalue * (v1 - v4 + v5) * a0;
//  Real b1 = 2.0 * Avalue * (v2 - v3) * a0;
//  Real b2 = Avalue * (v1 - v4 - v5) * a0;
//  
//  std::vector<Real> A = {1.0, a1, a2};
//  std::vector<Real> B = {b0, b1, b2};
//  return IirFilter(B, A);
//}
//
//IirFilter PeakingFilter(const Real fc, const Real g, const Real Q, const int sample_rate) {
//  Real omega = 2.0 * PI * fc / ((Real) sample_rate);
//  Real cosOmega = -2.0 * cos(omega);
//  Real alpha = sin(omega) / (2.0 * Q);
//  Real Avalue = sqrt(g);
//  Real v1 = alpha * Avalue;
//  Real v2 = alpha / Avalue;
//  
//  Real a0 = 1.0 / (1 + v2); // a[0] isn't used in GetOutput
//  Real a1 = cosOmega * a0;
//  Real a2 = (1 - v2) * a0;
//
//  Real b0 = (1 + v1) * a0;
//  Real b1 = a1;
//  Real b2 = (1 - v1) * a0;
//  
//  std::vector<Real> A = {1.0, a1, a2};
//  std::vector<Real> B = {b0, b1, b2};
//  return IirFilter(B, A);
//}
//
//
//
//IirFilter GraphicEqFilter(const std::vector<Real>& gain, const std::vector<Real>& fc, const Real Q, const Real sampling_frequency) {
//  
//  const size_t num_filters = fc.size() + 2;
//  const Real peak_low_shelf_fc = (fc[0] / 2.0)* sqrt(fc[0] / (fc[0] / 2.0));
//  const Real peak_high_shelf_fc = fc[num_filters - 3] * sqrt((2.0 * fc[num_filters - 3]) / fc[num_filters - 3]);
//  const Real EPS = 0.0000001;
//  
//  Matrix<Real> mat(num_filters, num_filters);
//  std::vector<Real> input_gain(num_filters, 0.0);
//  std::vector<Real> target_gain(num_filters + 1, 0.0);
//  std::vector<Real> current_gain(num_filters + 1, 0.0);
//  std::vector<Real> db_gain(num_filters, 0.0);
//  
//  std::vector<Real> f = std::vector<Real>(num_filters, 0.0);
//
//  f[0] = fc[0] / 2.0;
//  for (size_t i = 1; i < num_filters - 1; i++)
//    f[i] = fc[i - 1];
//  f[num_filters - 1] = 2.0 * fc[num_filters - 3];
//
//  
//  Real pdb = 6.0;
//  Real p = pow(10.0, pdb / 20.0);
//
//  
//  // Calculate matrix
//  std::vector<Real> out = std::vector<Real>(f.size(), 0.0);
//  int j = 0;
//  
//  IirFilter temp_filter = PeakLowShelfFilter(peak_low_shelf_fc, p, Q, sampling_frequency);
//  out = temp_filter.GetFrequencyResponse(f, sampling_frequency);
//
//  for (size_t i = 0; i < out.size(); i++) {
//    mat.SetElement(j, i, mat.GetElement(j, i)+out[i]);
//  }
//    
//  j++;
//
//  for (size_t i = 0; i < num_filters - 2; i++) {
//    IirFilter temp_filter = PeakingFilter(fc[i], p, Q, sampling_frequency);
//    
//    out = temp_filter.GetFrequencyResponse(f, sampling_frequency);
//
//    for (size_t i = 0; i < out.size(); i++)
//      mat.SetElement(j, i, mat.GetElement(j, i)+out[i]);
//    j++;
//  }
//
//  IirFilter temp_filter_high = PeakHighShelfFilter(peak_high_shelf_fc, p, Q, sampling_frequency);
//  out = temp_filter_high.GetFrequencyResponse(f, sampling_frequency);
//
//  for (size_t i = 0; i < out.size(); i++) {
//    mat.SetElement(j, i, mat.GetElement(j, i)+out[i]);
//  }
//  
//  mat = Inverse(mat);
//  mat = Multiply(mat, pdb);
//  
//  
//  // when dB is used here. Factors of 20 are cancelled out.
//  Real g = (gain[0] + gain[1]) / 2.0;
//  db_gain[0] = std::max(g, EPS); // Prevent log10(0)
//
//  for (size_t i = 1; i < num_filters - 1; i++)
//    db_gain[i] = std::max(gain[i - 1], EPS); // Prevent log10(0)
//
//  g = (gain[num_filters - 4] + gain[num_filters - 3]) / 2.0;
//  db_gain[num_filters - 1] = std::max(g, EPS); // Prevent log10(0)
//
//  std::transform(db_gain.begin(), db_gain.end(), db_gain.begin(), [](double x) { return std::log10(x); });
//  Real mean_db_gain = Sum(db_gain) / ((Real)db_gain.size());
//  target_gain[0] = mcl::Pow(10.0, mean_db_gain); // 10 ^ mean(db_gain);
//  std::transform(db_gain.begin(), db_gain.end(), db_gain.begin(), [mean_db_gain](double x) { return x - mean_db_gain; }); // db_gain - mean(db_gain);
//
//  input_gain = mcl::Multiply(db_gain, mat);
//  input_gain = mcl::Pow(10.0, input_gain);
//  
//  for (size_t i = 0; i < num_filters; i++)
//    target_gain[i + 1] = input_gain[i];
//  
//  // Create filters
//  std::vector<IirFilter> all_filters;
//  all_filters.push_back(GainFilter(target_gain[0]));
//  all_filters.push_back(PeakLowShelfFilter(peak_low_shelf_fc, target_gain[1], Q, sampling_frequency));
//  for (size_t i = 0; i < fc.size(); i++) {
//    all_filters.push_back(PeakingFilter(fc[i], target_gain[i+2], Q, sampling_frequency));
//  }
//  all_filters.push_back(PeakHighShelfFilter(peak_high_shelf_fc, target_gain[num_filters], Q, sampling_frequency));
//  
//  return SeriesFilter(all_filters);
//}



IirFilter SeriesFilter(const IirFilter& filter_a, const IirFilter& filter_b) {
  return IirFilter(Conv(filter_a.B(), filter_b.B()), Conv(filter_a.A(), filter_b.A()));
}

IirFilter SeriesFilter(const std::vector<IirFilter>& filters) {
  IirFilter output(filters[0]);
  for (size_t i=1; i < filters.size(); ++i) {
    output = SeriesFilter(output, filters[i]);
  }
  return output;
}




  
std::vector<Real> IirFilterBank::ProcessSample(const Real input) {
  const size_t N = filters_.size();
  std::vector<Real> outputs(N);
  for (size_t i=0; i<N; ++i) {
    outputs[i] = filters_[i].ProcessSample(input);
  }
  return outputs;
}

  
std::vector<std::vector<Real> >
IirFilterBank::ProcessBlock(const std::vector<Real>& input) {
  const size_t N = filters_.size();
  std::vector<std::vector<Real> > outputs(N);
  for (size_t i=0; i<N; ++i) {
    outputs[i] = filters_[i].ProcessBlock(input);
  }
  return outputs;
}

  
void IirFilterBank::Reset() {
  const size_t N = filters_.size();
  for (size_t i=0; i<N; ++i) {
    filters_[i].Reset();
  }
}

  
} // namespace mcl




