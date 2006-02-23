# libgoom2

#if HAVE_MMX
#MMX_FILES=mmx.c xmmx.c 
#else
#MMX_FILES=
#endif

#if HAVE_PPC
#PPC_FILES=ppc_zoom_ultimate.s ppc_drawings.s
#else
#PPC_FILES=
#endif
#-----
actor_plugin_LTLIBRARIES = actor_goom2k4.la

LIBS += $(LIBVISUAL_LIBS)

AM_CFLAGS = $(LIBVISUAL_CFLAGS)

INCLUDES = $(all_includes) -I$(top_srcdir)

actor_goom2k4_la_LDFLAGS = -module -avoid-version

GOOMSL_FILES = goomsl_yacc.c goomsl_yacc.h goomsl_lex.c

actor_goom2k4_la_SOURCES = \
	actor_goom2k4.c \
	config_param.c convolve_fx.c filters.c \
	cpu_info.c cpu_info.h \
	default_scripts.h \
	drawmethods.c drawmethods.h \
	flying_stars_fx.c  \
	gfontlib.c gfontlib.h \
	gfontrle.c gfontrle.h \
	goom.h \
	goom_config.h goom_config_param.h \
	goom_core.c \
	goom_filters.h \
	goom_fx.h \
	goom_graphic.h \
	goom_plugin_info.h \
	goom_visual_fx.h \
	goom_tools.c goom_tools.h \
	goom_typedefs.h \
	goomsl.c goomsl.h \
	goomsl_hash.c goomsl_hash.h \
	goomsl_heap.c goomsl_heap.h \
	goomsl_private.h \
	$(MMX_FILES) $(PPC_FILES) \
	graphic.c \
	ifs.c ifs.h \
	lines.c lines.h \
	mathtools.c mathtools.h \
	motif_goom1.h motif_goom2.h \
	plugin_info.c \
	sound_tester.c sound_tester.h \
	surf3d.c surf3d.h \
	tentacle3d.c tentacle3d.h \
	v3d.c v3d.h \
	$(GOOMSL_FILES)

EXTRA_DIST = goomsl_yacc.y goomsl_lex.l 
BUILT_SOURCES = $(GOOMSL_FILES)

CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = $(BUILT_SOURCES)

goomsl_yacc.c goomsl_yacc.h: goomsl_yacc.y
	$(YACC) -d -o$@ $<

goomsl_lex.c: goomsl_lex.l
	$(LEX) -o$@ $<
