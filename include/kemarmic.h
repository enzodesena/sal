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
#define NORMALISING_VALUE_KEMAR 32768.0 // This is 2^15 (16 bit in two's complement)

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
  enum DatasetType {
    kCompactDataset, // Using DB-061 (standard pinna)
    kFullDataset, // Using DB-061 (standard pinna)
    kDiffuseDataset, // Using DB-061 (standard pinna)
    kDirectoryCompact, // Either for "compact" directory or "diffuse" directory
    kDirectoryLeft, // Full with DB-061 (standard pinna)
    kDirectoryRight // Full with DB-065 (large red pinna)
  };
  
  /** 
   Constructs a Kemar microphone opject. 
   `directory` contains the hrtf database.
   With `num_samples` you can choose the length of the 
   BRIR. If set to zero yields the entire BRIR.
   */
  KemarMic(const mcl::Point& position,
           const mcl::Quaternion orientation,
           const DatasetType dataset_type = kDiffuseDataset,
           const Int num_samples = kFullBrirLength,
           const Int update_length = 0,
           const HeadRefOrientation reference_orientation = HeadRefOrientation::standard,
           const Time sampling_frequency = 44100.0,
           const std::string directory = "");
  
  static const int kFullBrirLength = -1;
  
  static bool IsDatabaseAvailable(const std::string directory,
                                  const DatasetType dataset_type);
  
  static void PrintParsedDatabase(const Ear ear,
                                  const std::string directory,
                                  const DatasetType dataset_type,
                                  const Int num_samples);
  
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
                                         const std::string directory,
                                         const DatasetType dataset_type);
  
  static std::vector<std::vector<Signal> > LoadEmbedded(const Ear ear,
                                                        const DatasetType dataset_type);
  
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
                                 const std::string directory,
                                 const DatasetType dataset_type) noexcept;
  
};

  
} // namespace sal


#endif
