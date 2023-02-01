## Process this file with automake to generate a Makefile.in

bin_PROGRAMS = $(TOOLS_LV_TOOL)

EXTRA_PROGRAMS = lv-tool-0.4

lv_tool_0_4_SOURCES = lv-tool.cpp
lv_tool_0_4_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/libvisual
lv_tool_0_4_CXXFLAGS = $(SDL_CFLAGS)
lv_tool_0_4_LDADD = $(SDL_LIBS) ../../libvisual/libvisual-@LIBVISUAL_VERSION_SUFFIX@.la

man1_MANS = lv-tool-0.4.1

EXTRA_DIST = lv-tool-0.4.1 sync-manpage-with-help-output.sh
