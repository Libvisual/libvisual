
INCLUDES = -I../../GForceCommon/Headers \
	 -I../../Common/UI/Headers \
	 -I../../Common \
	 -I../../Common/GeneralTools/Headers \
	 -I../../Common/math/Headers \
	 -I../../Common/io/Headers \
	 -I../../unix/Headers \
	 -I../../unix/libmfl

ALLSOURCES = actor_gforce.cpp

LIBS += @LIBVISUAL_LIBS@
AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

noinst_LTLIBRARIES = libactor_gforce.la

libactor_gforce_la_SOURCES = $(ALLSOURCES)
