## Process this file with automake to produce Makefile.in

DEFS = @DEFS@

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/examples

# for now CFLAGS is common for all targets
AM_CFLAGS = @CFLAGS@ @VISUAL_CFLAGS@ @DEBUG_CFLAGS@\
		-Wall -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes
		
lv_inspect_LDADD = @VISUAL_LIBS@

bin_PROGRAMS = lv-inspect

lv_inspect_SOURCES = lv-inspect.c

