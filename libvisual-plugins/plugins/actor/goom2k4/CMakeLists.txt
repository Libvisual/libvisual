SET(GOOM2K4_DATA_DIR ${LV_PLUGINS_DATA_DIR}/actor/actor_goom2k4)
SET(GOOM2K4_DATA_DIR_FULL ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_goom2k4)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_goom2k4_SOURCES
  actor_goom2k4.c
  config_param.c
  convolve_fx.c
  cpu_info.c
  drawmethods.c
  filters.c
  flying_stars_fx.c
  gfontlib.c
  gfontrle.c
  goom_core.c
  goomsl.c
  goomsl_hash.c
  goomsl_heap.c
  goom_tools.c
  graphic.c
  ifs.c
  lines.c
  mathtools.c
  plugin_info.c
  sound_tester.c
  surf3d.c
  tentacle3d.c
  v3d.c
)

#IF(VISUAL_ARCH_X86)
#  LIST(APPEND actor_goom2k4_SOURCES
#    jitc_x86.c
#    filters_mmx.s
#    mmx.c
#  )
#ELSEIF(VISUAL_ARCH_POWERPC)
#  LIST(APPEND actor_goom2k4_SOURCES
#    ppc_zoom_ultimate.s
#    ppc_zoom_drawings.s
#  )
#ENDIF(VISUAL_ARCH_X86)

FLEX_TARGET(goomsl_lex goomsl_lex.l ${CMAKE_CURRENT_BINARY_DIR}/goomsl_lex.c)
BISON_TARGET(goomsl_yacc goomsl_yacc.y ${CMAKE_CURRENT_BINARY_DIR}/goomsl_yacc.c)
ADD_FLEX_BISON_DEPENDENCY(goomsl_lex goomsl_yacc)
LIST(APPEND actor_goom2k4_SOURCES
  ${FLEX_goomsl_lex_OUTPUTS}
  ${BISON_goomsl_yacc_OUTPUTS}
)

ADD_LIBRARY(actor_goom2k4 MODULE ${actor_goom2k4_SOURCES})
#-avoid-version

SET_TARGET_PROPERTIES(actor_goom2k4
  PROPERTIES COMPILE_FLAGS "-Wno-missing-braces"   # Needed to kill massive warnings with *motif.h
)

TARGET_LINK_LIBRARIES(actor_goom2k4
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_goom2k4 LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
