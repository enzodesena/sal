/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include "fdtd.hpp"

namespace sal
{
template<typename T>
bool Fdtd<T>::Test()
{
  unsigned int Nt = 20;
  mcl::Vector<T> s(Nt);
  s[1] = 1.0;

  unsigned int Nx = 4;
  unsigned int Ny = 4;
  unsigned int Nz = 4;

  mcl::Vector<sal::Sample> p_out = Fdtd::RunFdtd
  (
    Nx,
    Ny,
    Nz,
    Fdtd::CreateGeometry(Nx, Ny, Nz),
    10,
    s,
    1.0 / sqrt(3.0),
    3,
    3,
    3,
    // source
    4,
    4,
    4); // microphone

  mcl::Vector<sal::Sample> rir_cmp(20);
  rir_cmp[0] = 0;
  rir_cmp[1] = 0;
  rir_cmp[2] = 0;
  rir_cmp[3] = 0;
  rir_cmp[4] = 0;
  rir_cmp[5] = 0.2222;
  rir_cmp[6] = 0;
  rir_cmp[7] = -0.1687;
  rir_cmp[8] = 0.2281;
  rir_cmp[9] = 0.0877;
  rir_cmp[10] = 0.0478;
  rir_cmp[11] = 0.2823;
  rir_cmp[12] = 0.0120;
  rir_cmp[13] = 0.2293;
  rir_cmp[14] = 0.2840;
  rir_cmp[15] = -0.2871;
  rir_cmp[16] = 0.0954;
  rir_cmp[17] = 0.5274;
  rir_cmp[18] = 0.1049;
  rir_cmp[19] = -0.0675;

  ASSERT(mcl::IsApproximatelyEqual(rir_cmp, p_out, 0.001));

  return true;
}


template<typename T>
sal::Time Fdtd<T>::SimulationTime()
{
  //  clock_t launch=clock();
  unsigned int Nt = 1000;
  mcl::Vector<T> s(Nt);
  s[1] = 1.0;

  unsigned int Nx = 50;
  unsigned int Ny = 50;
  unsigned int Nz = 50;

  clock_t launch = clock();
  Fdtd::RunFdtd
  (
    Nx,
    Ny,
    Nz,
    Fdtd::CreateGeometry(Nx, Ny, Nz),
    10,
    s,
    1.0 / sqrt(3.0),
    3,
    3,
    3, // source
    4,
    4,
    4); // microphone
  clock_t done = clock();

  return (done - launch) / ((sal::Time)CLOCKS_PER_SEC);
}
} // namespace sal
