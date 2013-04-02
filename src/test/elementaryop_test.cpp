/*
 elementaryop_test.cpp
 Matlab Cpp Library (MCL)
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 Last committed:     $Revision: 93 $
 Last changed date:  $Date: 2012-06-07 09:58:54 +0100 (Thu, 07 Jun 2012) $
 */


#include "elementaryop.h"
#include "equalityop.h"
#include "mcltypes.h"
#include <vector>
#include <cassert>


#if defined(__APPLE__)
#if (__GNUC__ >= 4)
#include <cmath>
#define isnan(x) std::isnan(x)
#else
#include <math.h>
#define isnan(x) __isnand((double)x)
#endif
#endif

namespace mcl {
  
bool ElementaryOpTest() {
  
  
  
  assert(Fix(0.2) == 0);
  assert(Fix(2.2) == 2);
  assert(Fix(2.6) == 2);
  assert(Fix(-2.2) == -2);
  assert(Fix(-2.7) == -2);

  
  
  
  assert(IsEqual(Rem((Real)2.0, 2.0), 0.0));
  assert(IsEqual(Rem((Real)5.0, 2.0), 1.0));
  assert(IsEqual(Rem((Real)5.5, 2.0), 1.5));
  assert(IsEqual(Rem((Real)2.0, 3.0), 2.0));
  assert(IsEqual(Rem((Real)-2.0, 3.0), -2.0));
  assert(IsEqual(Rem((Real)2.0, -3.0), 2.0));
  assert(IsEqual(Rem((Real)-2.0, -3.0), -2.0));
  assert(IsEqual(Rem((Real)-2.0, -3.0), -2.0));

  
  
  
  assert(IsEqual(Mod((Real) 3.0, 0.0), 3.0));
  assert(IsEqual(Mod((Real) 2.0, 2.0), 0.0));
  assert(IsEqual(Mod((Real) 3.0, -5.0), -2.0));
  assert(IsEqual(Rem((Real) 3.0, -5.0), 3.0));
  assert(isnan(Rem((Real) 3.0, 0.0)));
  assert(!isnan(Rem((Real) 3.0, 1.0)));
  assert(IsEqual(Mod((Real) 3.0, -100.0), -97.0));
  assert(IsEqual(Rem((Real) 3.0, -100.0), 3.0));
  assert(IsEqual(Rem((Real) -2.0, 4.0), -2.0));
  assert(IsEqual(Mod((Real) -2.0, 4.0), 2.0));
  assert(IsEqual(Rem((Real) -2.5, -3.5), -2.5));
  assert(IsEqual(Mod((Real) -2.5, -3.5), -2.5));
  
  assert(Mod((Int) 3, 0) == 3);
  assert(Mod((Int) 2, 2) == 0);
  assert(Mod((Int) -3, 3) == 0);
  assert(Mod((Int) 3, -5) == -2);
  assert(Mod((Int) 3, -100) == -97);
  assert(Mod((Int) -2, 4) == 2);

  
  
  assert(IsEqual(0.4, Abs(0.4)));
  assert(IsEqual(0.0, Abs(0.0)));
  assert(IsEqual(0.4, Abs(-0.4)));

  
  assert(IsEqual(Max(0.2, -0.4), 0.2));
  assert(IsEqual(Max(0.3, 0.3), 0.3));
  assert(IsEqual(Max(0.3, 0.5), 0.5));
  assert(Max(-1, 2) == 2);
  assert(Max(3, 3) == 3);

  
  assert(IsEqual(RealPart(Complex(-0.3,0.1)), -0.3));
  assert(IsEqual(RealPart(Complex(0.3,0.1)), 0.3));
  assert(IsEqual(RealPart(Complex(-0.3,0.5)), -0.3));
  assert(IsEqual(RealPart(Complex(15.6,0.1)), 15.6));
  
  assert(Mod((Int) -2, (Int) 4) == 2);
  
  assert(IsEqual(Abs(Complex(0.5, 2.1)), 2.158703314492290));
  assert(IsEqual(Abs(Complex(-0.5, 1.5)), 1.581138830084190));
  
  return true;
}
  
} // namespace mcl