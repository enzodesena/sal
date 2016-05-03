AUTOMAKE_OPTIONS = gnu


# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2
AM_CPPFLAGS = -I$(includedir) -I$(top_srcdir)/include -I$(top_srcdir)/lib/libsndfile/include -I$(includedir)/mcl
AM_LDFLAGS = -L$(libdir) -L$(top_srcdir)/lib
AM_CXXFLAGS = -std=c++11

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = saltest

saltest_SOURCES = src/ambisonics.cpp src/binauralmic.cpp src/cipicmic.cpp src/delayfilter.cpp src/freefieldsimulation.cpp src/kemarmic.cpp src/microphone.cpp src/microphonearray.cpp src/point.cpp src/propagationline.cpp src/psrmic.cpp src/source.cpp src/sphericalmic.cpp src/wavhandler.cpp src/bin/sal_tests.cpp src/test/ambisonics_test.cpp src/test/cipicmic_test.cpp src/test/delayfilter_test.cpp src/test/freefieldsimulation_test.cpp src/test/kemarmic_test.cpp src/test/microphone_test.cpp src/test/microphonearray_test.cpp src/test/point_test.cpp src/test/propagationline_test.cpp src/test/psrmic_test.cpp src/test/sphericalheadmic_test.cpp src/test/stream_test.cpp
saltest_LDADD = $(libdir)/libmcl.a $(libdir)/libsndfile.a

lib_LIBRARIES = libsal.a
libsal_a_SOURCES = src/ambisonics.cpp src/binauralmic.cpp src/cipicmic.cpp src/delayfilter.cpp src/freefieldsimulation.cpp src/kemarmic.cpp src/microphone.cpp src/microphonearray.cpp src/point.cpp src/propagationline.cpp src/psrmic.cpp src/source.cpp src/sphericalmic.cpp src/wavhandler.cpp
libsal_a_LIBADD = $(libdir)/libmcl.a $(libdir)/libsndfile.a

pkginclude_HEADERS = include/*.h lib/libsndfile/include/sndfile.h lib/libsndfile/include/sndfile.hh