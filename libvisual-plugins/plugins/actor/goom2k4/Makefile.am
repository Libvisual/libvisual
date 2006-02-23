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

actor_goom2k4_la_SOURCES = \
	goomsl_yacc.y goomsl_lex.l goomsl.c goomsl_hash.c goomsl_heap.c \
	goom_tools.c $(MMX_FILES) $(PPC_FILES) \
	config_param.c convolve_fx.c filters.c \
	flying_stars_fx.c gfontlib.c gfontrle.c \
	goom_core.c graphic.c ifs.c lines.c \
	mathtools.c sound_tester.c surf3d.c \
	tentacle3d.c plugin_info.c \
	v3d.c drawmethods.c \
	cpu_info.c actor_goom2k4.c \
	goom.h goom_plugin_info.h goom_typedefs.h \
	goom_graphic.h goom_config_param.h goom_visual_fx.h \
	goom_filters.h goom_tools.h goomsl.h goomsl_hash.h \
	goomsl_heap.h goom_tools.h goom_config.h

