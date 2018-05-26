/*
 cipicmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_CIPICMIC_H
#define SAL_CIPICMIC_H

#include <map>
#include <vector>
#include "microphone.h"
#include "quaternion.h"
#include "saltypes.h"
#include "array.h"
#include "binauralmic.h"
#include "salconstants.h"

#define NUM_ELEVATIONS_CIPIC 50
#define LENGTH_BRIR_CIPIC 200
#define NORMALISING_VALUE_CIPIC 1.0

namespace sal {
  

  
class CipicMic : public DatabaseBinauralMic {
public:
  
  enum DataType {
    txt,
    wav
  };
  
  /**
   Constructs a Kemar microphone opject.
   `directory` contains the hrtf database.
   */
  CipicMic(const mcl::Point& position, const mcl::Quaternion& orientation,
           const std::string& directory, const DataType data_type,
           const Int update_length = 0);
  
  using BinauralMic::IsCoincident;
  using BinauralMic::num_channels;
  
  static bool Test();
  
  ~CipicMic() {}
  
  
private:
  static std::vector<std::vector<Signal> > Load(const Ear ear,
                                                const std::string& directory,
                                                const DataType data_type,
                                                const std::vector<sal::Angle>& azimuths);

  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept;
  
  std::vector<sal::Angle> azimuths_;
  
};
  
  
  
} // namespace sal

#endif
