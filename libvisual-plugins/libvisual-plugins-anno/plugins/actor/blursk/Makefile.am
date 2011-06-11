## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_blursk.la

LIBS += $(LIBVISUAL_LIBS)


INCLUDES = $(all_includes) -I$(top_srcdir) 

COMMON_SRC = actor_blursk.c actor_blursk.h blur.c blursk.c blursk.h color.c \
        config.c config.h img.c render.c bitmap.c paste.c text.c

if ARCH_X86

EXTRA_CFLAGS = -DX86_ASM_OPT
EXTRA_SRC = loopx86.s

else

EXTRA_CFLAGS =
EXTRA_SRC = loop.c 

endif

AM_CFLAGS = $(LIBVISUAL_CFLAGS) $(EXTRA_CFLAGS)

actor_blursk_la_LDFLAGS = -module -avoid-version 

actor_blursk_la_SOURCES = $(COMMON_SRC) $(EXTRA_SRC)

