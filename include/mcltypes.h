/*
 mcltypes.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */

#ifndef MCL_MCLTYPES_H
#define MCL_MCLTYPES_H

#ifndef PI
  #define PI 3.141592653589793238462643383279502884197169399375105820974944
#endif

#include <complex>
#include <vector>

namespace mcl {
typedef double Real; /**< Real type */
typedef std::complex<Real> Complex; /**< Complex type */
typedef unsigned long UInt; /**< Unisgned int type */
typedef long Int; /**< Int type */  
  


} // namespace mcl



#endif
