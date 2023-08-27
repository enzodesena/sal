/*
 kemarmic.h
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */

#ifndef SAL_KEMARMIC_H
#define SAL_KEMARMIC_H

#define MAX_LENGTH_KEMAR 512
#define COMPACT_LENGTH_KEMAR 128
#define NUM_ELEVATIONS_KEMAR 14
#define NORMALISING_VALUE_KEMAR 30000.0

#include <map>
#include <vector>
#include "microphone.h"
#include "saltypes.h"
#include "array.h"
#include "binauralmic.h"
#include "salconstants.h"

namespace sal {
  

class KemarMic : public DatabaseBinauralMic {
public:
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   With `num_samples` you can choose the length of the 
   BRIR. If set to zero yields the entire BRIR.
   */
  KemarMic(const mcl::Point& position,
           const mcl::Quaternion orientation,
           const std::string directory = "",
           const Int num_samples = kFullBrirLength,
           const Int update_length = 0,
           const HeadRefOrientation reference_orientation = HeadRefOrientation::standard,
           const Time sampling_frequency = 44100.0);
  
  static const int kFullBrirLength = -1;
  
  static bool IsDatabaseAvailable(const std::string directory);
  
  static void PrintParsedDatabase(const Ear ear,
                                  const std::string directory,
                                  const Int num_samples,
                                  std::string variable_name);
  
  
  enum EmbeddedDatasetType {
    kCompactDataset,
    kFullDataset,
    kDiffuseDataset
  };
  
  // TODO: turn the following two structs into a single struct
  struct KemarData {
    Int elevation_id;
    Int azimuth_id;
    Sample data[MAX_LENGTH_KEMAR];
  };
  
  struct KemarDataCompact {
    Int elevation_id;
    Int azimuth_id;
    Sample data[COMPACT_LENGTH_KEMAR];
  };
  
  static bool Test();
private:
  virtual Signal GetBrir(const Ear ear, const mcl::Point& point) noexcept;
  
  static
  std::vector<std::vector<Signal> > Load(const Ear ear,
                                         const std::string directory);
  
  static std::vector<std::vector<Signal> > LoadEmbedded(const Ear ear,
                                                        const EmbeddedDatasetType dataset_type);
  
  
  // TODO: these methods are identical except for the variables they load
  static void LoadEmbeddedCompactData(const Ear ear,
                                      std::vector<std::vector<Signal> >& data);
  static void LoadEmbeddedFullData(const Ear ear,
                                   std::vector<std::vector<Signal> >& data);
  static void LoadEmbeddedDiffuseData(const Ear ear,
                                      std::vector<std::vector<Signal> >& data);
  
  /**
   Returns the elevation index for kemar database for elevation in azimuth.
   The index departs from 0.
   */
  Int FindElevationIndex(Angle elevation);
  
  
  /** 
   Returns the azimuthal index for kemar database for azimuth in grad.
   The index departs from 0.
   */
  Int FindAzimuthIndex(Angle azimuth, Int elevation_index);
  
  static Array<mcl::Int, NUM_ELEVATIONS_KEMAR> GetNumMeasurements() noexcept;
  static Array<mcl::Int, NUM_ELEVATIONS_KEMAR> GetElevations() noexcept;
  
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements_;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations_;
  
  static std::string GetFilePath(const Angle elevation, const Angle angle,
                                 const std::string directory) noexcept;
  
};
  





  
} // namespace sal


#endif
