/*
 Spatial Audio Library (SAL)
 Copyright (c) 2013-2018, Enzo De Sena
 All rights reserved.
 
 Authors: 
 Niccolo' Antonello
 Enzo De Sena, enzodesena@gmail.com
 */

#include "fdtd.hpp"
#include "salconstants.hpp"

using sal::Time;
using sal::Length;
using sal::Sample;
using mcl::IirFilter;
using sal::Int;
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
  
  

  
sal::Signal Fdtd::RunFdtd(Int Nx, Int Ny, Int Nz,
                          Int Nt,
                          mcl::Vector<std::vector<mcl::Vector<sal::Int> > > G,
                          Sample xi,
                          const Sample* signal,
                          Sample lmb,
                          Int pos_s_x, Int pos_s_y, Int pos_s_z,
                          Int pos_m_x, Int pos_m_y, Int pos_m_z) {
  
    
  mcl::Vector<sal::Sample> p_out(Nt, 0);
  
  mcl::Vector<std::vector<mcl::Vector<double> > > p_0;
  mcl::Vector<std::vector<mcl::Vector<double> > > p_1;
  mcl::Vector<std::vector<mcl::Vector<double> > > p_2;
  Fdtd::Initialise3DArray<double>(p_0, Nx+2, Ny+2, Nz+2);
  Fdtd::Initialise3DArray<double>(p_1, Nx+2, Ny+2, Nz+2);
  Fdtd::Initialise3DArray<double>(p_2, Nx+2, Ny+2, Nz+2);
  
  double lmb_2 = pow(lmb,2.0);
  
  for (Int n=2; n<=Nt; n++) {
    //std::cout<<"Running FDTD sample n."<<n<<std::endl;
    for (Int l=2; l<=(Nx+1); l++) {
      for (Int m=2; m<=(Ny+1); m++) {
        for (Int i=2; i<=(Nz+1); i++) {
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
              p_2[l-1][m-1][i-1] += signal[n-2]; // Soft source
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


void Fdtd::Run(const MonoBuffer& input_buffer, Buffer& output_buffer) {
  ASSERT(input_buffer.num_samples() == output_buffer.num_samples());
  Triplet dimensions = ((CuboidRoom*)room_)->dimensions();
  
  double curant_number = 1.0/sqrt(3.0);
  
  double spatial_frequency = SOUND_SPEED/(curant_number*sampling_frequency_);
  
  Int Nx = mcl::RoundToInt(dimensions.x()/spatial_frequency);
  Int Ny = mcl::RoundToInt(dimensions.y()/spatial_frequency);
  Int Nz = mcl::RoundToInt(dimensions.z()/spatial_frequency);
  
  Int pos_s_x = mcl::RoundToInt(source_->position().x()/spatial_frequency)+1;
  Int pos_s_y = mcl::RoundToInt(source_->position().y()/spatial_frequency)+1;
  Int pos_s_z = mcl::RoundToInt(source_->position().z()/spatial_frequency)+1;
  
  Int pos_m_x = mcl::RoundToInt(microphone_->position().x()/spatial_frequency)+1;
  Int pos_m_y = mcl::RoundToInt(microphone_->position().y()/spatial_frequency)+1;
  Int pos_m_z = mcl::RoundToInt(microphone_->position().z()/spatial_frequency)+1;
  
  rir_ = Fdtd::RunFdtd(Nx, Ny, Nz,
                       input_buffer.num_samples(),
                       CreateGeometry(Nx, Ny, Nz),
                       xi_,
                       input_buffer.GetReadPointer(),
                       lmb_,
                       pos_s_x, pos_s_y, pos_s_z,
                       pos_m_x, pos_m_y, pos_m_z);
}
  
  
mcl::Vector<std::vector<mcl::Vector<sal::Int> > >
Fdtd::CreateGeometry(Int Nx, Int Ny, Int Nz) {
  // K = 6 air, K = 5 face, K = 4 edge, K = 3 corner
  mcl::Vector<std::vector<mcl::Vector<sal::Int> > > G;
  
  
  Nx = Nx+2;
  Ny = Ny+2;
  Nz = Nz+2;
  
  Initialise3DArray<sal::Int>(G, Nx, Ny, Nz);
  
  
  // G = 6.*ones(Nx,Ny,Nz); %K = 6 air
  for (Int i = 0; i < Nx; ++i) {
    for (Int j = 0; j < Ny; ++j) {
      for (Int k = 0; k < Nz; ++k) {
        G[i][j][k] = 6;
      }
    }
  }
  
  
  //    %K = 5
  //    G(2,:,:) = 5;
  //    G(Nx-1,:,:) = 5;
  for (Int j = 0; j < Ny; ++j) {
    for (Int k = 0; k < Nz; ++k) {
      G[1][j][k] = 5;
      G[Nx-2][j][k] = 5;
    }
  }
  
  
  //    G(:,2,:) = 5;
  //    G(:,Ny-1,:) = 5;
  for (Int i = 0; i < Nx; ++i) {
    for (Int k = 0; k < Nz; ++k) {
      G[i][1][k] = 5;
      G[i][Ny-2][k] = 5;
    }
  }
  
  
  //    G(:,:,2) = 5;
  //    G(:,:,Nz-1) = 5;
  for (Int i = 0; i < Nx; ++i) {
    for (Int j = 0; j < Ny; ++j) {
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
  for (Int k = 0; k < Nz; ++k) {
    G[1][1][k] = 4;
    G[1][Ny-2][k] = 4;
    G[Nx-2][Ny-2][k] = 4;
    G[Nx-2][1][k] = 4;
  }
  
  
  //    G(:,2,2) = 4;
  //    G(:,2,Nz-1) = 4;
  //    G(:,Ny-1,Nz-1) = 4;
  //    G(:,Ny-1,2) = 4;
  for (Int i = 0; i < Nx; ++i) {
    G[i][1][1] = 4;
    G[i][1][Nz-2] = 4;
    G[i][Ny-2][Nz-2] = 4;
    G[i][Ny-2][1] = 4;
  }
  
  //    G(2,:,2) = 4;
  //    G(2,:,Nz-1) = 4;
  //    G(Nx-1,:,Nz-1) = 4;
  //    G(Nx-1,:,2) = 4;
  for (Int j = 0; j < Ny; ++j) {
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
  for (Int j = 0; j < Ny; ++j) {
    for (Int k = 0; k < Nz; ++k) {
      G[0][j][k] = 0;
      G[Nx-1][j][k] = 0;
    }
  }
  
  //
  //    G(:,1,:) = 0;
  //    G(:,Ny,:) = 0;
  for (Int i = 0; i < Nx; ++i) {
    for (Int k = 0; k < Nz; ++k) {
      G[i][0][k] = 0;
      G[i][Ny-1][k] = 0;
    }
  }
  
  
  //
  //    G(:,:,1) = 0;
  //    G(:,:,Nz) = 0;
  for (Int i = 0; i < Nx; ++i) {
    for (Int j = 0; j < Ny; ++j) {
      G[i][j][0] = 0;
      G[i][j][Nz-1] = 0;
    }
  }
  
  // Print out
  //    for (Int k=0; k<Nz; k++) {
  //      for (Int i = 0; i < Nx; ++i) {
  //        for (Int j = 0; j < Ny; ++j) {
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
