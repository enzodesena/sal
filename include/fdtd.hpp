/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: 
 Niccolo' Antonello
 Enzo De Sena, enzodesena@gmail.com
 */

#pragma once

#include "cuboidroom.hpp"
#include "source.hpp"
#include "receiver.hpp"

namespace sal
{
template<typename T>
class Fdtd
{
private:
  Triplet room_dimensions_;
  Triplet source_position_;
  Triplet receiver_position_;
  
  T xi_;
  T lmb_;

  Time sampling_frequency_;

  mcl::Vector<T> rir_;

  static
  Signal<T> RunFdtd(
    const size_t Nx,
    const size_t Ny,
    const size_t Nz,
    const mcl::Vector<mcl::Vector<mcl::Vector<Int>>> G,
    const T xi,
    const mcl::Vector<T> input,
    const T lmb,
    const size_t position_x,
    const size_t position_y,
    const size_t position_z,
    const size_t position_m_x,
    const size_t position_m_y,
    const size_t position_m_z);

  template<typename G>
  static void Initialise3DArray(
    mcl::Vector<mcl::Vector<mcl::Vector<G>>>& array,
    size_t size_x,
    size_t size_y,
    size_t size_z)
  {
    array = mcl::Vector<mcl::Vector<mcl::Vector<G>>>(size_x);
    for (size_t i = 0; i < size_x; ++i)
    {
      array[i] = mcl::Vector<mcl::Vector<G>>(size_y);
      for (size_t j = 0; j < size_y; ++j)
      {
        array[i][j] = mcl::Vector<G>(size_z);
      }
    }
  }


public:
  Fdtd(
    const CuboidRoom<T>& room,
    const Source& source,
    const Receiver<T>& microphone,
    const Time sampling_frequency,
    const T xi,
    const T lmb);

  void Run(
    const mcl::Vector<T>& input,
    Receiver<T>& receiver,
    Buffer<T>& output_buffer);


  Signal<T> rir() const
  {
    return rir_;
  }


  static
  mcl::Vector<mcl::Vector<mcl::Vector<sal::Int>>>
  CreateGeometry(
    size_t Nx,
    size_t Ny,
    size_t Nz);

  static bool Test();
  static Time SimulationTime();
};
} // namespace raily

#include "fdtd.ipp"
