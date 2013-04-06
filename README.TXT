Matlab-C++ Library (MCL)


MCL is a C++ library that implements many of Matlab's functions. The library 
is written so as to minimise the time needed to port Matlab code to C++. 
The  main point of departure between the two is that c++ requires the definition 
of the types you are using. In MCL we have three types: Real, Int, Complex. 
Furthermore, vectors start from 0 instead of 1.
The library has been written following Google's C++ style guide:
http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

A sample of how to use the library is:

std::vector<Real> my_vector_a;
my_vector_a = Zeros<Real>(3);                   // my_vector_a = zeros(3,1);
my_vector_a[0] = 2.5;                           // my_vector_a(1) = 2.5;
my_vector_a[1] = 0.0;                           // my_vector_a(2) = 0.0;
my_vector_a[2] = -2.4;                          // my_vector_a(3) = -2.4;

Real max_value = Max(my_vector);                // max_value = max(my_vector);
// max_value is 2.5

Real mean_value = Mean(my_vector);              // mean_value = mean(my_vector);

std::vector<Real> my_vector_b;
my_vector_b = Zeros<Real>(3);                   // my_vector_b = zeros(3,1);
my_vector_b[0] = 1.0;                           // my_vector_b(1) = 1.0;
my_vector_b[1] = 0.0;                           // my_vector_b(2) = 0.0;
my_vector_b[2] = 2.4;                           // my_vector_b(3) = 2.4;

std::vector<Real> my_vector_add;
my_vector_add = Add(my_vector_a, my_vector_b); // my_vector_add = 
                                               //     my_vector_a+my_vector_b

// Convert to complex vector
std::vector<Complex> my_vector_c = ComplexVector(my_vector_a);

// Computes the FFT on 16 points
std::vector<Complex> my_vector_fft;
my_vector_fft = Fft(my_vector_c, 16);          // my_vector_fft = fft(my_vector_c, 16);
std::vector<Real> my_vector_fft_abs;
my_vector_fft_abs = Abs(my_vector_fft);        // my_vector_fft_abs = abs(my_vector_fft);     

std::vector<Real> my_vector_conv;
my_vector_conv = Conv(my_vector_a, my_vector_b);// my_vector_conv = 
                                               //      conv(my_vector_a, my_vector_b);


// Some more advanced functions
std::vector<Real> my_vector_rceps;
my_vector_rceps = RCeps(my_vector);            // my_vector_rceps = rceps(my_vector);

std::vector<Complex> my_vector_hilbert;
my_vector_hilbert = Hilbert(my_vector);        // my_vector_hilbert = hilbert(my_vector);



Enzo De Sena
