## Process this file with automake to produce Makefile.in

DEFS = @DEFS@

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/examples -I$(top_builddir)

AM_CFLAGS = @SDL_CFLAGS@ @VISUAL_CFLAGS@

bin_PROGRAMS = $(EXAMPLES)

EXTRA_PROGRAMS = simplesdl morphsdl

simplesdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@
morphsdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@

simplesdl_SOURCES = simplesdl.c

morphsdl_SOURCES = morphsdl.c

