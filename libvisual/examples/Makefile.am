## Process this file with automake to produce Makefile.in

DEFS = @DEFS@

# for now CFLAGS is common for all targets
AM_CFLAGS = @CFLAGS@ @SDL_CFLAGS@ @VISUAL_CFLAGS@\
		-Wall -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/examples

bin_PROGRAMS = simplesdl morphsdl

simplesdl_SOURCES = simplesdl.c
simplesdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@

morphsdl_SOURCES = morphsdl.c
morphsdl_LDADD = @SDL_LIBS@ @VISUAL_LIBS@

