/*
 mcltypes.h
 Matlab Cpp Library (MCL)
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 95 $
 Last changed date:  $Date: 2012-06-07 20:07:36 +0100 (Thu, 07 Jun 2012) $
 */

#ifndef MCL_MCLTYPES_H
#define MCL_MCLTYPES_H

#ifndef PI
  #define PI 3.141592653589793238462643383279502884197169399375105820974944
#endif

#include <complex>
#include <vector>

namespace mcl {
typedef double Real;
typedef std::complex<Real> Complex;
typedef unsigned long UInt;
typedef long Int;
  
  


} // namespace mcl



#endif
