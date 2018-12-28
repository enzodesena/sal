/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


namespace sal
{
template<typename T>
Receiver<T>::Receiver(
  Directivity<T> directivity_prototype,
  Point position,
  Quaternion orientation,
  size_t max_num_incoming_waves)
  : position_(position)
  , orientation_(orientation)
  , handedness_(mcl::kRightHanded)
  , directivity_instances_(max_num_incoming_waves, directivity_prototype)
  , bypass_(false)
{
}


template<typename T>
Point Receiver<T>::position() const noexcept
{
  return position_;
}


template<typename T>
void Receiver<T>::SetPosition(
  const Point& position) noexcept
{
  position_ = position;
}


template<typename T>
Quaternion Receiver<T>::orientation() const noexcept
{
  return orientation_;
}


template<typename T>
void Receiver<T>::SetOrientation(
  const Quaternion& orientation) noexcept
{
  orientation_ = orientation;
}


template<typename T>
void Receiver<T>::SetHandedness(
  mcl::Handedness handedness) noexcept
{
  handedness_ = handedness;
}


template<typename T>
void Receiver<T>::SetBypass(
  bool bypass) noexcept
{
  bypass_ = bypass;
}


template<typename T>
void Receiver<T>::ReceiveAndAddToBuffer(
  const mcl::Vector<T>& input,
  const Point& point,
  const size_t wave_id,
  Buffer<T>& output_buffer) noexcept
{
  ASSERT_WITH_MESSAGE
  (
    wave_id<directivity_instances_.size(),
    "Requested a wave id larger than the max num of incoming waves.");
  if (! bypass_)
  {
    directivity_instances_[wave_id].ReceiveAndAddToBuffer
    (
      input,
      GetRelativePoint(point),
      output_buffer);
  }
  else
  {
    for (size_t i=0; i<output_buffer.num_channels(); ++i)
    {
      std::copy(input.begin(), input.end(), output_buffer.begin(i));
    }
  }
}


template<typename T>
void Receiver<T>::ReceiveAndAddToBuffer(
  const mcl::Vector<T>& input,
  const Point& point,
  Buffer<T>& output_buffer) noexcept
{
  if (! bypass_)
  {
    directivity_instances_[0].ReceiveAndAddToBuffer
    (
      input,
      GetRelativePoint(point),
      output_buffer);
  }
  else
  {
    for (size_t i=0; i<output_buffer.num_channels(); ++i)
    {
      std::copy(input.begin(), input.end(), output_buffer.begin(i));
    }
  }
}


template<typename T>
void Receiver<T>::ResetState() noexcept
{
  for (
    auto iter=directivity_instances_.begin();
    iter != directivity_instances_.end();
    ++iter)
  {
    (*iter).ResetState();
  }
}


template<typename T>
Point Receiver<T>::GetRelativePoint(
  const Point& point) const noexcept
{
  if (mcl::IsEqual(point, position_))
  {
    mcl::Logger::GetInstance().
      LogError
      (
        "Microphone (%f, %f, %f) and observation point (%f, %f, %f) appear "
        "to be approximately in the same position. Behaviour undefined.",
        point.x(),
        point.y(),
        point.z(),
        position_.x(),
        position_.y(),
        position_.z());
  }

  // Instead of rotating the head, we are rotating the point in an opposite
  // direction (that's why the QuatInverse).
  Point rotated = mcl::QuatRotate
  (
    mcl::QuatInverse(orientation_),
    Point(point.x() - position_.x(),
    point.y() - position_.y(),
    point.z() - position_.z()),
    handedness_);
  return rotated;
}
} // namespace sal
