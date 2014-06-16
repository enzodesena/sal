/*
 elementaryop_test.cpp
 MCL
 Copyright (c) 2012, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@me.com
 
 */


#include "elementaryop.h"
#include "comparisonop.h"
#include "mcltypes.h"
#include <vector>
#include <cassert>



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
  
  assert(IsEqual(ImagPart(Complex(-0.3,0.1)), 0.1));
  assert(IsEqual(ImagPart(Complex(0.3,0.1)), 0.1));
  assert(IsEqual(ImagPart(Complex(-0.3,0.5)), 0.5));
  assert(IsEqual(ImagPart(Complex(15.6,0.1)), 0.1));
  
  assert(Mod((Int) -2, (Int) 4) == 2);
  
  assert(IsEqual(Abs(Complex(0.5, 2.1)), 2.158703314492290));
  assert(IsEqual(Abs(Complex(-0.5, 1.5)), 1.581138830084190));
  
  
  assert(Next2(3) == 4);
  assert(Next2(3.0) == 4);
  assert(Next2(5) == 8);
  assert(Next2(8) == 8);
  assert(Next2(8.0) == 8);
  assert(Next2(6001) == 8192);
  
  
  // Testing entropy
  std::vector<Real> pdf_1(4);
  pdf_1[0] = 0.2;
  pdf_1[1] = 0.35;
  pdf_1[2] = 0.15;
  pdf_1[3] = 0.3;
  assert(IsEqual(Entropy(pdf_1, exp(1)), 1.335085165092020));
  assert(IsEqual(Entropy(pdf_1, 2.0), 1.926120746842681));
  
  std::vector<Real> pdf_2(2);
  pdf_2[0] = 0.5;
  pdf_2[1] = 0.5;
  assert(IsEqual(Entropy(pdf_2, exp(1)), 0.693147180559945));
  assert(IsEqual(Entropy(pdf_2, 2.0), 1.0));
  
  assert(Factorial(0) == 1);
  assert(Factorial(1) == 1);
  assert(Factorial(2) == 2);
  assert(Factorial(3) == 6);
  assert(Factorial(6) == 720);
  assert(Factorial(9) == 362880);
  
#if MCL_LOAD_BOOST  
  assert(IsEqual(AssociatedLegendreP(3, 0, 0.8), 0.08));
  assert(IsEqual(AssociatedLegendreP(3, 1, 0.8), -1.980000000000001));
  assert(IsEqual(AssociatedLegendreP(3, 2, 0.8), 4.320000000000000));
  assert(IsEqual(AssociatedLegendreP(3, 3, 0.8), -3.239999999999998));
  assert(IsEqual(AssociatedLegendreP(1, 0, 0.7), 0.7));
  assert(IsEqual(AssociatedLegendreP(1, 1, 0.7), -0.714142842854285));
  assert(IsEqual(AssociatedLegendreP(1, 0, -0.7), -0.7));
  assert(IsEqual(AssociatedLegendreP(1, 1, -0.7), -0.714142842854285));
  
  assert(IsEqual(SphericalHarmonic(4, 4, 1.0, 2.0), Complex(-0.0322823, 0.219511)));
  assert(IsEqual(SphericalHarmonic(4, 2, 1.5, 2.2), Complex(0.0987125, 0.305646)));
  assert(IsEqual(SphericalHarmonic(4, -2, 1.5, 2.2), Complex(0.0987125, -0.305646)));
#endif  
  
  return true;
}
  
} // namespace mcl