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

namespace sal
{
class Fdtd
{
private:
  Room* const room_;
  Source* const source_;
  Microphone* const microphone_;
  Sample xi_;
  Sample lmb_;

  Time sampling_frequency_;

  mcl::Vector<Sample> rir_;

  static
  Signal RunFdtd(
    Int Nx,
    Int Ny,
    Int Nz,
    Int Nt,
    mcl::Vector<std::vector<mcl::Vector<Int>>> G,
    Sample xi,
    const Sample* signal,
    Sample lmb,
    Int position_x,
    Int position_y,
    Int position_z,
    Int position_m_x,
    Int position_m_y,
    Int position_m_z);

  template<class T>
  static
  void
  Initialise3DArray(
    mcl::Vector<std::vector<mcl::Vector<T>>>& array,
    Int size_x,
    Int size_y,
    Int size_z)
  {
    array.resize(size_x);
    for (Int i = 0; i < size_x; ++i)
    {
      array[i].resize(size_y);
      for (Int j = 0; j < size_y; ++j) { array[i][j].resize(size_z); }
    }
  }

public:
  Fdtd(
    Room* room,
    Source* source,
    Microphone* microphone,
    Time sampling_frequency,
    Sample xi,
    Sample lmb);

  void Run(
    const MonoBuffer& input_buffer,
    Buffer& output_buffer);

  Signal rir() const { return rir_; }

  static
  mcl::Vector<std::vector<mcl::Vector<Int>>>
  CreateGeometry(
    Int Nx,
    Int Ny,
    Int Nz);

  static bool Test();
  static Time SimulationTime();
};
} // namespace raily

#endif
