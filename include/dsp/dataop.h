/*
 Spatial Audio Library (SAL)
 Copyright (c) 2012-24, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#ifndef SAL_DSP_DATAOP_H
#define SAL_DSP_DATAOP_H

#include <vector>

#include "dsptypes.h"

namespace sal {

namespace dsp {

/** Writes the vector to a file. The separator is endline. */
void Save(const std::vector<Real>& vector, const std::string& file_name,
          const dsp::Int precision = 5);

} // namespace dsp

} // namespace sal

#endif
