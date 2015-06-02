/*
 cipicmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef SAL_CIPICMIC_H
#define SAL_CIPICMIC_H

#include <map>
#include <vector>
#include "microphone.h"
#include "mcl.h"
#include "saltypes.h"
#include "stream.h"
#include "array.h"
#include "binauralmic.h"


#define NUM_ELEVATIONS_CIPIC 50
#define NORMALISING_VALUE_CIPIC 1.0

namespace sal {
  
class CipicMic : public BinauralMic {
public:
  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  CipicMic(Point position, Angle theta, Angle phi, Angle psi,
           const std::string directory);
  
  static bool Test();
  
  virtual ~CipicMic() {}
private:
  
  
  std::vector<std::vector<Signal> > Load(const Ear ear,
                                         const std::string directory);
  
  virtual Signal GetBrir(const Ear ear, const Point& point);
  
  std::vector<sal::Angle> azimuths_;
  
  
  // Database
  std::vector<std::vector<Signal> > hrtf_database_right_;
  std::vector<std::vector<Signal> > hrtf_database_left_;
};
  
  
  
} // namespace sal

#endif
