/*
 simulation.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 This class calculates the signals recorded by N microphones due to M acoustic
 sources in free-field.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


namespace sal
{
template<typename T>
FreeFieldSim<T>::FreeFieldSim(
  const mcl::Vector<Receiver<T>>& microphones,
  const mcl::Vector<Source>& sources,
  const Time sampling_frequency,
  const Length sound_speed)
{
  Init(microphones, sources, sampling_frequency, sound_speed);
}


template<typename T>
FreeFieldSim<T>::FreeFieldSim(
  const Receiver<T>& microphone,
  const mcl::Vector<Source>& sources,
  const Time sampling_frequency,
  const Length sound_speed)
{
  Init
  (
    mcl::UnaryVector<Receiver<T>>(microphone),
    sources,
    sampling_frequency,
    sound_speed);
}


template<typename T>
FreeFieldSim<T>::FreeFieldSim(
  const mcl::Vector<Receiver<T>>& microphones,
  const Source& source,
  const Time sampling_frequency,
  const Length sound_speed)
{
  Init
  (
    microphones,
    mcl::UnaryVector<Source>(source),
    sampling_frequency,
    sound_speed);
}


template<typename T>
FreeFieldSim<T>::FreeFieldSim(
  const Receiver<T>& microphone,
  const Source& source,
  const Time sampling_frequency,
  const Length sound_speed)
{
  Init
  (
    mcl::UnaryVector<Receiver<T>>(microphone),
    mcl::UnaryVector<Source>(source),
    sampling_frequency,
    sound_speed);
}


template<typename T>
void FreeFieldSim<T>::Init(
  const mcl::Vector<Receiver<T>>& microphones,
  const mcl::Vector<Source>& sources,
  const Time sampling_frequency,
  const Length sound_speed)
{
  receivers_ = microphones;
  sources_ = sources;
  sampling_frequency_ = sampling_frequency;
  sound_speed_ = sound_speed;
  const size_t num_microphones = receivers_.size();
  const size_t num_sources = sources_.size();
  Length max_distance = MaximumDistance(receivers_, sources_);
  Length min_distance = MinimumDistance(receivers_, sources_);

  propagation_lines_ = mcl::Vector<mcl::Vector<PropagationLine<T>>>(num_sources);

  // Define the propagation lines
  for (size_t source_i = 0; source_i < sources_.size(); ++source_i)
  {
    propagation_lines_[source_i] = mcl::Vector<PropagationLine<T>>
    (
      num_microphones,
      PropagationLine<T>(min_distance, sampling_frequency, max_distance));
    for (size_t mic_i = 0; mic_i < receivers_.size(); ++mic_i)
    {
      Length distance = Distance(sources_[source_i].position(), receivers_[mic_i].position());
      propagation_lines_[source_i][mic_i].SetDistance(distance);
    }
  }
}





template<typename T>
void FreeFieldSim<T>::Run(
  const Buffer<T>& input,
  Buffer<T>& output) noexcept
{
  const size_t num_input_samples = input.num_samples();
  const size_t num_output_sampels = output.num_samples();
  ASSERT(input.num_channels() == sources_.size());
  ASSERT(output.num_channels() == receivers_.size());

  for (size_t sample_id = 0; sample_id < num_output_sampels; ++sample_id)
  {
    for (size_t source_i = 0; source_i < sources_.size(); ++source_i)
    {
      size_t first_channel_id = 0;
      for (size_t mic_i = 0; mic_i < receivers_.size(); ++mic_i)
      {
        T next_input_sample =
          (sample_id < num_input_samples)
          ? input.GetSample(source_i, sample_id)
          : T(0.0);
        propagation_lines_[source_i][mic_i].Write(next_input_sample);
        T next_output_sample = propagation_lines_[source_i][mic_i].Read();
        
        Buffer<T> temp = MakeBufferReference
        (
          output,
          sample_id, 1,
          first_channel_id, 1);
        receivers_[mic_i].ReceiveAdd
        (
          mcl::UnaryVector<Sample>(next_output_sample),
          sources_[source_i].position(),
          source_i,
          temp);
        first_channel_id += receivers_[mic_i].GetNumChannels();
      }
    }

    // Pass to next time sample (tick propagation lines).
    Tick();
  }
}


template<typename T>
void FreeFieldSim<T>::Tick()
{
  for (size_t mic_i = 0; mic_i < receivers_.size(); ++mic_i)
  {
    for (size_t source_i = 0; source_i < sources_.size(); ++source_i)
    {
      propagation_lines_[source_i][mic_i].Tick();
    }
  }
}


template<typename T>
mcl::Vector<Length>
FreeFieldSim<T>::AllDistances(
  const mcl::Vector<Receiver<T>>& microphones,
  const mcl::Vector<Source>& sources)
{
  const size_t num_microphones = microphones.size();
  const size_t num_sources = sources.size();

  mcl::Vector<Length> distances(num_microphones*num_sources);
  size_t k = 0;
  for (size_t mic_index = 0; mic_index < num_microphones; ++mic_index)
  {
    for (size_t source_index = 0; source_index < num_sources; ++source_index)
    {
      distances[k++] =
        Distance
        (
          microphones[mic_index].position(),
         sources[source_index].position());
    }
  }

  return distances;
}


template<typename T>
Length FreeFieldSim<T>::MinimumDistance(
  const mcl::Vector<Receiver<T>>& microphones,
  const mcl::Vector<Source>& sources)
{
  return mcl::Min(AllDistances(microphones, sources));
}


template<typename T>
Length FreeFieldSim<T>::MaximumDistance(
  const mcl::Vector<Receiver<T>>& microphones,
  const mcl::Vector<Source>& sources)
{
  return mcl::Max(AllDistances(microphones, sources));
}
} // namespace sal
