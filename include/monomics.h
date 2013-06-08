/*
 omnimicrophones.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAT_DIRECTIVITYFUNCTION_H
#define SAT_DIRECTIVITYFUNCTION_H

#include "mcl.h"
#include "point.h"
#include "satconstants.h"
#include "stream.h"
#include "microphone.h"

namespace sat {
  
class MonoMic : public Microphone {
public:
  MonoMic(Point position, Angle theta, Angle phi, Angle psi) :
          Microphone(position, theta, phi, psi) {}
  
  virtual void Tick() {
    stream_.Tick();
  }
  
  MonoStream* stream() { return &stream_; }
  
protected:
  MonoStream stream_;
};
  
class GainMic : public MonoMic {
public:
  GainMic(Point position, Angle theta, Angle phi, Angle psi,
          Sample gain) :
          MonoMic(position, theta, phi, psi),
          gain_(gain) {}
  
  virtual ~GainMic() {}
private:
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point&,
                                       const UInt&) {
    stream_.Add(sample*gain_);
  }
  
  //  virtual std::vector<Sample> ImpulseResponse(Point point) const {
  //    return mcl::UnaryVector<Sample>(gain_);
  //  }
  
  Sample gain_;
};
  

class OmniMic : public GainMic {
public:
  OmniMic(Point position, Angle theta, Angle phi, Angle psi) :
          GainMic(position, theta, phi, psi, (Sample) 1.0) {}
};
  

// This class describes directivity pattern whose expression is of the type:
// a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
// Note that such an expression is axisimmetric.
class TrigMic : public MonoMic {
public:
  TrigMic(Point position, Angle theta, Angle phi, Angle psi,
          std::vector<Sample> coefficients) :
          MonoMic(position, theta, phi, psi),
          coefficients_(coefficients) {}
  
  virtual ~TrigMic() {}
  
  private:
  Sample GetDirectivity(const Point& point) {
    Angle theta = point.theta();
    
    const UInt N = coefficients_.size();
    Sample directivity(coefficients_[0]);
    for (UInt i=1; i<N; ++i) { directivity += coefficients_[i]*pow(cos(theta),i); }
    return directivity;
  }
  
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt&) {
    stream_.Add(sample*GetDirectivity(point));
  }
  
  std::vector<Sample> coefficients_;
};
  
  




} // namespace sat
  
#endif



