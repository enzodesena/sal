/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: 
 Niccolo' Antonello
 Enzo De Sena, enzodesena@me.com
 */

#include "fdtd.h"
#include "salconstants.h"

using sal::Time;
using sal::Length;
using sal::Sample;
using mcl::IirFilter;
using sal::UInt;
using sal::Int;
using sal::Source;
using sal::Microphone;
using mcl::Point;
using sal::Time;
using sal::Sample;

namespace sal {


  
Fdtd::Fdtd(Room* const room,
           Source* const source,
           Microphone* const microphone,
           const Time sampling_frequency,
           sal::Sample xi,
           sal::Sample lmb) :
        room_(room), source_(source), microphone_(microphone),
        sampling_frequency_(sampling_frequency),
        xi_(xi), lmb_(lmb) {}
  
  

  
sal::Signal Fdtd::RunFdtd(UInt Nx, UInt Ny, UInt Nz,
                          UInt Nt,
                          std::vector<std::vector<std::vector<sal::Int> > > G,
                          Sample xi,
                          std::vector<sal::Sample> s,
                          Sample lmb,
                          UInt pos_s_x, UInt pos_s_y, UInt pos_s_z,
                          UInt pos_m_x, UInt pos_m_y, UInt pos_m_z) {
  
    
  std::vector<sal::Sample> p_out(Nt, 0);
  
  std::vector<std::vector<std::vector<double> > > p_0;
  std::vector<std::vector<std::vector<double> > > p_1;
  std::vector<std::vector<std::vector<double> > > p_2;
  Fdtd::Initialise3DArray<double>(p_0, Nx+2, Ny+2, Nz+2);
  Fdtd::Initialise3DArray<double>(p_1, Nx+2, Ny+2, Nz+2);
  Fdtd::Initialise3DArray<double>(p_2, Nx+2, Ny+2, Nz+2);
  
  double lmb_2 = pow(lmb,2.0);
  
  for (UInt n=2; n<=Nt; n++) {
    //std::cout<<"Running FDTD sample n."<<n<<std::endl;
    for (UInt l=2; l<=(Nx+1); l++) {
      for (UInt m=2; m<=(Ny+1); m++) {
        for (UInt i=2; i<=(Nz+1); i++) {
          Int K = G[l-1][m-1][i-1];
          
          if (K != 0) {
            double KK = (double)K;
            double beta = (6.0-KK)/(2.0*xi);
            
            p_2[l-1][m-1][i-1] = 1.0/(1.0+lmb*beta)*( (2.0-KK*lmb_2)*p_1[l-1][m-1][i-1]
                                         +lmb_2*(p_1[l][m-1][i-1]+p_1[l-2][m-1][i-1]
                                                 +p_1[l-1][m][i-1]+p_1[l-1][m-2][i-1]
                                                 +p_1[l-1][m-1][i]+p_1[l-1][m-1][i-2])
                                          +(lmb*beta-1.0)*p_0[l-1][m-1][i-1]);
            
            if (pos_s_x == l && pos_s_y == m && pos_s_z == i) {
              p_2[l-1][m-1][i-1] += s[n-2]; // Soft source
            }
          }
        }
      }
    }
    
    p_0 = p_1;
    p_1 = p_2;

    p_out[n-1] = p_1[pos_m_x-1][pos_m_y-1][pos_m_z-1];
  }
  
  return p_out;
}


void Fdtd::Run() {
  
  double curant_number = 1.0/sqrt(3.0);
  
  sal::Signal s = source_->stream()->PullAll();
  
  double spatial_frequency = SOUND_SPEED/(curant_number*sampling_frequency_);
  
  UInt Nx = (UInt) round(((CuboidRoom*)room_)->x()/spatial_frequency);
  UInt Ny = (UInt) round(((CuboidRoom*)room_)->y()/spatial_frequency);
  UInt Nz = (UInt) round(((CuboidRoom*)room_)->z()/spatial_frequency);
  
  UInt pos_s_x = (UInt) round(source_->position().x()/spatial_frequency)+1;
  UInt pos_s_y = (UInt) round(source_->position().y()/spatial_frequency)+1;
  UInt pos_s_z = (UInt) round(source_->position().z()/spatial_frequency)+1;
  
  UInt pos_m_x = (UInt) round(microphone_->position().x()/spatial_frequency)+1;
  UInt pos_m_y = (UInt) round(microphone_->position().y()/spatial_frequency)+1;
  UInt pos_m_z = (UInt) round(microphone_->position().z()/spatial_frequency)+1;
  
  rir_ = Fdtd::RunFdtd(Nx, Ny, Nz,
                       s.size(),
                       CreateGeometry(Nx, Ny, Nz),
                       xi_,
                       s,
                       lmb_,
                       pos_s_x, pos_s_y, pos_s_z,
                       pos_m_x, pos_m_y, pos_m_z);
}
  
  
std::vector<std::vector<std::vector<sal::Int> > >
Fdtd::CreateGeometry(UInt Nx, UInt Ny, UInt Nz) {
  // K = 6 air, K = 5 face, K = 4 edge, K = 3 corner
  std::vector<std::vector<std::vector<sal::Int> > > G;
  
  
  Nx = Nx+2;
  Ny = Ny+2;
  Nz = Nz+2;
  
  Initialise3DArray<sal::Int>(G, Nx, Ny, Nz);
  
  
  // G = 6.*ones(Nx,Ny,Nz); %K = 6 air
  for (UInt i = 0; i < Nx; ++i) {
    for (UInt j = 0; j < Ny; ++j) {
      for (UInt k = 0; k < Nz; ++k) {
        G[i][j][k] = 6;
      }
    }
  }
  
  
  //    %K = 5
  //    G(2,:,:) = 5;
  //    G(Nx-1,:,:) = 5;
  for (UInt j = 0; j < Ny; ++j) {
    for (UInt k = 0; k < Nz; ++k) {
      G[1][j][k] = 5;
      G[Nx-2][j][k] = 5;
    }
  }
  
  
  //    G(:,2,:) = 5;
  //    G(:,Ny-1,:) = 5;
  for (UInt i = 0; i < Nx; ++i) {
    for (UInt k = 0; k < Nz; ++k) {
      G[i][1][k] = 5;
      G[i][Ny-2][k] = 5;
    }
  }
  
  
  //    G(:,:,2) = 5;
  //    G(:,:,Nz-1) = 5;
  for (UInt i = 0; i < Nx; ++i) {
    for (UInt j = 0; j < Ny; ++j) {
      G[i][j][1] = 5;
      G[i][j][Nz-2] = 5;
    }
  }
  
  
  // % K = 4
  //
  //    G(2,2,:) = 4;
  //    G(2,Ny-1,:) = 4;
  //    G(Nx-1,Ny-1,:) = 4;
  //    G(Nx-1,2,:) = 4;
  for (UInt k = 0; k < Nz; ++k) {
    G[1][1][k] = 4;
    G[1][Ny-2][k] = 4;
    G[Nx-2][Ny-2][k] = 4;
    G[Nx-2][1][k] = 4;
  }
  
  
  //    G(:,2,2) = 4;
  //    G(:,2,Nz-1) = 4;
  //    G(:,Ny-1,Nz-1) = 4;
  //    G(:,Ny-1,2) = 4;
  for (UInt i = 0; i < Nx; ++i) {
    G[i][1][1] = 4;
    G[i][1][Nz-2] = 4;
    G[i][Ny-2][Nz-2] = 4;
    G[i][Ny-2][1] = 4;
  }
  
  //    G(2,:,2) = 4;
  //    G(2,:,Nz-1) = 4;
  //    G(Nx-1,:,Nz-1) = 4;
  //    G(Nx-1,:,2) = 4;
  for (UInt j = 0; j < Ny; ++j) {
    G[1][j][1] = 4;
    G[1][j][Nz-2] = 4;
    G[Nx-2][j][1] = 4;
    G[Nx-2][j][Nz-2] = 4;
  }
  
  
  //    % K = 3
  //    G(2,2,2) = 3;
  //    G(Nx-1,2,2) =3;
  //    G(2,Ny-1,2) =3;
  //    G(2,2,Nz-1) =3;
  //    G(Nx-1,Ny-1,2) =3;
  //    G(Nx-1,2,Nz-1) =3;
  //    G(2,Ny-1,Nz-1) =3;
  //    G(Nx-1,Ny-1,Nz-1) =3;
  G[1][1][1] = 3;
  G[Nx-2][1][1] =3;
  G[1][Ny-2][1] =3;
  G[1][1][Nz-2] =3;
  G[Nx-2][Ny-2][1] =3;
  G[Nx-2][1][Nz-2] =3;
  G[1][Ny-2][Nz-2] =3;
  G[Nx-2][Ny-2][Nz-2] =3;
  
  
  
  //    % %K = 0
  
  
  //    G(1,:,:) = 0;
  //    G(Nx,:,:) = 0;
  for (UInt j = 0; j < Ny; ++j) {
    for (UInt k = 0; k < Nz; ++k) {
      G[0][j][k] = 0;
      G[Nx-1][j][k] = 0;
    }
  }
  
  //
  //    G(:,1,:) = 0;
  //    G(:,Ny,:) = 0;
  for (UInt i = 0; i < Nx; ++i) {
    for (UInt k = 0; k < Nz; ++k) {
      G[i][0][k] = 0;
      G[i][Ny-1][k] = 0;
    }
  }
  
  
  //
  //    G(:,:,1) = 0;
  //    G(:,:,Nz) = 0;
  for (UInt i = 0; i < Nx; ++i) {
    for (UInt j = 0; j < Ny; ++j) {
      G[i][j][0] = 0;
      G[i][j][Nz-1] = 0;
    }
  }
  
  // Print out
  //    for (UInt k=0; k<Nz; k++) {
  //      for (UInt i = 0; i < Nx; ++i) {
  //        for (UInt j = 0; j < Ny; ++j) {
  //          std::cout<<G[i][j][k]<<" ";
  //        }
  //        std::cout<<std::endl;
  //      }
  //      std::cout<<std::endl;
  //      std::cout<<std::endl;
  //    }
  
  return G;
}



} // namespace sal
