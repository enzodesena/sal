/*
 MCL
 Copyright (c) 2012-18, Enzo De Sena
 All rights reserved.

 Authors: Enzo De Sena, enzodesena@gmail.com
 */

#include <cassert>
#include <vector>

#include "comparisonop.h"
#include "elementaryop.h"
#include "mcltypes.h"

namespace mcl {

bool ElementaryOpTest() {
  ASSERT(Fix(0.2) == 0);
  ASSERT(Fix(2.2) == 2);
  ASSERT(Fix(2.6) == 2);
  ASSERT(Fix(-2.2) == -2);
  ASSERT(Fix(-2.7) == -2);

  Real rem_a = Rem((Real)2.0, 2.0);
  ASSERT(IsEqual(rem_a, 0.0));
  ASSERT(IsEqual(Rem((Real)5.0, 2.0), 1.0));
  ASSERT(IsEqual(Rem((Real)5.5, 2.0), 1.5));
  ASSERT(IsEqual(Rem((Real)2.0, 3.0), 2.0));
  ASSERT(IsEqual(Rem((Real)-2.0, 3.0), -2.0));
  ASSERT(IsEqual(Rem((Real)2.0, -3.0), 2.0));
  ASSERT(IsEqual(Rem((Real)-2.0, -3.0), -2.0));
  ASSERT(IsEqual(Rem((Real)-2.0, -3.0), -2.0));

  ASSERT(Mod((Real)-90.0, 360.0) > 0.0);

  ASSERT(IsEqual(Mod((Real)3.0, 0.0), 3.0));
  ASSERT(IsEqual(Mod((Real)2.0, 2.0), 0.0));
  ASSERT(IsEqual(Mod((Real)3.0, -5.0), -2.0));
  ASSERT(IsEqual(Rem((Real)3.0, -5.0), 3.0));
  ASSERT(isnan(Rem((Real)3.0, 0.0)));
  ASSERT(!isnan(Rem((Real)3.0, 1.0)));
  ASSERT(IsEqual(Mod((Real)3.0, -100.0), -97.0));
  ASSERT(IsEqual(Rem((Real)3.0, -100.0), 3.0));
  ASSERT(IsEqual(Rem((Real)-2.0, 4.0), -2.0));
  ASSERT(IsEqual(Mod((Real)-2.0, 4.0), 2.0));
  ASSERT(IsEqual(Rem((Real)-2.5, -3.5), -2.5));
  ASSERT(IsEqual(Mod((Real)-2.5, -3.5), -2.5));

  ASSERT(Mod((Int)3, 0) == 3);
  ASSERT(Mod((Int)2, 2) == 0);
  ASSERT(Mod((Int)-3, 3) == 0);
  ASSERT(Mod((Int)3, -5) == -2);
  ASSERT(Mod((Int)3, -100) == -97);
  ASSERT(Mod((Int)-2, 4) == 2);

  ASSERT(IsEqual(0.4, Abs(0.4)));
  ASSERT(IsEqual(0.0, Abs(0.0)));
  ASSERT(IsEqual(0.4, Abs(-0.4)));

  ASSERT(IsEqual(Max(0.2, -0.4), 0.2));
  ASSERT(IsEqual(Max(0.3, 0.3), 0.3));
  ASSERT(IsEqual(Max(0.3, 0.5), 0.5));
  ASSERT(Max(-1, 2) == 2);
  ASSERT(Max(3, 3) == 3);

  ASSERT(IsEqual(RealPart(Complex(-0.3, 0.1)), -0.3));
  ASSERT(IsEqual(RealPart(Complex(0.3, 0.1)), 0.3));
  ASSERT(IsEqual(RealPart(Complex(-0.3, 0.5)), -0.3));
  ASSERT(IsEqual(RealPart(Complex(15.6, 0.1)), 15.6));

  ASSERT(IsEqual(ImagPart(Complex(-0.3, 0.1)), 0.1));
  ASSERT(IsEqual(ImagPart(Complex(0.3, 0.1)), 0.1));
  ASSERT(IsEqual(ImagPart(Complex(-0.3, 0.5)), 0.5));
  ASSERT(IsEqual(ImagPart(Complex(15.6, 0.1)), 0.1));

  ASSERT(Mod((Int)-2, (Int)4) == 2);

  ASSERT(IsEqual(Abs(Complex(0.5, 2.1)), 2.158703314492290));
  ASSERT(IsEqual(Abs(Complex(-0.5, 1.5)), 1.581138830084190));

  ASSERT(Next2(3) == 4);
  ASSERT(Next2(3.0) == 4);
  ASSERT(Next2(5) == 8);
  ASSERT(Next2(8) == 8);
  ASSERT(Next2(8.0) == 8);
  ASSERT(Next2(6001) == 8192);

  // Testing entropy
  std::vector<Real> pdf_1(4);
  pdf_1[0] = 0.2;
  pdf_1[1] = 0.35;
  pdf_1[2] = 0.15;
  pdf_1[3] = 0.3;
  ASSERT(IsEqual(Entropy(pdf_1, exp(1)), 1.335085165092020));
  ASSERT(IsEqual(Entropy(pdf_1, 2.0), 1.926120746842681));

  std::vector<Real> pdf_2(2);
  pdf_2[0] = 0.5;
  pdf_2[1] = 0.5;
  ASSERT(IsEqual(Entropy(pdf_2, exp(1)), 0.693147180559945));
  ASSERT(IsEqual(Entropy(pdf_2, 2.0), 1.0));

  ASSERT(Factorial(0) == 1);
  ASSERT(Factorial(1) == 1);
  ASSERT(Factorial(2) == 2);
  ASSERT(Factorial(3) == 6);
  ASSERT(Factorial(6) == 720);
  ASSERT(Factorial(9) == 362880);

  ASSERT(IsEqual(LinearInterpolation(1.0, 0.0, 2.0, 1.0, 1.0), 0.0));
  ASSERT(IsEqual(LinearInterpolation(1.0, 0.0, 2.0, 1.0, 2.0), 1.0));
  ASSERT(IsEqual(LinearInterpolation(1.0, 0.0, 2.0, 1.0, 1.5), 0.5));

  ASSERT(RoundToInt(3.2) == 3);
  ASSERT(RoundToInt(3.6) == 4);
  ASSERT(RoundToInt(0.2) == 0);
  ASSERT(RoundToInt(-0.2) == 0);
  ASSERT(RoundToInt(-3.6) == -4);

#if MCL_LOAD_BOOST
  ASSERT(IsEqual(AssociatedLegendreP(3, 0, 0.8), 0.08));
  ASSERT(IsEqual(AssociatedLegendreP(3, 1, 0.8), -1.980000000000001));
  ASSERT(IsEqual(AssociatedLegendreP(3, 2, 0.8), 4.320000000000000));
  ASSERT(IsEqual(AssociatedLegendreP(3, 3, 0.8), -3.239999999999998));
  ASSERT(IsEqual(AssociatedLegendreP(1, 0, 0.7), 0.7));
  ASSERT(IsEqual(AssociatedLegendreP(1, 1, 0.7), -0.714142842854285));
  ASSERT(IsEqual(AssociatedLegendreP(1, 0, -0.7), -0.7));
  ASSERT(IsEqual(AssociatedLegendreP(1, 1, -0.7), -0.714142842854285));

  ASSERT(IsEqual(SphericalHarmonic(4, 4, 1.0, 2.0),
                 Complex(-0.0322823, 0.219511)));
  ASSERT(
      IsEqual(SphericalHarmonic(4, 2, 1.5, 2.2), Complex(0.0987125, 0.305646)));
  ASSERT(IsEqual(SphericalHarmonic(4, -2, 1.5, 2.2),
                 Complex(0.0987125, -0.305646)));
#endif

  return true;
}

}  // namespace mcl
