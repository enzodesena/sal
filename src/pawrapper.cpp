/*
 pawrapper.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "pawrapper.h"

namespace sal {
  
  
PaError PaWrapper::Init() { return Pa_Initialize(); }
  
  
void PaWrapper::PrintDevicesInfo() {
  Init();
  
  const PaDeviceInfo *deviceInfo;
  int numDevices = Pa_GetDeviceCount();
  for(int i=0; i<numDevices; ++i) {
    deviceInfo = Pa_GetDeviceInfo(i);
    std::cout<<"Device id:"<<i<<", name: "<<deviceInfo->name<<
    ", max output channels: "<<deviceInfo->maxOutputChannels<<"\n";
  }
  
  Terminate();
}
  
std::vector<mcl::Int> PaWrapper::SelectChannelIds(const Int num_loudspeakers,
                                                  const Int out_dev_id) {
  Init();
  
  const PaDeviceInfo* deviceInfo(Pa_GetDeviceInfo((int) out_dev_id));
  const Int max_num_channels(deviceInfo->maxOutputChannels);
  std::vector<mcl::Int> channel_ids = std::vector<Int>(max_num_channels, -1);
  for (int i=0; i<num_loudspeakers; ++i) {
    Int channel_id;
    std::cout<<"Select channel id for mic n.: "<<i<<" (from 0 to "<<
    (max_num_channels-1)<<"): ";
    std::cin >> channel_id;
    channel_ids[channel_id] = i;
  }
  
  Terminate();
  
  return channel_ids;
}
  
Int PaWrapper::NumOutputChannels(const Int out_dev_id) {
  Init();
  const PaDeviceInfo* deviceInfo(Pa_GetDeviceInfo((int) out_dev_id));
  const Int max_num_channels(deviceInfo->maxOutputChannels);
  Terminate();
  return max_num_channels;
}
  

PaWrapper::PaWrapper(Time sampling_frequency,
                     Int out_dev_num,
                     std::vector<Int> channel_ids) :
          channel_ids_(channel_ids) {
  //if (decoder->num_loudspeakers() != channel_ids.size()) { ASSERT(false); }
  
  Init();
  
  const PaDeviceInfo* device_info = Pa_GetDeviceInfo((int) out_dev_num);
            
  PaStreamParameters output_parameters;
  bzero(&output_parameters, sizeof(output_parameters));
  output_parameters.channelCount = device_info->maxOutputChannels;
  output_parameters.device = (int) out_dev_num;
  output_parameters.hostApiSpecificStreamInfo = NULL;
  output_parameters.sampleFormat = paFloat32;
  output_parameters.suggestedLatency = device_info->defaultLowOutputLatency;
  //See you specific host's API docs for info on using this field
  output_parameters.hostApiSpecificStreamInfo = NULL;
  
  /* Open an audio I/O stream. */
  Pa_OpenStream(&stream_,
                NULL,
                &output_parameters,
                sampling_frequency,
                paFramesPerBufferUnspecified,
                paNoFlag, //flags that can be used to define dither, clip settings and more
                NULL, //your callback function
                NULL); //data to be passed to callback. In C++, it is frequently (void *)this
}
  
PaError PaWrapper::StartStream() { return Pa_StartStream(stream_); }

  
PaError PaWrapper::WriteStream(const MultichannelBuffer& output_buffer) {
  const Int num_samples = output_buffer.num_samples();
  sal::Int max_num_channels = channel_ids_.size();
  const Int block_length = num_samples*max_num_channels;
  float sample_block[block_length];
   
  for (Int i=0; i<num_samples; ++i) {
    for (Int j=0; j<max_num_channels; ++j) {
      float sample;
      if (channel_ids_[j] == -1 ||
          i >= output_buffer.num_samples()) {
        sample = 0.0;
      } else {
        sample = (float) output_buffer.GetSample(channel_ids_[j], i);
      }
      ASSERT(i*max_num_channels+j<block_length);
      sample_block[i*max_num_channels+j] = sample;
    }
  }
  
  return Pa_WriteStream(stream_, sample_block, num_samples);
}
  
PaError PaWrapper::StopStream() { return Pa_CloseStream(stream_); }
  
void PaWrapper::PrintError(PaError err) {
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
}
  
PaError PaWrapper::Terminate() {
  return Pa_Terminate();
}
  
PaWrapper::~PaWrapper() {
  Terminate();
}
  
} // namespace sal
