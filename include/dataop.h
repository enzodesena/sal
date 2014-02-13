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
  
void Save(const std::vector<Real>& vector,
          const std::string& file_name,
          const mcl::UInt precision = 5);

} // namespace mcl



#endif
