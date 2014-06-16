AUTOMAKE_OPTIONS = gnu


# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/include/lib
AM_CXXFLAGS = -std=c++11

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = mcltest

mcltest_SOURCES = src/basicop.cpp src/comparisonop.cpp src/dataop.cpp src/elementaryop.cpp src/firfilter.cpp src/iirfilter.cpp src/matrixop.cpp src/pointwiseop.cpp src/randomop.cpp src/statisticsop.cpp src/transformop.cpp src/vectorop.cpp src/lib/kiss_fft.c src/lib/butter.cpp src/targets/mcl_test.cpp test/basicop_test.cpp test/comparisonop_test.cpp test/digitalfilter_test.cpp test/elementaryop_test.cpp test/matrixop_test.cpp test/pointwiseop_test.cpp test/random_test.cpp test/statisticsop_test.cpp test/transformop_test.cpp test/vectorop_test.cpp

lib_LIBRARIES = libmcl.a
libmcl_a_SOURCES = src/basicop.cpp src/comparisonop.cpp src/dataop.cpp src/elementaryop.cpp src/firfilter.cpp src/iirfilter.cpp src/matrixop.cpp src/pointwiseop.cpp src/randomop.cpp src/statisticsop.cpp src/transformop.cpp src/vectorop.cpp src/lib/kiss_fft.c src/lib/butter.cpp

pkginclude_HEADERS = include/basicop.h include/comparisonop.h include/dataop.h include/digitalfilter.h include/elementaryop.h include/firfilter.h include/iirfilter.h include/matrixop.h include/mcl.h include/mcltypes.h include/pointwiseop.h include/randomop.h include/statisticsop.h include/transformop.h include/vectorop.h include/lib/butter.h include/lib/kiss_fft.h include/lib/kissfft.hh include/lib/_kiss_fft_guts.h
