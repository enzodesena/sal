/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#include "fdtd.h"


namespace sal {
  
bool Fdtd::Test() {
  
  unsigned int Nt=20;
  std::vector<sal::Sample> s=mcl::Zeros<sal::Sample>(Nt);
  s[1]=1.0;
  
  unsigned int Nx=4;
  unsigned int Ny=4;
  unsigned int Nz=4;
  
  std::vector<sal::Sample> p_out=Fdtd::RunFdtd(Nx, Ny, Nz,
                                Nt,
                                Fdtd::CreateGeometry(Nx, Ny, Nz),
                                10,
                                s,
                                1.0/sqrt(3.0),
                                3, 3, 3, // source
                                4, 4, 4); // microphone
  

  
  std::vector<sal::Sample> rir_cmp;
  rir_cmp.push_back(0);
  rir_cmp.push_back(0);
  rir_cmp.push_back(0);
  rir_cmp.push_back(0);
  rir_cmp.push_back(0);
  rir_cmp.push_back(0.2222);
  rir_cmp.push_back(0);
  rir_cmp.push_back(-0.1687);
  rir_cmp.push_back(0.2281);
  rir_cmp.push_back(0.0877);
  rir_cmp.push_back(0.0478);
  rir_cmp.push_back(0.2823);
  rir_cmp.push_back(0.0120);
  rir_cmp.push_back(0.2293);
  rir_cmp.push_back(0.2840);
  rir_cmp.push_back(-0.2871);
  rir_cmp.push_back(0.0954);
  rir_cmp.push_back(0.5274);
  rir_cmp.push_back(0.1049);
  rir_cmp.push_back(-0.0675);

  assert(mcl::IsEqual(rir_cmp, p_out, 0.001));

  
  return true;
}
  
  
sal::Time Fdtd::SimulationTime() {

  //  clock_t launch=clock();
  unsigned int Nt=1000;
  std::vector<sal::Sample> s=mcl::Zeros<sal::Sample>(Nt);
  s[1]=1.0;
  
  unsigned int Nx=50;
  unsigned int Ny=50;
  unsigned int Nz=50;
  
  
  clock_t launch=clock();
  Fdtd::RunFdtd(Nx, Ny, Nz,
             Nt,
             Fdtd::CreateGeometry(Nx, Ny, Nz),
             10,
             s,
             1.0/sqrt(3.0),
             3, 3, 3, // source
             4, 4, 4); // microphone
  clock_t done=clock();
  
  return (done - launch) / ((sal::Time) CLOCKS_PER_SEC);
  }
  
  
} // namespace sal

