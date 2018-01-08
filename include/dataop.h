/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 */


#ifndef MCL_DATAOP_H
#define MCL_DATAOP_H

#include <vector>
#include "mcltypes.h"

#ifdef MCL_EXPORTS
  #define MCL_API __declspec(dllexport)
#else
  #define MCL_API
#endif

namespace mcl {
  
/** Writes the vector to a file. The separator is endline. */
MCL_API void Save(const std::vector<Real>& vector,
                  const std::string& file_name,
                  const mcl::UInt precision = 5);

} // namespace mcl



#endif
