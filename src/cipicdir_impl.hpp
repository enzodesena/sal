/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


namespace sal
{

// Definitions
template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::hrir_left_;
template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::hrir_right_;

template<typename T>
CipicDirectivity<T>::CipicDirectivity(
  const std::string& directory,
  const DataType data_type,
  const size_t update_length,
  const HeadRefOrientation reference_orientation)
  : DatabaseBinauralDirectivity<T>(update_length, reference_orientation)
{
  if (hrir_left_.size() == 0)
  {
    hrir_right_ = Load(Ear::kRight, directory, data_type, azimuths_);
    hrir_left_ = Load(Ear::kLeft, directory, data_type, azimuths_);
  }
}


template<typename T>
std::vector<mcl::Vector<Signal<T>>> CipicDirectivity<T>::Load(
  const Ear ear,
  const std::string& directory,
  const DataType data_type,
  const mcl::Vector<sal::Angle>& azimuths)
{
  std::vector<mcl::Vector<Signal<T>>> hrtf_database;

  for (Int j = 0; j < (Int)azimuths.size(); ++j)
  {
    Int azimuth = (Int)azimuths[j];

    std::string sign_text = (azimuth < 0) ? "neg" : "";
    std::string azimuth_text = ToString((azimuth > 0) ? azimuth : -azimuth);
    std::string ear_text = (ear == Ear::kLeft) ? "left" : "right";
    std::string data_type_text = (data_type == DataType::kWav) ? ".wav" : ".txt";
    std::string file_name =
      sign_text + azimuth_text + "az" + ear_text + data_type_text;
    std::string file_path = directory + "/" + file_name;

    std::ifstream file;
    file.open(file_path, std::ios::in | std::ios::binary | std::ios::ate);
    if (! file.good())
    {
      mcl::Logger::GetInstance().LogErrorToCerr("Cipic lib not found.");
      ASSERT(false);
    }
    file.close();

    mcl::Vector<mcl::Vector<sal::Sample>> brirs;
    switch (data_type)
    {
#ifdef __x86_64__
#ifndef IOSARM
#ifndef NO_WAV_HANDLER
      case DataType::kWav:
        // For some reason I can't understand, the wav files contain the
      // BRIR across channels--there are 200 channels, one per sample;
        // there are 50 samples, one per elevation....
        brirs =
        mcl::Transpose(mcl::Matrix<sal::Sample>(WavHandler<T>::Read(file_path))).data();
        break;
#endif
#endif
#endif

    case DataType::kTxt:
      brirs = mcl::Matrix<sal::Sample>::Load(file_path).data();
      break;
    default:
      ASSERT(false);
      break;
    }

    ASSERT(brirs.size() == NUM_ELEVATIONS_CIPIC);
    ASSERT(brirs[0].size() == LENGTH_BRIR_CIPIC);
    hrtf_database.push_back(brirs);
  }

  return hrtf_database;
}


template<typename T>
Signal<T> CipicDirectivity<T>::GetBrir(
  const Ear ear,
  const Point& point) noexcept
{
  // Calculate azimuth
  // For forward looking direction, Azimuth = 0 and elevation =0
  // "positive azimuth coresponds to moving right."
  Angle azimuth = (PI / 2.0 -
    AngleBetweenPoints(point, Point(0.0, -1.0, 0.0))) / PI * 180.0;

  // Calculate elevation
  Point proj_xz = Projection(point, Point(0.0, 1.0, 0.0));
  // "Positive elevation coresponds to moving up"
  Angle elevation;
  if (point.x() >= 0.0)
  {
    elevation = 90.0 - proj_xz.theta() / PI * 180.0;
  }
  else
  {
    elevation = 90.0 + proj_xz.theta() / PI * 180.0;
  }

  if (isnan(elevation))
  {
    elevation = 0.0;
  }
  if (isnan(azimuth))
  {
    azimuth = 0.0;
  }

  ASSERT
  (
    (elevation >= (-90.0 - VERY_SMALL)) &
    (elevation <= (270.0 + VERY_SMALL)));
  ASSERT
  (
    (azimuth >= (-90.0 - VERY_SMALL)) &
    (azimuth <= (90.0 + VERY_SMALL)));

  size_t azimuth_index = mcl::MinIndex
  (
    mcl::Abs
    (
      mcl::Add
      (
        azimuths_,
        -azimuth)));

  Int elevation_index;
  if (elevation < -45.0)
  {
    elevation_index = 0;
  }
  else if (elevation > -45.0 + 360.0 / 64.0 * 49.0)
  {
    elevation_index = 49;
  }
  else
  {
    elevation_index = mcl::RoundToInt((elevation + 45.0) * 64.0 / 360.0);
  }

  ASSERT((azimuth_index >= 0) & (azimuth_index < azimuths_.size()));
  ASSERT((elevation_index >= 0) & (elevation_index <= 49));

  return (ear == Ear::kLeft) ?
           hrir_left_[azimuth_index][elevation_index] :
           hrir_right_[azimuth_index][elevation_index];
}

} // namespace sal
