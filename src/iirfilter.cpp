/*
 iirfilter.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "iirfilter.h"
#include "comparisonop.h"
#include "vectorop.h"
#include "butter.h"
#include <limits>

namespace mcl {
  
std::vector<Real> IirFilter::B() const {
  // Return the non-normalised version
  return Multiply(B_, A0_);
}

std::vector<Real> IirFilter::A() const {
  // Return the non-normalised version
  return Multiply(A_, A0_);
}

// Constructor
IirFilter::IirFilter() :
  B_(mcl::UnaryVector<Real>(1.0)), A_(mcl::UnaryVector<Real>(1.0)) {
  UInt size = B_.size();
  state_ = new Real[size];
  for (int i=0; i<size; ++i) { state_[i] = 0.0; }
}
  
IirFilter::IirFilter(std::vector<Real> B, std::vector<Real> A) : 
          B_(B), A_(A) {
  // TODO: implement also for B.size != A.size
  if (B.size() != A.size()) { throw_line(); }
  if (B.size() < 1) { throw_line(); }
  
  A0_ = A[0];
  if (! IsEqual(A[0], 1.0, std::numeric_limits<Real>::epsilon())) {
    B_ = Multiply(B, (Real) 1.0 / A[0]);
    A_ = Multiply(A, (Real) 1.0 / A[0]);
  }
            
            
  UInt size = B.size();
  state_ = new Real[size];
  for (int i=0; i<size; ++i) { state_[i] = 0.0; }
}

// Copy constructor
IirFilter::IirFilter(const IirFilter& copy) :
          B_(copy.B_), A_(copy.A_), A0_(copy.A0_) {
  UInt size = B_.size();
  state_ = new Real[size];
  for (int i=0; i<size; ++i) {
    state_[i] = copy.state_[i];
  }
}

// Assignment constructor
IirFilter& IirFilter::operator= (const IirFilter& other) {
  if (this != &other) {
    UInt size = other.B_.size();
    delete [] state_;
    state_ = new Real[size];
    for (int i=0; i<size; ++i) { state_[i] = other.state_[i]; }
    B_ = other.B_;
    A_ = other.A_;
    A0_ = other.A0_;
  }
  return *this;
}

IirFilter::~IirFilter() { delete [] state_; }

void IirFilter::UpdateFilter(std::vector<Real> B,
                             std::vector<Real> A) {
  // TODO: implement case where length changes.
  if (B_.size() != B.size()) { throw_line(); }
  if (A_.size() != A.size()) { throw_line(); }
  
  B_ = B;
  A_ = A;
}


Real IirFilter::Filter(Real input) {
  Real v = input; // The temporary value in the recursive branch.
  Real output(0.0);
  
  UInt size = B_.size();
  
  // Speed up return for simple gain filters
  if (size == 1) { return v*B_[0]; }
  
  // The index i in both loops refers to the branch in the classic plot of a 
  // direct form II, with the highest branch (the one multiplied by b(0) only)
  // being i=0.
  for (int i=1; i<size; ++i) { 
    v += state_[i-1]*(-A_[i]);
    output += state_[i-1]*B_[i];
  }
  
  for (Int i=(size-1); i>=1; --i) {
    state_[i] = state_[i-1];
  }
  
  state_[0] = v;
  
  output += v*B_[0];
  
  return output;
}
  
void IirFilter::Reset() {
  const UInt size = B_.size();
  for (int i=0; i<size; ++i) { state_[i] = 0.0; }
}


std::vector<Real> IirFilter::Filter(const std::vector<Real>& input) {
  
  std::vector<Real> output(input.size());
  
  for (UInt i=0; i<input.size(); ++i) {
    output[i] = this->Filter(input[i]);
  }
  
  return output;
}

IirFilter IirFilter::GainFilter(Real gain) {
  std::vector<Real> B(1);
  std::vector<Real> A(1);
  B[0] = 1.0*gain;
  A[0] = 1.0;
  
  return IirFilter(B,A);
}

IirFilter IirFilter::IdenticalFilter() { return IirFilter::GainFilter(1.0); }

IirFilter IirFilter::WallFilter(WallType wall_type, Real sampling_frequency) {
  // TODO: implement for frequencies other than 44100
  if (! IsEqual(sampling_frequency, 44100)) { throw_line(); }
  
  std::vector<Real> B;
  std::vector<Real> A;
  
  switch (wall_type) {
    case carpet_pile:
      // B_carpet_pile=[0.562666833756030  -1.032627191365576   0.469961155406544];
      // A_carpet_pile=[1.000000000000000  -1.896102349247713   0.896352947528892];
      B.push_back(0.562666833756030);
      B.push_back(-1.032627191365576);
      B.push_back(0.469961155406544);
      A.push_back(1.000000000000000);
      A.push_back(-1.896102349247713);
      A.push_back(0.896352947528892);
      break;
      
    case carpet_cotton:
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
      
    case wall_bricks:
      // B_wall=[0.978495798553620  -1.817487798457697   0.839209660516074];
      // A_wall=[1.000000000000000  -1.858806492488240   0.859035906864860];
      B.push_back(0.978495798553620);
      B.push_back(-1.817487798457697);
      B.push_back(0.839209660516074);
      A.push_back(1.000000000000000);
      A.push_back(-1.858806492488240);
      A.push_back(0.859035906864860);
      break;
      
    case ceiling_tile:
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
  
  return IirFilter(B,A);
}

IirFilter IirFilter::PinkifierFilter() {
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
  
IirFilter IirFilter::Butter(const UInt order,
                            const Real w_low, const Real w_high) {
  std::vector<double> DenC = ComputeDenCoeffs((int) order, w_low, w_high);
  std::vector<double> NumC = ComputeNumCoeffs((int) order, w_low, w_high, DenC);
  std::vector<Real> denominator(DenC.begin(), DenC.end());
  std::vector<Real> numerator(NumC.begin(), NumC.end());
  
  return IirFilter(numerator, denominator);
}

IirFilter IirFilter::OctaveFilter(const UInt order,
                                  const Real center_frequency,
                                  const Real sampling_frequency) {
  // beta = pi/2/N/sin(pi/2/N);
  Real beta = PI/2.0/((Real) order)/sin(PI/2.0/((Real) order));
  
  // alpha = (1+sqrt(1+8*beta^2))/4/beta;
  Real alpha = (1.0+sqrt(1.0+8.0*pow(beta,2.0)))/4.0/beta;
  
  // W1 = Fc/(Fs/2)*sqrt(1/2)/alpha;
  Real W1 = center_frequency/(sampling_frequency/2.0)*sqrt(1.0/2.0)/alpha;
  
  // W2 = Fc/(Fs/2)*sqrt(2)*alpha;
  Real W2 = center_frequency/(sampling_frequency/2.0)*sqrt(2.0)*alpha;
  
  // [B,A] = butter(N,[W1,W2]);
  return IirFilter::Butter(order, W1, W2);
}

  
std::vector<Real> IirFilterBank::Filter(const Real input) {
  const UInt N = filters_.size();
  std::vector<Real> outputs(N);
  for (UInt i=0; i<N; ++i) {
    outputs[i] = filters_[i].Filter(input);
  }
  return outputs;
}

  
std::vector<std::vector<Real> >
IirFilterBank::Filter(const std::vector<Real>& input) {
  const UInt N = filters_.size();
  std::vector<std::vector<Real> > outputs(N);
  for (UInt i=0; i<N; ++i) {
    outputs[i] = filters_[i].Filter(input);
  }
  return outputs;
}

  
void IirFilterBank::Reset() {
  const UInt N = filters_.size();
  for (UInt i=0; i<N; ++i) {
    filters_[i].Reset();
  }
}

IirFilterBank IirFilterBank::OctaveFilterBank(const UInt order,
                                              const UInt num_bands,
                                              const Real starting_frequency,
                                              const Real sampling_frequency) {
  Real current_frequency = starting_frequency;
  std::vector<IirFilter> filters;
  for (UInt i=0; i<num_bands; ++i) {
    
    mcl::IirFilter
    octave_filter = mcl::IirFilter::OctaveFilter(order,
                                                 current_frequency,
                                                 sampling_frequency);
    filters.push_back(octave_filter);
    current_frequency = current_frequency * 2.0;
  }
  return IirFilterBank(filters);
}
  
} // namespace mcl




