## Process this file with automake to generate a Makefile.in

actor_plugin_LTLIBRARIES = actor_lv_gltest.la

actor_lv_gltest_la_CFLAGS = -I$(top_srcdir) $(LIBVISUAL_CFLAGS) $(X_CFLAGS)
actor_lv_gltest_la_LDFLAGS = -module -avoid-version 
actor_lv_gltest_la_LIBADD = $(LIBVISUAL_LIBS) $(X_LIBS) -lGL -lGLU 
actor_lv_gltest_la_SOURCES = actor_lv_gltest.c
