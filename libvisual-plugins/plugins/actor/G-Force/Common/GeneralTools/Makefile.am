
SUBDIRS = Headers

INCLUDES = -I../../unix/Headers\
	-I../../Common\
	-I../../Common/io/Headers\
	-I../../Common/GeneralTools/Headers\
	-I../../GForceCommon/Headers\
	-I../../unix/libmfl

AM_CXXFLAGS = @LIBVISUAL_CFLAGS@

ALLSOURCES = Arg.cpp ArgList.cpp FileSpecList.cpp Hashtable.cpp RectUtils.cpp TempMem.cpp UtilStr.cpp XFloatList.cpp XLongList.cpp XPtrList.cpp XPtrMatrix.cpp XStrList.cpp nodeClass.cpp 

#CXXFLAGS = "$CXXFLAGS -DUNIX_X -D_REENTRANT"

noinst_LTLIBRARIES = libgt.la

libgt_la_SOURCES = $(ALLSOURCES)

