
SUBDIRS = Headers

INCLUDES = -I../../Common\
	-I../../Common/UI/Headers\
	-I../../Common/GeneralTools/Headers\
	-I../../Common/math/Headers\
	-I../../Common/io/Headers\
	-I../../unix/Headers\
	-I../../unix/libmfl

ALLSOURCES = ExprArray.cpp ExprVirtualMachine.cpp Expression.cpp ExpressionDict.cpp FourierAnalyzer.cpp R3Matrix.cpp V3.cpp 

LIBS += @LIBVISUAL_LIBS@
AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

noinst_LTLIBRARIES = libmath.la

libmath_la_SOURCES = $(ALLSOURCES)

