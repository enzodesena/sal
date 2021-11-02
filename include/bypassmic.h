/*
 Spatial Audio Library (SAL)
 Copyright (c) 2021, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_BYPASSMIC_H
#define SAL_BYPASSMIC_H

#include "salconstants.h"
#include "saltypes.h"
#include "salutilities.h"
#include "microphone.h"

namespace sal {

/** This microphone copies whatever is coming in to respective channels. */
class BypassMic : public sal::Microphone {
public:
  /**
   Constructs a bypass microphone object.
   
   @param[in] position The position in space of the microphone.
   */
  BypassMic(mcl::Point position, sal::Int num_channels) noexcept;
  
  /** Returns whether or not the microphone is a coincident microphone. */
  virtual bool IsCoincident() const noexcept { return true; }
  
  /** The destructor of the object. */
  virtual ~BypassMic() {}
  
  virtual sal::Int num_channels() const noexcept { return num_channels_; }
  
  virtual void AddPlaneWaveRelative(const sal::Sample* input_data,
                                    const sal::Int num_samples,
                                    const mcl::Point& point,
                                    const sal::Int wave_id,
                                    sal::Buffer& output_buffer) noexcept;
  
private:
  sal::Int num_channels_;
};
  
  
} // namespace sal


#endif
