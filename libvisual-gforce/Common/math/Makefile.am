
SUBDIRS = Headers

INCLUDES = -I$(top_srcdir)/Common\
	-I$(top_srcdir)/Common/UI/Headers\
	-I$(top_srcdir)/Common/GeneralTools/Headers\
	-I$(top_srcdir)/Common/math/Headers\
	-I$(top_srcdir)/Common/io/Headers\
	-I$(top_srcdir)/unix/Headers\
	-I$(top_srcdir)/unix/libmfl

ALLSOURCES = ExprArray.cpp ExprVirtualMachine.cpp Expression.cpp ExpressionDict.cpp FourierAnalyzer.cpp R3Matrix.cpp V3.cpp 

LIBS += @LIBVISUAL_LIBS@
AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

noinst_LTLIBRARIES = libmath.la

libmath_la_SOURCES = $(ALLSOURCES)

