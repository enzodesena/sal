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

#ifndef throw_line
  #define throw_line(arg) throw std::runtime_error(std::string("Error at line n. ") +std::string(std::to_string(__LINE__)) + std::string(" of file ") + std::string(__FILE__));
#endif

#include <complex>
#include <vector>

namespace mcl {
typedef float Real; /**< Real type */
typedef std::complex<Real> Complex; /**< Complex type */
typedef unsigned long UInt; /**< Unisgned int type */
typedef long Int; /**< Int type */  
  


} // namespace mcl



#endif
