/*
 pawrapper.cpp
 Spatial Audio Library (SAL)
 Copyright (c) 2016, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#include "pawrapper.h"
#include "mcl.h"

namespace sal {
  
PaWrapper::PaWrapper(Decoder& decoder,
                     Time sampling_frequency,
                     UInt frames_per_buffer,
                     Int out_dev_num,
                     std::vector<Int> channel_ids) {
  decoder_ = &decoder;
  frames_per_buffer_ = frames_per_buffer;
  channel_ids_ = channel_ids;
  
  
  PaError err;
  
  std::cout<<"Starting portaudio...\n"; std::cout.flush();
  
  err = Pa_Initialize();
  if( err != paNoError ) { PaErrorHandler(err); }
  
  
//  const   PaDeviceInfo *deviceInfo;
//  int numDevices = Pa_GetDeviceCount();
//  for(int i=0; i<numDevices; ++i) {
//    deviceInfo = Pa_GetDeviceInfo( i );
//    std::cout<<"Device id:"<<i<<", name: "<<deviceInfo->name<<
//    ", max output channels: "<<deviceInfo->maxOutputChannels<<"\n";
//  }
//  std::cout<<"Select the device id: ";
//  int outDevNum;
//  std::cin >> outDevNum;
  
//  UInt num_loudspeakers = decoder_->num_loudspeakers();
//  const PaDeviceInfo* deviceInfo(Pa_GetDeviceInfo((int) out_dev_num));
//  const UInt max_num_channels(deviceInfo->maxOutputChannels);
//  channel_ids_ = std::vector<Int>(max_num_channels, -1);
//  for (int i=0; i<num_loudspeakers; ++i) {
//    UInt channel_id;
//    std::cout<<"Select channel id for mic n.: "<<i<<" (from 0 to "<<
//    (max_num_channels-1)<<"): ";
//    std::cin >> channel_id;
//    channel_ids_[channel_id] = i;
//  }
  
  PaStreamParameters outputParameters;
  bzero( &outputParameters, sizeof( outputParameters ) );
  outputParameters.channelCount = 8;
  outputParameters.device = (int) out_dev_num;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency =
        Pa_GetDeviceInfo((int) out_dev_num)->defaultLowOutputLatency ;
  //See you specific host's API docs for info on using this field
  outputParameters.hostApiSpecificStreamInfo = NULL;
  
  /* Open an audio I/O stream. */
  err = Pa_OpenStream(&stream_,
                      NULL,
                      &outputParameters,
                      sampling_frequency,
                      frames_per_buffer,
                      paNoFlag, //flags that can be used to define dither, clip settings and more
                      NULL, //your callback function
                      NULL); //data to be passed to callback. In C++, it is frequently (void *)this
  if(err != paNoError) { PaErrorHandler(err); }
}
  
void PaWrapper::StartStream() {
  PaError err = Pa_StartStream(stream_);
  if(err != paNoError) { PaErrorHandler(err); }
}

void PaWrapper::WriteStream() {
  sal::UInt max_num_channels = channel_ids_.size();
  float sample_block[frames_per_buffer_*max_num_channels];
  
  sal::UInt num_channels(0);
  for (mcl::UInt i=0; i<max_num_channels; ++i) {
    if (channel_ids_[i] != -1) { ++num_channels; }
  }
  
  mcl::Int i = 0;
  while(! decoder_->stream(0)->IsEmpty()) {
    for (int j=0; j<max_num_channels; ++j) {
      float sample = (channel_ids_[j] != -1) ?
      (float) (200.0*decoder_->stream(channel_ids_[j])->Pull()) : 0.0;
      sample_block[i*max_num_channels+j] = sample;
    }
    i++;
  }
  
  PaError err = Pa_WriteStream(stream_, sample_block, frames_per_buffer_);
  
  if( err != paNoError ) { PaErrorHandler(err); }
}
  
void PaWrapper::StopStream() {
  if(stream_) {
    Pa_AbortStream(stream_);
    Pa_CloseStream(stream_);
  }
}
  
void PaWrapper::PaErrorHandler(PaError err) {
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  StopStream();
  Pa_Terminate();
  exit(1);
}
  
  
PaWrapper::~PaWrapper() {
  StopStream();
  Pa_Terminate();
}
  
} // namespace sal