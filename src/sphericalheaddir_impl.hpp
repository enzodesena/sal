/*
 Spatial Audio Library (SAL)
 Copyright (c) 2011-2018, Enzo De Sena
 All rights reserved.
 
 Authors: Enzo De Sena, enzodesena@gmail.com
 
 */


namespace sal
{

template<typename T>
SphericalHeadDirectivity<T>::SphericalHeadDirectivity(
  const Angle ears_angle,
  const Length sphere_radius,
  const size_t ir_length,
  const Time sampling_frequency,
  const size_t update_length)
  : FirBinauralDirectivity<T>(update_length)
  , ears_angle_(ears_angle)
  , sphere_radius_(sphere_radius)
  , sampling_frequency_(sampling_frequency)
  , impulse_response_length_(ir_length)
  , sound_speed_(343.0)
  , alg_threshold_(0.0001)
{
}


template<typename T>
Angle SphericalHeadDirectivity<T>::GetTheta(
  const Point& point,
  const Angle& ears_angle,
  const Ear& ear)
{
  Angle theta_ear = PI / 2.0;
  Angle phi_ear = (ear == Ear::kLeft) ? ears_angle : -ears_angle;

  // Here we compute the angle between the direction of the ear and the
  // direction of the incoming wave.
  Angle relative_angle = mcl::AngleBetweenDirections
  (
    theta_ear,
    phi_ear,
    point.theta(),
    point.phi());
  return relative_angle;
}


// Ported from pseudo-code in R.O. Duda and W. L. Martens, "Range dependence
// of the response of a spherical head model".
template<typename T>
mcl::Complex<T> SphericalHeadDirectivity<T>::Sphere(
  Length a,
  Length r,
  Angle theta,
  Time f,
  Time c,
  mcl::Real threshold)
{
  using mcl::Real;
  using mcl::Complex;

  Real x = cos(theta);
  Real mu = (2 * PI * f * a) / c;
  Real rho = r / a;
  // i = sqrt(-1);
  // zr = 1 / (i * mu * rho);
  Complex<T> zr = Complex<T>(0.0, -1 / (mu * rho));
  // za = 1 / (i * mu);
  Complex<T> za = Complex<T>(0.0, -1 / (mu));
  Complex<T> Qr2 = zr;
  // Qr1 = zr * (1.0 - zr);
  Complex<T> Qr1 = zr * (Complex<T>(1.0, 0.0) - zr);
  Complex<T> Qa2 = za;
  // Qa1 = za * (1 - za);
  Complex<T> Qa1 = za * (Complex<T>(1.0, 0.0) - za);
  Complex<T> P2 = Complex<T>(1.0, 0.0);
  Complex<T> P1 = Complex<T>(x, 0.0);
  Complex<T> sum = Complex<T>(0.0, 0.0);
  // term = zr / (za * (za - 1));
  Complex<T> term = zr / (za * (za - Complex<T>(1.0, 0.0)));
  sum = sum + term;
  // term = (3 * x * zr * (zr - 1) ) / (za * (2 * za^2 - 2 * za + 1) );
  term = (Complex<T>(3.0, 0.0) * x * zr * (zr - Complex<T>(1.0, 0.0))) /
  (za * (Complex<T>(2.0, 0.0) * pow(za, (Real)2.0) -
    Complex<T>(2.0, 0.0) * za + Complex<T>(1.0, 0.0)));
  sum = sum + term;
  Real oldratio = 1;
  Real newratio = std::abs(term) / std::abs(sum);
  Int m = 2;
  while (oldratio > threshold || newratio > threshold)
  {
    // Qr = - (2 * m - 1) * zr * Qr1 + Qr2;
    Complex<T> Qr = - (Complex<T>(2.0, 0.0) * ((Complex<T>)m) -
      Complex<T>(1.0, 0.0)) * zr * Qr1 + Qr2;
    // Qa = - (2 * m - 1) * za * Qa1 + Qa2;
    Complex<T> Qa = - (Complex<T>(2.0, 0.0) * ((Complex<T>)m) -
      Complex<T>(1.0, 0.0)) * za * Qa1 + Qa2;
    // P = ( (2 * m - 1) * x * P1 - (m - 1) * P2) / m;
    Complex<T> P = ((Complex<T>(2.0, 0.0) * ((Complex<T>)m) - Complex<T>(1.0, 0.0)) * x * P1
      - (((Complex<T>)m) - Complex<T>(1.0, 0.0)) * P2) / ((Complex<T>)m);
    // term = ( (2 * m + 1) * P * Qr) / ( (m + 1) * za * Qa - Qa1);
    term = ((Complex<T>(2.0, 0.0) * ((Complex<T>)m) + Complex<T>(1.0, 0.0)) * P * Qr) /
      ((((Complex<T>)m) + Complex<T>(1.0, 0.0)) * za * Qa - Qa1);
    sum = sum + term;
    m = m + 1;
    Qr2 = Qr1;
    Qr1 = Qr;
    Qa2 = Qa1;
    Qa1 = Qa;
    P2 = P1;
    P1 = P;
    oldratio = newratio;
    newratio = std::abs(term) / std::abs(sum);
  }

  return (rho * exp(- Complex<T>(0.0, 1.0) * mu) * sum) / (Complex<T>(0.0, 1.0) * mu);
}


template<typename T>
Signal<T> SphericalHeadDirectivity<T>::GetBrir(
  const Ear ear,
  const Point& point) noexcept
{
  return GenerateImpulseResponse
  (
    sphere_radius_,
    point.norm(),
    // point distance
    GetTheta(point, ears_angle_, ear),
    sound_speed_,
    alg_threshold_,
    impulse_response_length_,
    sampling_frequency_);
}


template<typename T>
Signal<T> SphericalHeadDirectivity<T>::GenerateImpulseResponse(
  Length sphere_radius,
  Length source_distance,
  Angle theta,
  Time sound_speed,
  mcl::Real threshold,
  Int num_samples,
  Time sampling_frequency,
  bool minimum_phase)
{
  // TODO: implement for the odd case.
  ASSERT(num_samples % 2 == 0);

  using mcl::LinSpace;
  using mcl::Zeros;
  using mcl::Length;
  using mcl::Real;
  using mcl::Complex;
  using mcl::RealPart;
  using mcl::Ifft;
  using mcl::Conj;
  using mcl::CircShift;
  using mcl::IsReal;

  Int N = num_samples;

  theta = mcl::Mod(theta, 2.0 * PI);

  // f = linspace(Fs/N, Fs/2, N/2-1);
  mcl::Vector<Time> frequencies = LinSpace
  (
    sampling_frequency / ((Time)N),
    sampling_frequency / 2.0,
    N / 2 - 1);

  mcl::Vector<Complex<T>> H = Zeros<Complex<T>>(N);

  for (Int i = 0; i < N / 2 - 1; ++i)
  {
    H[i + 1] = SphericalHeadDirectivity<T>::Sphere
    (
      sphere_radius,
      source_distance,
      theta,
      frequencies[i],
      sound_speed,
      threshold);
  }

  // Sphere can't output values for 0 frequency. The value is not important
  // as it is outside the hearing range.
  H[0] = Complex<T>(RealPart(H[1]), T(0.0));
  // We need to impose a real value at half of the transfrom, otherwise H would
  // not be hermitian. The exact value is not important because this will be
  // outside the hearing range.
  H[N / 2] = Complex<T>(RealPart(H[1]), T(0.0));

  // H(N/2+1:end) = flipud(conj(H(1:N/2-1)));
  for (Int i = (N / 2 + 1); i < N; ++i)
  {
    H[i] = Conj(H[N - i]);
  }

  // I take the ifft of the vector. I also include a Conj because Sphere
  // seems to output time-reversed samples.
  // The output should already be real, but I still extract the real value to
  // account for round-off errors.
  mcl::Vector<Complex<T>> inverse = Ifft(Conj(H), N);
  ASSERT(mcl::IsApproximatelyReal(inverse));
  mcl::Vector<Real> h = RealPart(inverse);

  h = CircShift(h, -((Int)N) / 2);

  if (minimum_phase)
  {
    h = mcl::MinPhase(h);

    // Calculate delay due to propagation around the head
    Angle theta_0 = acos(sphere_radius / source_distance);
    sal::Length distance;
    if (theta < theta_0 || theta > (2.0 * PI - theta_0))
    {
      distance = sqrt
      (
        pow(source_distance, 2.0) + pow(sphere_radius, 2.0)
        - 2.0 * source_distance * sphere_radius * cos(theta));
    }
    else
    {
      // The minimum in this formula is due to the shortest path around the head.
      distance = sqrt(pow(source_distance, 2.0) - pow(sphere_radius, 2.0))
        + sphere_radius * mcl::Min<Real>(
          theta - theta_0, (2.0 * PI - theta_0) - theta);
    }
    // Subract the distance between sphere and source
    distance = distance - (source_distance - sphere_radius);
    ASSERT(distance > 0.0);
    Int num_delay_tap = mcl::RoundToInt(
      distance / sound_speed * sampling_frequency);
    h = mcl::Concatenate
    (
      Zeros<Sample>(num_delay_tap),
      mcl::Subset(h, 0, num_samples - num_delay_tap - 1));
    ASSERT((Int)h.size() == num_samples);
  }

  return h;
}

} // namespace sal
