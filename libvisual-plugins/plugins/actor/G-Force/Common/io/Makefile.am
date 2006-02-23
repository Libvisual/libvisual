
SUBDIRS = Headers

GFORCE_TOP_SRCDIR = $(top_srcdir)/plugins/actor/G-Force

INCLUDES = \
	-I$(GFORCE_TOP_SRCDIR)/Common \
	-I$(GFORCE_TOP_SRCDIR)/Common/GeneralTools/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/io/Headers \
	-I$(GFORCE_TOP_SRCDIR)/Common/UI/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/Headers \
	-I$(GFORCE_TOP_SRCDIR)/unix/libmfl

ALLSOURCES = CEgErr.cpp CEgFileSpec.cpp CEgIFile.cpp CEgIOFile.cpp CEgIStream.cpp CEgOStream.cpp ConfigFile.cpp Prefs.cpp 

noinst_LTLIBRARIES = libio.la

libio_la_SOURCES = $(ALLSOURCES)

