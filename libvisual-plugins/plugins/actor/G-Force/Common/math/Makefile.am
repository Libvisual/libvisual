
SUBDIRS = Headers

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/Common/UI/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/math/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl \
	-I$(top_srcdir)

ALLSOURCES = ExprArray.cpp ExprVirtualMachine.cpp Expression.cpp ExpressionDict.cpp FourierAnalyzer.cpp R3Matrix.cpp V3.cpp 

LIBS += $(LIBVISUAL_LIBS)
AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)

noinst_LTLIBRARIES = libmath.la

libmath_la_SOURCES = $(ALLSOURCES)

