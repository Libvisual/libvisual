
INCLUDES = -I$(top_srcdir)/GForceCommon/Headers \
	 -I$(top_srcdir)/Common/UI/Headers \
	 -I$(top_srcdir)/Common \
	 -I$(top_srcdir)/Common/GeneralTools/Headers \
	 -I$(top_srcdir)/Common/math/Headers \
	 -I$(top_srcdir)/Common/io/Headers \
	 -I$(top_srcdir)/unix/Headers \
	 -I$(top_srcdir)/unix/libmfl

ALLSOURCES = actor_gforce.cpp

LIBS += @LIBVISUAL_LIBS@
AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

noinst_LTLIBRARIES = libactor_gforce.la

libactor_gforce_la_SOURCES = $(ALLSOURCES)
