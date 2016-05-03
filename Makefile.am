AUTOMAKE_OPTIONS = gnu


# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/include -I$(top_srcdir)/lib/butter/include/ -I$(top_srcdir)/lib/kissfft/include/
AM_CXXFLAGS = -std=c++11

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = mcltest

mcltest_SOURCES = src/basicop.cpp src/comparisonop.cpp src/dataop.cpp src/elementaryop.cpp src/firfilter.cpp src/iirfilter.cpp src/matrixop.cpp src/pointwiseop.cpp src/randomop.cpp src/statisticsop.cpp src/transformop.cpp src/vectorop.cpp lib/kissfft/src/kiss_fft.c lib/butter/src/butter.cpp src/bin/mcl_test.cpp src/test/basicop_test.cpp src/test/comparisonop_test.cpp src/test/digitalfilter_test.cpp src/test/elementaryop_test.cpp src/test/matrixop_test.cpp src/test/pointwiseop_test.cpp src/test/random_test.cpp src/test/statisticsop_test.cpp src/test/transformop_test.cpp src/test/vectorop_test.cpp

lib_LIBRARIES = libmcl.a
libmcl_a_SOURCES = src/basicop.cpp src/comparisonop.cpp src/dataop.cpp src/elementaryop.cpp src/firfilter.cpp src/iirfilter.cpp src/matrixop.cpp src/pointwiseop.cpp src/randomop.cpp src/statisticsop.cpp src/transformop.cpp src/vectorop.cpp lib/kissfft/src/kiss_fft.c lib/butter/src/butter.cpp

pkginclude_HEADERS = include/*.h lib/butter/include/butter.h lib/kissfft/include/*.h lib/kissfft/include/*.hh
