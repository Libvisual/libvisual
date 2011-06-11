
SUBDIRS = Headers

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/GForceCommon/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl

AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)

ALLSOURCES = Arg.cpp ArgList.cpp FileSpecList.cpp Hashtable.cpp RectUtils.cpp TempMem.cpp UtilStr.cpp XFloatList.cpp XLongList.cpp XPtrList.cpp XPtrMatrix.cpp XStrList.cpp nodeClass.cpp 

#CXXFLAGS = "$CXXFLAGS -DUNIX_X -D_REENTRANT"

noinst_LTLIBRARIES = libgt.la

libgt_la_SOURCES = $(ALLSOURCES)

