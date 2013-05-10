/*
 directivityhrtf.h
 Spatial Audio Toolbox (SAT)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 106 $
 Last changed date:  $Date: 2012-06-22 03:31:02 +0100 (Fri, 22 Jun 2012) $
 */

#ifndef SAT_DIRECTIVITYSPHERICAL_H
#define SAT_DIRECTIVITYSPHERICAL_H

#include "microphone.h"
#include "sattypes.h"
#include "kemarmic.h"
#include "mcl.h"
#include "stream.h"
#include <map>

namespace sat {

class SHMicInstance;
  

// References:
// Duda, "From Range dependence of the response of a spherical head model"
class SphericalHeadMic : public StereoMicrophone {
public:
  SphericalHeadMic(const Point position, const Angle theta,
                   const Angle phi, const Angle psi,
                   const Angle ears_angle,
                   const Length sphere_radius,
                   const UInt ir_length,
                   const Time sampling_frequency);
  
  virtual void Tick();
  
  virtual void Reset();
  
  static bool Test();
  
  virtual ~SphericalHeadMic() {}
private:
  virtual void RecordPlaneWaveRelative(const Sample& sample, const Point& point,
                                       const UInt& wave_id);
  
  // For the various definitions see Duda's paper.
  static mcl::Complex Sphere(Length a, Length r, Angle theta,
                             Time f, Time c, mcl::Real threshold);
  
  // Generate the impulse response. theta is the angle in Duda's article
  // reference system. If `minimum_phase` is true the output is the
  // minimum phase version delayed by the appropriate amount of delay due to
  // propagation around the head (it does NOT include propagation from the
  // source to the centre of the head).
  static Signal GenerateImpulseResponse(Length sphere_radius,
                                        Length source_distance,
                                        Angle theta,
                                        Time sound_speed,
                                        mcl::Real threshold,
                                        UInt num_samples,
                                        Time sampling_frequency,
                                        bool minimum_phase = false);
  
  // Calculates the angle in the reference system of the sphere.
  static Angle GetTheta(const Point& point,
                            const Angle& ears_angle,
                            const Ear& ear);

  
  //virtual std::vector<Sample> ImpulseResponse(Point point) const;
  
  
  
  // Angle formed between the facing direction and the ear [rad]. The default
  // value is 100 deg as suggested in Duda's paper.
  Angle ears_angle_;
  
  // Radius of the sphere (head model) in [m].
  Length sphere_radius_;
  
  Time sampling_frequency_;
  
  // This is the length of the impulse response. To avoid time aliasing, it
  // should be longer that the maximum expected length of the impulse response 
  // (in the same way that we should sample in frequency at a higher freq
  // than double the highest frequency). A conservative value for sampling
  // frequency 44100 is 512. Since there are ffts involved here, it is a 
  // good idea to use a power of 2.
  UInt impulse_response_length_;
  
  Time sound_speed_;
  
  // This is the threshold of the sphere algorithm.
  mcl::Real alg_threshold_;
  
  std::map<UInt, SHMicInstance> instances_left_;
  std::map<UInt, SHMicInstance> instances_right_;
  
  friend class SHMicInstance;
};
  
class SHMicInstance {
private:
  SHMicInstance(SphericalHeadMic* base_mic, Ear ear) :
          base_mic_(base_mic),
          filter_(mcl::FirFilter::GainFilter(1.0)),
          ear_(ear),
          previous_point_(Point(NAN, NAN, NAN)) {}
  
  
  Sample RecordPlaneWaveRelative(const Sample& sample,
                                 const Point& point);

  // The microphone object is called for every sample, while the position
  // of SDN's elements is changed once in a while. Hence, these angles are
  // stored so that we don't need to update the filter coefficients at every
  // sample, but only when something changes.
  Point previous_point_;
  
  mcl::FirFilter filter_;
  
  Ear ear_;
  
  SphericalHeadMic* base_mic_;
  
  friend class SphericalHeadMic;
};

  
} // namespace sat

#endif
