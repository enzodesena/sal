/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */

#ifndef MCL_MCLTYPES_H
#define MCL_MCLTYPES_H

#ifndef PI
  #define PI 3.141592653589793238462643383279502884197169399375105820974944
#endif

#define throw_line(arg) throw
// #define throw_line(arg) throw std::runtime_error(std::string("Error at line n. ") +std::string(std::to_string(__LINE__)) + std::string(" of file ") + std::string(__FILE__));

#include <complex>
#include <vector>

#if _WIN32 || _WIN64
  #if _WIN64
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

#if __GNUC__
  #if __x86_64__ || __ppc64__
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif


namespace mcl {
typedef double Real; /**< Real type */
typedef std::complex<Real> Complex; /**< Complex type */
  
#ifdef ENV64BIT
  typedef unsigned long long UInt; /**< Unsigned int type */
  typedef long long Int; /**< Int type */
#else // If it is 32 bits or unknown then...
  typedef unsigned long UInt; /**< Unisgned int type */
  typedef long Int; /**< Int type */
#endif


} // namespace mcl



#endif
