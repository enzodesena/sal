/*
 kemarmic.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2015, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com

 */

#include <algorithm>
#include <fstream>

#include "butter.h"
#include "kemarmic.h"
#include "point.h"
#include "salconstants.h"
#include "vectorop.h"

#ifdef _WIN32
#define sprintf(...) sprintf_s(__VA_ARGS__)
#define strcpy(...) strcpy_s(__VA_ARGS__)
#define strcat(...) strcat_s(__VA_ARGS__)
#endif

using mcl::Point;
using mcl::Quaternion;

namespace sal {

KemarMic::KemarMic(const Point& position, const Quaternion orientation,
                   const DatasetType dataset_type, const Int num_samples,
                   const Int update_length,
                   const HeadRefOrientation reference_orientation,
                   const Time sampling_frequency, const std::string directory)
    : DatabaseBinauralMic(position, orientation, update_length,
                          reference_orientation) {
  num_measurements_ = GetNumMeasurements();
  elevations_ = GetElevations();

  if (dataset_type != kDirectoryCompact && dataset_type != kDirectoryLeft &&
      dataset_type != kDirectoryRight) {
    hrtf_database_right_ = LoadEmbedded(kRightEar, dataset_type);
    hrtf_database_left_ = LoadEmbedded(kLeftEar, dataset_type);
  } else {
    hrtf_database_right_ = Load(kRightEar, directory, dataset_type);
    hrtf_database_left_ = Load(kLeftEar, directory, dataset_type);
  }

  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();

  // This is the sampling frequency that will actually be used
  Time used_sampling_frequency;
  Int used_num_samples;

  if (sampling_frequency > 33075.0) {
    used_sampling_frequency = 44100.0;
    used_num_samples = kFullBrirLength;
    // Do nothing as the database is already 44100.0
  } else {
    used_sampling_frequency = 22050.0;
    used_num_samples = 64;
    // Downsample the database by a factor 2
    mcl::IirFilter filter = mcl::Butter(10, 0.001, 0.45);
    FilterAll(filter);
    for (Int i = 0; i < NUM_ELEVATIONS_KEMAR; ++i) {
      for (Int j = 0; j < num_measurements[i]; ++j) {
        hrtf_database_right_[i][j] =
            mcl::Downsample(hrtf_database_right_[i][j], 2);
        hrtf_database_left_[i][j] =
            mcl::Downsample(hrtf_database_left_[i][j], 2);
      }
    }
  }

  if (!mcl::IsEqual(sampling_frequency, 22050.0) &&
      !mcl::IsEqual(sampling_frequency, 44100.0)) {
    mcl::Logger::GetInstance().LogError(
        "The sampling frequency (%f) is not supported for "
        "the Kemar mic. Using %f instead.",
        sampling_frequency, used_sampling_frequency);
  }

  if (used_num_samples != kFullBrirLength) {
    for (Int i = 0; i < NUM_ELEVATIONS_KEMAR; ++i) {
      for (Int j = 0; j < num_measurements[i]; ++j) {
        hrtf_database_right_[i][j] =
            mcl::ZeroPad<Sample>(hrtf_database_right_[i][j], used_num_samples);
        hrtf_database_left_[i][j] =
            mcl::ZeroPad<Sample>(hrtf_database_left_[i][j], used_num_samples);
      }
    }
  }
}

Array<mcl::Int, NUM_ELEVATIONS_KEMAR> KemarMic::GetNumMeasurements() noexcept {
  return {56, 60, 72, 72, 72, 72, 72, 60, 56, 45, 36, 24, 12, 1};
}

Array<mcl::Int, NUM_ELEVATIONS_KEMAR> KemarMic::GetElevations() noexcept {
  return {-40, -30, -20, -10, 0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
}

bool KemarMic::IsDatabaseAvailable(const std::string directory,
                                   const DatasetType dataset_type) {
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations = GetElevations();

  for (Int i = 0; i < NUM_ELEVATIONS_KEMAR; ++i) {
    Angle resolution = 360.0 / num_measurements[i];
    Angle elevation = elevations[i];
    Int num_measurement = (UInt)floor(((Angle)num_measurements[i]) / 2.0) + 1;

    for (Int j = 0; j < num_measurement; ++j) {
      Angle angle = mcl::RoundToInt(j * resolution);

      std::ifstream file;
      file.open(GetFilePath(elevation, angle, directory, dataset_type),
                std::ios::in | std::ios::binary | std::ios::ate);
      if (!file.good()) {
        return false;
      }
    }
  }
  return true;
}

std::string KemarMic::GetFilePath(const Angle elevation, const Angle angle,
                                  const std::string directory,
                                  const DatasetType dataset_type) noexcept {
  char file_name[1000];
  char directory_name[1000];
  char file_path[1000];
  char preamble_char = '0';

  switch (dataset_type) {
    case kDirectoryCompact:
      preamble_char = 'H';
      break;
    case kDirectoryLeft:
      preamble_char = 'L';
      break;
    case kDirectoryRight:
      preamble_char = 'R';
      break;
    default:
      ASSERT(false);
      break;
  }
  snprintf(directory_name, 20, "/elev%d/", (int)elevation);

  snprintf(file_name, 20, "%c%de%03da.dat", preamble_char, (int)elevation,
           (int)angle);

  strcpy(file_path, directory.c_str());
  strcat(file_path, directory_name);
  strcat(file_path, file_name);
  return std::string(file_path);
}

void KemarMic::PrintParsedDatabase(const Ear ear, const std::string directory,
                                   const DatasetType dataset_type,
                                   const Int num_samples) {
  std::vector<std::vector<Signal> > hrtf_database =
      KemarMic::Load(ear, directory, dataset_type);

  for (Int i = 0; i < (Int)hrtf_database.size(); ++i) {
    for (Int j = 0; j < (Int)hrtf_database[i].size(); ++j) {
      printf("{%d,%d,", (int)i, (int)j);
      for (Int sample_id = 0; sample_id < (Int)hrtf_database[i][j].size();
           ++sample_id) {
        printf("%d", static_cast<int>(hrtf_database[i][j][sample_id] *
                                      NORMALISING_VALUE_KEMAR));
        if (sample_id < (Int)hrtf_database[i][j].size() - 1) {
          printf(",");
        }
      }
      printf("}, \n");
    }
  }
}

std::vector<std::vector<Signal> > KemarMic::LoadEmbedded(
    const Ear ear, const DatasetType dataset_type) {
  std::vector<std::vector<Signal> > hrtf_database;
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();

  const Int num_samples = (dataset_type == kCompactDataset)
                              ? COMPACT_LENGTH_KEMAR
                              : MAX_LENGTH_KEMAR;

  for (Int i = 0; i < NUM_ELEVATIONS_KEMAR; ++i) {
    // Initialise vector
    hrtf_database.push_back(std::vector<Signal>(num_measurements[i]));
    for (Int j = 0; j < num_measurements[i]; ++j) {
      hrtf_database[i].push_back(Signal(num_samples));
    }
  }

  switch (dataset_type) {
#ifndef DO_NOT_LOAD_EMBEDDED_KEMAR
    case kCompactDataset:
      LoadEmbeddedCompactData(ear, hrtf_database);
      break;
    case kFullDataset:
      LoadEmbeddedFullData(ear, hrtf_database);
      break;
    case kDiffuseDataset:
      LoadEmbeddedDiffuseData(ear, hrtf_database);
      break;
#endif
    default:
      ASSERT(false);
  }

  mcl::IirFilter normalising_filter =
      mcl::GainFilter(1.0 / NORMALISING_VALUE_KEMAR);
  mcl::FilterAll(normalising_filter, hrtf_database, hrtf_database);
  return hrtf_database;
}

std::vector<std::vector<Signal> > KemarMic::Load(
    const Ear ear, const std::string directory,
    const DatasetType dataset_type) {
  bool dataset_is_compact = (dataset_type == kDirectoryCompact);

  std::vector<std::vector<Signal> > hrtf_database;

  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> num_measurements = GetNumMeasurements();
  Array<mcl::Int, NUM_ELEVATIONS_KEMAR> elevations = GetElevations();

  for (Int i = 0; i < NUM_ELEVATIONS_KEMAR; ++i) {
    // Initialise vector
    hrtf_database.push_back(std::vector<Signal>(num_measurements[i]));

    const Angle resolution = 360.0 / num_measurements[i];
    const Angle elevation = elevations[i];
    const Int num_measurement =
        dataset_is_compact
            ? ((Int)floor(((Angle)num_measurements[i]) / 2.0) + 1)
            : num_measurements[i];

    for (Int j = 0; j < num_measurement; ++j) {
      Angle angle = mcl::RoundToInt(j * resolution);

      std::ifstream file;
      file.open(GetFilePath(elevation, angle, directory, dataset_type),
                std::ios::in | std::ios::binary | std::ios::ate);
      if (!file.good()) {
        mcl::Logger::GetInstance().LogErrorToCerr("Kemar lib not found.");
        ASSERT(false);
      }
      long size = (long)file.tellg();
      ASSERT(sizeof(short) == 2);
      short* data = new short[size / 2];

      for (int k = 0; k < size; k += sizeof(short)) {
        file.seekg(k, std::ios::beg);
        short big_endian;
        file.read((char*)&big_endian, sizeof(short));
        short little_endian =
            (short)(((big_endian & 0xff) << 8) | ((big_endian & 0xff00) >> 8));
        data[k / 2] = little_endian;
      }

      file.close();
      size = size / 2;  // Length in number of samples
      ASSERT(size % 2 == 0);
      ASSERT((size / 2) % 2 == 0);

      if (dataset_is_compact) {  // If the data is "compact", then left and
                                 // right channels are interleaved
        for (Int k = 0; k < size; k += 2) {
          Int ipsilateral_index = j;
          Int contralateral_index =
              (UInt)((((Int)num_measurements[i]) - ((Int)j)) %
                     (Int)num_measurements[i]);

          if (ear == kRightEar) {
            hrtf_database[i][ipsilateral_index].push_back(
                data[k] / NORMALISING_VALUE_KEMAR);
            // In the two cases for azimuth = 0, and azimuth = 180 the signals
            // at left and right ears are equal.
            if (ipsilateral_index != contralateral_index) {
              hrtf_database[i][contralateral_index].push_back(
                  data[k + 1] / NORMALISING_VALUE_KEMAR);
            }
          } else {
            hrtf_database[i][ipsilateral_index].push_back(
                data[k + 1] / NORMALISING_VALUE_KEMAR);
            if (ipsilateral_index != contralateral_index) {
              hrtf_database[i][contralateral_index].push_back(
                  data[k] / NORMALISING_VALUE_KEMAR);
            }
          }
        }
      } else {
        Int ipsilateral_index = j;
        Int contralateral_index =
            (UInt)((((Int)num_measurements[i]) - ((Int)j)) %
                   (Int)num_measurements[i]);

        // The reason why the the left/right ears are flipped is due to the
        // difference between reference systems in Kemar dataset and this
        // software (I think..)
        Int azimuth_index =
            ((dataset_type == kDirectoryLeft && ear == kRightEar) ||
             (dataset_type == kDirectoryRight && ear == kLeftEar))
                ? ipsilateral_index
                : contralateral_index;

        for (Int k = 0; k < size; k++) {
          hrtf_database[i][azimuth_index].push_back(data[k] /
                                                    NORMALISING_VALUE_KEMAR);
        }
      }

      delete[] data;
    }
  }

  return hrtf_database;
}

Int KemarMic::FindElevationIndex(Angle elevation) {
  Int elevation_index = mcl::RoundToInt(elevation / 10.0) + 4;
  if (elevation_index < 0) {
    return 0;
  } else if (elevation_index > 13) {
    return 13;
  } else {
    return (UInt)elevation_index;
  }
}

Int KemarMic::FindAzimuthIndex(Angle azimuth, Int elevation_index) {
  const Int num_measurements[] = {56, 60, 72, 72, 72, 72, 72,
                                  60, 56, 45, 36, 24, 12, 1};

  Angle angular_resolution = 360.0 / ((Angle)num_measurements[elevation_index]);
  Int azimuth_index = mcl::RoundToInt(azimuth / angular_resolution);

  if (azimuth_index == num_measurements[elevation_index]) {
    azimuth_index = 0;
  }

  return azimuth_index;
}

Signal KemarMic::GetBrir(const Ear ear, const Point& point) noexcept {
  // For forward looking direction, Azimuth = 0 and elevation =0
  Point norm_point = Normalized(point);
  Angle elevation = (asin((double)norm_point.z())) / PI * 180.0;

  Angle azimuth;

  switch (reference_orientation_) {
    case HeadRefOrientation::standard:
      azimuth = atan((double)norm_point.y() / norm_point.x()) / PI * 180.0;
      if (mcl::IsNan(azimuth)) {
        azimuth = 0.0;
      }  // Conventionally, if x=y=0 then azimuth is taken as 0
      if (norm_point.x() < 0.0) {
        azimuth += 180.0;
      }
      break;
    case HeadRefOrientation::y_z:
      azimuth = -atan((double)norm_point.x() / norm_point.y()) / PI * 180.0;
      if (mcl::IsNan(azimuth)) {
        azimuth = 0.0;
      }  // Conventionally, if x=y=0 then azimuth is taken as 0
      if (norm_point.y() < 0.0) {
        azimuth += 180.0;
      }
      break;
    default:
      ASSERT(false);
      break;
  }

  azimuth = mcl::Mod(azimuth, 360.0);

  if (std::isnan(elevation)) {
    elevation = 0.0;
    mcl::Logger::GetInstance().LogError(
        "The elevation of a sound source "
        "appears to be NAN (possibily due to coincident sound source and "
        "observation point). Reverting to a %f elevation. ",
        elevation);
  }

  if (std::isnan(azimuth)) {
    azimuth = 0.0;
    mcl::Logger::GetInstance().LogError(
        "The azimuth of a sound source "
        "appears to be NAN (possibily due to coincident sound source and "
        "observation point). Reverting to a %f azimuth.",
        azimuth);
  }

  ASSERT((elevation >= (-90.0 - VERY_SMALL)) &
         (elevation <= (90.0 + VERY_SMALL)));
  ASSERT((azimuth >= (0.0 - VERY_SMALL)) & (azimuth <= (360.0 + VERY_SMALL)));

  Int elevation_index = FindElevationIndex(elevation);
  Int azimuth_index = FindAzimuthIndex(azimuth, elevation_index);

  if (ear == kLeftEar) {
    return hrtf_database_left_[elevation_index][azimuth_index];
  } else {
    return hrtf_database_right_[elevation_index][azimuth_index];
  }
}

}  // namespace sal
