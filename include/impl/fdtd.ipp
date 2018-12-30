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


namespace sal
{
template<typename T>
Fdtd<T>::Fdtd(
  const CuboidRoom<T>& room,
  const Source& source,
  const Receiver<T>& microphone,
  const Time sampling_frequency,
  const T xi,
  const T lmb)
  : sampling_frequency_(sampling_frequency)
  , xi_(xi)
  , lmb_(lmb)
{
}


template<typename T>
Signal<T> Fdtd<T>::RunFdtd(
  const size_t Nx,
  const size_t Ny,
  const size_t Nz,
  const mcl::Vector<mcl::Vector<mcl::Vector<Int>>> G,
  const T xi,
  const mcl::Vector<T> input,
  const T lmb,
  const size_t pos_s_x,
  const size_t pos_s_y,
  const size_t pos_s_z,
  const size_t pos_m_x,
  const size_t pos_m_y,
  const size_t pos_m_z)
{
  mcl::Vector<sal::Sample> p_out(input.size(), 0);

  mcl::Vector<mcl::Vector<mcl::Vector<T>>> p_0;
  mcl::Vector<mcl::Vector<mcl::Vector<T>>> p_1;
  mcl::Vector<mcl::Vector<mcl::Vector<T>>> p_2;
  Initialise3DArray(p_0, Nx + 2, Ny + 2, Nz + 2);
  Initialise3DArray(p_1, Nx + 2, Ny + 2, Nz + 2);
  Initialise3DArray(p_2, Nx + 2, Ny + 2, Nz + 2);

  double lmb_2 = pow(lmb, 2.0);

  for (size_t n = 2; n <= input.size(); n++)
  {
    //std::cout<<"Running FDTD sample n."<<n<<std::endl;
    for (size_t l = 2; l <= (Nx + 1); l++)
    {
      for (size_t m = 2; m <= (Ny + 1); m++)
      {
        for (size_t i = 2; i <= (Nz + 1); i++)
        {
          Int K = G[l - 1][m - 1][i - 1];

          if (K != 0)
          {
            double KK = (double)K;
            double beta = (6.0 - KK) / (2.0 * xi);

            p_2[l - 1][m - 1][i - 1] = 1.0 / (1.0 + lmb * beta) * ((2.0 - KK *
                lmb_2) * p_1[l - 1][m - 1][i - 1]
              + lmb_2 * (p_1[l][m - 1][i - 1] + p_1[l - 2][m - 1][i - 1]
                + p_1[l - 1][m][i - 1] + p_1[l - 1][m - 2][i - 1]
                + p_1[l - 1][m - 1][i] + p_1[l - 1][m - 1][i - 2])
              + (lmb * beta - 1.0) * p_0[l - 1][m - 1][i - 1]);

            if (pos_s_x == l && pos_s_y == m && pos_s_z == i)
            {
              p_2[l - 1][m - 1][i - 1] += input[n - 2]; // Soft source
            }
          }
        }
      }
    }

    p_0 = p_1;
    p_1 = p_2;

    p_out[n - 1] = p_1[pos_m_x - 1][pos_m_y - 1][pos_m_z - 1];
  }

  return p_out;
}


template<typename T>
void Fdtd<T>::Run(
  const mcl::Vector<T>& input,
  Receiver<T>& receiver,
  Buffer<T>& output_buffer)
{
  ASSERT(input.size() == output_buffer.num_samples());

  double curant_number = 1.0 / sqrt(3.0);

  double spatial_frequency = SOUND_SPEED / (curant_number * sampling_frequency_
  );

  size_t Nx = mcl::RoundToInt(room_dimensions_.x() / spatial_frequency);
  size_t Ny = mcl::RoundToInt(room_dimensions_.y() / spatial_frequency);
  size_t Nz = mcl::RoundToInt(room_dimensions_.z() / spatial_frequency);

  Int pos_s_x = mcl::RoundToInt(
    source_position_.x() / spatial_frequency) + 1;
  Int pos_s_y = mcl::RoundToInt(
    source_position_.y() / spatial_frequency) + 1;
  Int pos_s_z = mcl::RoundToInt(
    source_position_.z() / spatial_frequency) + 1;

  Int pos_m_x = mcl::RoundToInt(
    receiver_position_.x() / spatial_frequency) + 1;
  Int pos_m_y = mcl::RoundToInt(
    receiver_position_.y() / spatial_frequency) + 1;
  Int pos_m_z = mcl::RoundToInt(
    receiver_position_.z() / spatial_frequency) + 1;

  rir_ = Fdtd<T>::RunFdtd
  (
    Nx,
    Ny,
    Nz,
    CreateGeometry(Nx, Ny, Nz),
    xi_,
    input,
    lmb_,
    pos_s_x,
    pos_s_y,
    pos_s_z,
    pos_m_x,
    pos_m_y,
    pos_m_z);
}


template<typename T>
mcl::Vector<mcl::Vector<mcl::Vector<sal::Int>>>
Fdtd<T>::CreateGeometry(
  size_t Nx,
  size_t Ny,
  size_t Nz)
{
  // K = 6 air, K = 5 face, K = 4 edge, K = 3 corner
  mcl::Vector<mcl::Vector<mcl::Vector<sal::Int>>> G;

  Nx = Nx + 2;
  Ny = Ny + 2;
  Nz = Nz + 2;

  Initialise3DArray(G, Nx, Ny, Nz);

  // G = 6.*ones(Nx,Ny,Nz); %K = 6 air
  for (size_t i = 0; i < Nx; ++i)
  {
    for (size_t j = 0; j < Ny; ++j)
    {
      for (size_t k = 0; k < Nz; ++k)
      {
        G[i][j][k] = 6;
      }
    }
  }

  //    %K = 5
  //    G(2,:,:) = 5;
  //    G(Nx-1,:,:) = 5;
  for (size_t j = 0; j < Ny; ++j)
  {
    for (size_t k = 0; k < Nz; ++k)
    {
      G[1][j][k] = 5;
      G[Nx - 2][j][k] = 5;
    }
  }

  //    G(:,2,:) = 5;
  //    G(:,Ny-1,:) = 5;
  for (size_t i = 0; i < Nx; ++i)
  {
    for (size_t k = 0; k < Nz; ++k)
    {
      G[i][1][k] = 5;
      G[i][Ny - 2][k] = 5;
    }
  }

  //    G(:,:,2) = 5;
  //    G(:,:,Nz-1) = 5;
  for (size_t i = 0; i < Nx; ++i)
  {
    for (size_t j = 0; j < Ny; ++j)
    {
      G[i][j][1] = 5;
      G[i][j][Nz - 2] = 5;
    }
  }

  // % K = 4
  //
  //    G(2,2,:) = 4;
  //    G(2,Ny-1,:) = 4;
  //    G(Nx-1,Ny-1,:) = 4;
  //    G(Nx-1,2,:) = 4;
  for (size_t k = 0; k < Nz; ++k)
  {
    G[1][1][k] = 4;
    G[1][Ny - 2][k] = 4;
    G[Nx - 2][Ny - 2][k] = 4;
    G[Nx - 2][1][k] = 4;
  }

  //    G(:,2,2) = 4;
  //    G(:,2,Nz-1) = 4;
  //    G(:,Ny-1,Nz-1) = 4;
  //    G(:,Ny-1,2) = 4;
  for (size_t i = 0; i < Nx; ++i)
  {
    G[i][1][1] = 4;
    G[i][1][Nz - 2] = 4;
    G[i][Ny - 2][Nz - 2] = 4;
    G[i][Ny - 2][1] = 4;
  }

  //    G(2,:,2) = 4;
  //    G(2,:,Nz-1) = 4;
  //    G(Nx-1,:,Nz-1) = 4;
  //    G(Nx-1,:,2) = 4;
  for (size_t j = 0; j < Ny; ++j)
  {
    G[1][j][1] = 4;
    G[1][j][Nz - 2] = 4;
    G[Nx - 2][j][1] = 4;
    G[Nx - 2][j][Nz - 2] = 4;
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
  G[Nx - 2][1][1] = 3;
  G[1][Ny - 2][1] = 3;
  G[1][1][Nz - 2] = 3;
  G[Nx - 2][Ny - 2][1] = 3;
  G[Nx - 2][1][Nz - 2] = 3;
  G[1][Ny - 2][Nz - 2] = 3;
  G[Nx - 2][Ny - 2][Nz - 2] = 3;

  //    % %K = 0

  //    G(1,:,:) = 0;
  //    G(Nx,:,:) = 0;
  for (size_t j = 0; j < Ny; ++j)
  {
    for (size_t k = 0; k < Nz; ++k)
    {
      G[0][j][k] = 0;
      G[Nx - 1][j][k] = 0;
    }
  }

  //
  //    G(:,1,:) = 0;
  //    G(:,Ny,:) = 0;
  for (size_t i = 0; i < Nx; ++i)
  {
    for (size_t k = 0; k < Nz; ++k)
    {
      G[i][0][k] = 0;
      G[i][Ny - 1][k] = 0;
    }
  }

  //
  //    G(:,:,1) = 0;
  //    G(:,:,Nz) = 0;
  for (size_t i = 0; i < Nx; ++i)
  {
    for (size_t j = 0; j < Ny; ++j)
    {
      G[i][j][0] = 0;
      G[i][j][Nz - 1] = 0;
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
