
SUBDIRS = Headers

INCLUDES = -I$(top_srcdir)/Common\
	-I$(top_srcdir)/Common/GeneralTools/Headers\
	-I$(top_srcdir)/Common/io/Headers\
	-I$(top_srcdir)/Common/UI/Headers\
	-I$(top_srcdir)/unix/Headers\
	-I$(top_srcdir)/unix/libmfl

ALLSOURCES = CEgErr.cpp CEgFileSpec.cpp CEgIFile.cpp CEgIOFile.cpp CEgIStream.cpp CEgOStream.cpp ConfigFile.cpp Prefs.cpp 

noinst_LTLIBRARIES = libio.la

libio_la_SOURCES = $(ALLSOURCES)

