## Process this file with automake to produce Makefile.in

DEFS = @DEFS@

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/examples -I$(top_builddir)

# for now CFLAGS is common for all targets
AM_CFLAGS = @CFLAGS@ @SDL_CFLAGS@ @VISUAL_CFLAGS@ @DEBUG_CFLAGS@\
		-Wall -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes
		
simplesdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@
morphsdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@

bin_PROGRAMS = @EXAMPLES@

EXTRA_PROGRAMS = simplesdl morphsdl

simplesdl_SOURCES = simplesdl.c

morphsdl_SOURCES = morphsdl.c

