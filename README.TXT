Spatial Audio Library (SAL) is a C++ library containing a number of (I hope useful) objects for spatial audio. These include an interactive HRTF filter (supporting both measured Kemar database and Duda’s spherical head model), Ambisonics encoders and decoders, WAV handling routines (both read and write) etc..

This library requires linking with the other C++ library of mine MCL (see desena.org). If you use Xcode, like I do, it should (hopefully) all run smoothly.

The whole library works based on the concept of streams. Unfortunately, I haven’t had time to write a proper user guide yet. For the time being, I am afraid you’ll have to rely on the source code comments and the code itself, which I hope is quite self-explanatory. A useful source of information is the unit-testing files (directory test/) included in the distribution. 

The library was written following Google's C++ style guide:
http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

A sample of how to use the library is:

const UInt N = 2; // Ambisonics order
  
// Position an ambisonics microphone in 0,0,0
AmbisonicsMic mic_a(Point(0.0,0.0,0.0), 0.0, 0.0, 0.0, N);
BFormatStream* stream_a = mic_a.stream();
  
// Record a single sound sample for a source in 
// the direction of Point(1.0, 0.0, 0.0)
// (as described in the documentation, the propagation delay
// is not included in the microphone object)
Sample sample = 0.3;
mic_a.RecordPlaneWave(sample, Point(1.0, 0.0, 0.0));
// Record a second sample (automatically advances the entire simulation by one sample)
mic_a.RecordPlaneWave(0.5, Point(1.0, 0.0, 0.0));

// Print to screen the 0th degree and 0th order component of the first sample
std::cout<<stream_a->Pull(0, 0)<<std::endl;
// Print to screen the 0th degree and 0th order component of the second sample
std::cout<<stream_a->Pull(0, 0)<<std::endl;
