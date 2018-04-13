/*
 omnimicrophones.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DIRECTIVITYFUNCTION_H
#define SAL_DIRECTIVITYFUNCTION_H

#include "point.h"
#include "salconstants.h"
#include "stream.h"
#include "microphone.h"



namespace sal {
  
class SAL_API MonoMic : public Microphone {
public:
  MonoMic(mcl::Point position, mcl::Quaternion orientation) :
          Microphone(position, orientation) {}
  
  virtual void Tick() noexcept {
    stream_.Tick();
  }
  
  virtual bool IsOmni() { return true; }
  
  MonoStream* stream() { return &stream_; }
  
protected:
  MonoStream stream_;
};
  
class SAL_API GainMic : public MonoMic {
public:
  GainMic(mcl::Point position, Sample gain) :
          MonoMic(position, mcl::Quaternion::Identity()),
          gain_(gain) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~GainMic() {}
private:
  
  virtual void RecordPlaneWaveRelative(const Signal& signal, const mcl::Point&,
                                       const UInt&) noexcept {
    stream_.Add(mcl::Multiply<sal::Sample>(signal, gain_));
  }
  
  Sample gain_;
};
  

class SAL_API OmniMic : public GainMic {
public:
  OmniMic(mcl::Point position) :
        GainMic(position, (Sample) 1.0) {}
  
  virtual bool IsCoincident() { return true; }
};
  

/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class SAL_API TrigMic : public MonoMic {
public:
  TrigMic(mcl::Point position, mcl::Quaternion orientation,
          std::vector<Sample> coefficients) :
          MonoMic(position, orientation),
          coefficients_(coefficients) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~TrigMic() {}
  
private:
  Sample GetDirectivity(const mcl::Point& point) {
    Angle phi = AngleBetweenPoints(point,
                                               mcl::Point(1.0, 0.0, 0.0));
    
    const UInt N = coefficients_.size();
    Sample directivity(coefficients_[0]);
    for (UInt i=1; i<N; ++i) { directivity += coefficients_[i]*pow(cos(phi),i); }
    return directivity;
  }
  
  virtual void RecordPlaneWaveRelative(const Signal& signal, const mcl::Point& point,
                                       const UInt&) noexcept {
    stream_.Add(mcl::Multiply<sal::Sample>(signal, GetDirectivity(point)));
  }
  
  std::vector<Sample> coefficients_;
};
  
  
/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class SAL_API TanMic : public MonoMic {
public:
  TanMic(mcl::Point position, mcl::Quaternion orientation,
         sal::Sample base_angle) :
  MonoMic(position, orientation),
  base_angle_(base_angle) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~TanMic() {}
  
private:
  Sample GetDirectivity(const mcl::Point& point) {
    Angle phi = AngleBetweenPoints(point,
                                               mcl::Point(1.0, 0.0, 0.0));
    
    sal::Angle phi_l = 0;
    sal::Angle phi_lp1 = base_angle_; //PI/3.0;
    
    sal::Sample directivity;
    if (phi < phi_lp1) {
      directivity =
      1.0/sqrt(1+pow(sin(phi-phi_l), 2.0)/pow(sin(phi_lp1-phi), 2.0));
    } else { directivity = 0.0; }
    
    return directivity;
  }
  
  virtual void RecordPlaneWaveRelative(const Signal& signal, const mcl::Point& point,
                                       const UInt&) noexcept {
    stream_.Add(mcl::Multiply<sal::Sample>(signal, GetDirectivity(point)));
  }
  
  sal::Sample base_angle_;
};




} // namespace sal
  
#endif



