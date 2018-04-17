/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: 
 Niccolo' Antonello
 Enzo De Sena, enzodesena@me.com
 */


#ifndef SAL_FDTD_H
#define SAL_FDTD_H


#include "cuboidroom.h"
#include "source.h"
#include "microphone.h"

#ifdef SAL_EXPORTS
  #define SAL_API __declspec(dllexport)
#else
  #define SAL_API
#endif

namespace sal {
  
class SAL_API Fdtd {
private:
  Room* const room_;
  sal::Source* const source_;
  sal::Microphone* const microphone_;
  sal::Sample xi_;
  sal::Sample lmb_;
  
  sal::Time sampling_frequency_;
  
  std::vector<sal::Sample> rir_;
  
  static
  sal::Signal RunFdtd(sal::UInt Nx, sal::UInt Ny, sal::UInt Nz,
                      sal::UInt Nt,
                      std::vector<std::vector<std::vector<sal::Int> > > G,
                      sal::Sample xi,
                      const Sample* signal,
                      sal::Sample lmb,
                      sal::UInt position_x,
                      sal::UInt position_y,
                      sal::UInt position_z,
                      sal::UInt position_m_x,
                      sal::UInt position_m_y,
                      sal::UInt position_m_z);
  
  template<class T>
  static
  void
  Initialise3DArray(std::vector<std::vector<std::vector<T> > >& array,
                    sal::UInt size_x, sal::UInt size_y, sal::UInt size_z) {
    array.resize(size_x);
    for (sal::UInt i = 0; i < size_x; ++i) {
      array[i].resize(size_y);
      for (sal::UInt j = 0; j < size_y; ++j) {
        array[i][j].resize(size_z);
      }
    }
  }
  
public:
  Fdtd(Room* const room,
       sal::Source* const source,
       sal::Microphone* const microphone,
       const sal::Time sampling_frequency,
       sal::Sample xi,
       sal::Sample lmb);
  
  
  void Run(const MonoBuffer& input_buffer, Buffer& output_buffer);
  
  sal::Signal rir() const { return rir_; }
  
  static
  std::vector<std::vector<std::vector<sal::Int> > >
  CreateGeometry(sal::UInt Nx, sal::UInt Ny, sal::UInt Nz);
  
  static bool Test();
  static sal::Time SimulationTime();
};
  
} // namespace raily


#endif
