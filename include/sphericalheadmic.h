/*
 directivityhrtf.h
 Spatial Audio Library (SAL)
 Copyright (c) 2011, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_DIRECTIVITYSPHERICAL_H
#define SAL_DIRECTIVITYSPHERICAL_H

#include "microphone.h"
#include "saltypes.h"
#include "kemarmic.h"
#include <map>
#include "salconstants.h"

namespace sal {

/**
 This object implements the spherical head model proposed by Duda et al in the
 article "Range dependence of the response of a spherical head model"
 */
class SAL_API SphericalHeadMic : public BinauralMic {
public:
  SphericalHeadMic(const mcl::Point position, const mcl::Quaternion orientation, 
                   const Angle ears_angle,
                   const Length sphere_radius,
                   const Int ir_length,
                   const Time sampling_frequency,
                   const Int update_length = 0);
  
  static bool Test();
  
  virtual ~SphericalHeadMic() {}
private:
  
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept;
  
  /** For the various definitions see Duda's paper. */
  static mcl::Complex Sphere(Length a, Length r, Angle theta,
                             Time f, Time c, mcl::Real threshold);
  
  /**
   Generate the impulse response. theta is the angle in Duda's article
   reference system. If `minimum_phase` is true the output is the
   minimum phase version delayed by the appropriate amount of delay due to
   propagation around the head (it does NOT include propagation from the
   source to the centre of the head).
   */
  static Signal GenerateImpulseResponse(Length sphere_radius,
                                        Length source_distance,
                                        Angle theta,
                                        Time sound_speed,
                                        mcl::Real threshold,
                                        Int num_samples,
                                        Time sampling_frequency,
                                        bool minimum_phase = false);
  
  /** Calculates the angle in the reference system of the sphere. */
  static Angle GetTheta(const mcl::Point& point,
                            const Angle& ears_angle,
                            const Ear& ear);

  
  /**
   Angle formed between the facing direction and the ear [rad]. The default
   value is 100 deg as suggested in Duda's paper.
   */
  Angle ears_angle_;
  
  /** Radius of the sphere (head model) in [m]. */
  Length sphere_radius_;
  
  Time sampling_frequency_;
  
  /**
   This is the length of the impulse response. To avoid time aliasing, it
   should be longer that the maximum expected length of the impulse response 
   (in the same way that we should sample in time at a higher freq
   than double the highest frequency). A conservative value for sampling
   frequency 44100 is 512. Since there are ffts involved here, it is a 
   good idea to use a power of 2.
   */
  Int impulse_response_length_;
  
  Time sound_speed_;
  
  /** This is the threshold of the sphere algorithm. */
  mcl::Real alg_threshold_;
  
};
  
} // namespace sal

#endif
