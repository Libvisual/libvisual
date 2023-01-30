## Process this file with automake to produce Makefile.in

DEFS = @DEFS@

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/examples -I$(top_builddir)

AM_CFLAGS = $(SDL_CFLAGS) $(VISUAL_CFLAGS)

noinst_PROGRAMS = @EXAMPLES@

EXTRA_PROGRAMS = simplesdl morphsdl

simplesdl_LDADD = $(SDL_LIBS) ../libvisual/libvisual-@LIBVISUAL_VERSION_SUFFIX@.la
morphsdl_LDADD = $(SDL_LIBS) ../libvisual/libvisual-@LIBVISUAL_VERSION_SUFFIX@.la

simplesdl_SOURCES = simplesdl.c

morphsdl_SOURCES = morphsdl.c

