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
  
class SAL_API MonoMic : virtual public Microphone {
public:
  MonoMic(mcl::Point position, mcl::Quaternion orientation) :
          Microphone(position, orientation) {}
  
  /** This method is used mostly for testing, as it is very slow.
   You should use AddPlaneWave instead. */
  Sample RecordPlaneWave(const Sample input_sample,
                         const mcl::Point& point,
                         const Int& wave_id = 0) noexcept {
    MonoBuffer output_buffer(1);
    AddPlaneWave(&input_sample, 1, point, wave_id, output_buffer);
    return output_buffer.sample(0);
  }

  virtual ~MonoMic() {}
};
  
  
class SAL_API MemorylessMic : virtual public Microphone {
public:
  MemorylessMic(mcl::Point position, mcl::Quaternion orientation) :
      Microphone(position, orientation) {}
  
  virtual ~MemorylessMic() {}
private:
  virtual Sample GetDirectivity(const mcl::Point& point) = 0;
};
  
  
class SAL_API MemorylessMonoMic : public MemorylessMic, public MonoMic {
public:
  MemorylessMonoMic(mcl::Point position, mcl::Quaternion orientation) :
  MemorylessMic(position, orientation), MonoMic(position, orientation)  {}
  
  virtual void AddPlaneWaveRelative(const Sample* input_data,
                                    const Int num_samples,
                                    const mcl::Point& point,
                                    const Int wave_id,
                                    Buffer& output_buffer) noexcept {
    MonoBuffer& mono_buffer = dynamic_cast<MonoBuffer&>(output_buffer);
    assert(mono_buffer.num_channels() == 1);
    assert(num_samples <= mono_buffer.num_samples());
    
    mcl::MultiplyAdd(input_data,
                     GetDirectivity(point),
                     mono_buffer.GetReadPointer(),
                     num_samples,
                     mono_buffer.GetWritePointer());
  }
  
  virtual ~MemorylessMonoMic() {}
  
private:
  virtual Sample GetDirectivity(const mcl::Point& point) = 0;
};
  
  
class SAL_API GainMic : public MemorylessMonoMic {
public:
  GainMic(mcl::Point position, Sample gain) :
          MemorylessMonoMic(position, mcl::Quaternion::Identity()),
          Microphone(position, mcl::Quaternion::Identity()),
          gain_(gain) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~GainMic() {}
  
private:
  virtual Sample GetDirectivity(const mcl::Point& /* point */) {
    return gain_;
  }
  
  Sample gain_;
};
  

class SAL_API OmniMic : public GainMic {
public:
  OmniMic(mcl::Point position) :
        GainMic(position, (Sample) 1.0),
        Microphone(position, mcl::Quaternion::Identity()) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~OmniMic() {}
};
  

/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class SAL_API TrigMic : public MemorylessMonoMic {
public:
  TrigMic(mcl::Point position, mcl::Quaternion orientation,
          std::vector<Sample> coefficients) :
          MemorylessMonoMic(position, orientation),
          Microphone(position, orientation),
          coefficients_(coefficients) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~TrigMic() {}
private:
  virtual Sample GetDirectivity(const mcl::Point& point) {
    Angle phi = AngleBetweenPoints(point,
                                               mcl::Point(1.0, 0.0, 0.0));
    
    const Int N = coefficients_.size();
    Sample directivity(coefficients_[0]);
    for (UInt i=1; i<N; ++i) { directivity += coefficients_[i]*pow(cos(phi),i); }
    return directivity;
  }
  
  std::vector<Sample> coefficients_;
};
  
  
/**
 This class describes directivity pattern whose expression is of the type:
 a[0]+a[1]cos(theta)+a[2]cos^2(theta)+...
 Note that such an expression is axisimmetric.
 */
class SAL_API TanMic : public MemorylessMonoMic {
public:
  TanMic(mcl::Point position, mcl::Quaternion orientation,
         sal::Sample base_angle) :
      MemorylessMonoMic(position, orientation),
      Microphone(position, orientation),
      base_angle_(base_angle) {}
  
  virtual bool IsCoincident() { return true; }
  
  virtual ~TanMic() {}
private:
  virtual Sample GetDirectivity(const mcl::Point& point) {
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
  
  sal::Sample base_angle_;
};




} // namespace sal
  
#endif



