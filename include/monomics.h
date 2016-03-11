/*
 omnimicrophones.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DIRECTIVITYFUNCTION_H
#define SAL_DIRECTIVITYFUNCTION_H

#include "mcl.h"
#include "point.h"
#include "salconstants.h"
#include "stream.h"
#include "microphone.h"

namespace sal {
  
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
  
  virtual bool IsCoincident() { return true; }
  virtual bool IsFrameEnabled() { return true; }
  
  virtual ~GainMic() {}
private:
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point&,
                                       const UInt&) {
    stream_.Add(sample*gain_);
  }
  
  virtual void RecordPlaneWaveRelative(const Signal& signal, const Point&,
                                       const UInt&) {
    stream_.Add(mcl::Multiply<sal::Sample>(signal, gain_));
  }
  
  Sample gain_;
};
  

class OmniMic : public GainMic {
public:
  OmniMic(Point position, Angle theta, Angle phi, Angle psi) :
          GainMic(position, theta, phi, psi, (Sample) 1.0) {}
  
  virtual bool IsCoincident() { return true; }
};
  

/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TrigMic : public MonoMic {
public:
  TrigMic(Point position, Angle theta, Angle phi, Angle psi,
          std::vector<Sample> coefficients) :
          MonoMic(position, theta, phi, psi),
          coefficients_(coefficients) {}
  
  virtual bool IsCoincident() { return true; }
  
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
  
  
/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class TanMic : public MonoMic {
public:
  TanMic(Point position, Angle theta, Angle phi, Angle psi,
         sal::Sample base_angle) :
  MonoMic(position, theta, phi, psi),
  base_angle_(base_angle) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~TanMic() {}
  
private:
  Sample GetDirectivity(const Point& point) {
    Angle theta = point.theta();
    
    sal::Angle phi_l = 0;
    sal::Angle phi_lp1 = base_angle_; //PI/3.0;
    
    sal::Sample directivity;
    if (theta < phi_lp1) {
      directivity =
      1.0/sqrt(1+pow(sin(theta-phi_l), 2.0)/pow(sin(phi_lp1-theta), 2.0));
    } else {
      directivity = 0.0;
    }
    
    return directivity;
  }
  
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt&) {
    stream_.Add(sample*GetDirectivity(point));
  }
  
  sal::Sample base_angle_;
};




} // namespace sal
  
#endif



