AUTOMAKE_OPTIONS = gnu


# what flags you want to pass to the C compiler & linker
AM_CFLAGS = --pedantic -Wall -std=c99 -O2
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/include/lib -I$(includedir)/mcl
#AM_LDFLAGS = -L$(libdir) -L$(top_srcdir)/lib
AM_CXXFLAGS = -std=c++11

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = saltest

saltest_SOURCES = src/ambisonics.cpp src/delayfilter.cpp src/freefieldsimulation.cpp src/kemarmic.cpp src/microphone.cpp src/microphonearray.cpp src/point.cpp src/propagationline.cpp src/psrmic.cpp src/slowdelayfilter.cpp src/source.cpp src/sphericalmic.cpp src/wavhandler.cpp src/targets/sal_tests.cpp test/ambisonics_test.cpp test/delayfilter_test.cpp test/freefieldsimulation_test.cpp test/kemarmic_test.cpp test/microphone_test.cpp test/microphonearray_test.cpp test/point_test.cpp test/propagationline_test.cpp test/psrmic_test.cpp test/sphericalheadmic_test.cpp test/stream_test.cpp
saltest_LDADD = $(libdir)/libmcl.a $(libdir)/libsndfile.a

lib_LIBRARIES = libsal.a
libsal_a_SOURCES = src/ambisonics.cpp src/delayfilter.cpp src/freefieldsimulation.cpp src/kemarmic.cpp src/microphone.cpp src/microphonearray.cpp src/point.cpp src/propagationline.cpp src/psrmic.cpp src/slowdelayfilter.cpp src/source.cpp src/sphericalmic.cpp src/wavhandler.cpp
libsal_a_LIBADD = $(libdir)/libmcl.a $(libdir)/libsndfile.a

pkginclude_HEADERS = include/ambisonics.h include/array.h include/decoder.h include/delayfilter.h include/freefieldsimulation.h include/kemarmic.h include/microphone.h include/microphonearray.h include/monomics.h include/point.h include/propagationline.h include/psrmic.h include/salconstants.h include/saltypes.h include/slowdelayfilter.h include/source.h include/sphericalheadmic.h include/stream.h include/wavhandler.h include/lib/sndfile.h include/lib/sndfile.hh
