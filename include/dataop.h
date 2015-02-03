/*
 comparisonop.h
 MCL
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#ifndef MCL_DATAOP_H
#define MCL_DATAOP_H

#include <vector>
#include "mcltypes.h"

namespace mcl {
  
/** Writes the vector to a file. The separator is endline. */
void Save(const std::vector<Real>& vector,
          const std::string& file_name,
          const mcl::UInt precision = 5);

} // namespace mcl



#endif
