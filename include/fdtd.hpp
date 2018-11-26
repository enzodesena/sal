/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: 
 Niccolo' Antonello
 Enzo De Sena, enzodesena@gmail.com
 */


#ifndef SAL_FDTD_H
#define SAL_FDTD_H


#include "cuboidroom.hpp"
#include "source.hpp"
#include "microphone.hpp"

namespace sal {
  
class Fdtd {
private:
  Room* const room_;
  sal::Source* const source_;
  sal::Microphone* const microphone_;
  sal::Sample xi_;
  sal::Sample lmb_;
  
  sal::Time sampling_frequency_;
  
  mcl::Vector<sal::Sample> rir_;
  
  static
  sal::Signal RunFdtd(sal::Int Nx, sal::Int Ny, sal::Int Nz,
                      sal::Int Nt,
                      mcl::Vector<std::vector<mcl::Vector<sal::Int> > > G,
                      sal::Sample xi,
                      const Sample* signal,
                      sal::Sample lmb,
                      sal::Int position_x,
                      sal::Int position_y,
                      sal::Int position_z,
                      sal::Int position_m_x,
                      sal::Int position_m_y,
                      sal::Int position_m_z);
  
  template<class T>
  static
  void
  Initialise3DArray(mcl::Vector<std::vector<mcl::Vector<T> > >& array,
                    sal::Int size_x, sal::Int size_y, sal::Int size_z) {
    array.resize(size_x);
    for (sal::Int i = 0; i < size_x; ++i) {
      array[i].resize(size_y);
      for (sal::Int j = 0; j < size_y; ++j) {
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
  mcl::Vector<std::vector<mcl::Vector<sal::Int> > >
  CreateGeometry(sal::Int Nx, sal::Int Ny, sal::Int Nz);
  
  static bool Test();
  static sal::Time SimulationTime();
};
  
} // namespace raily


#endif
