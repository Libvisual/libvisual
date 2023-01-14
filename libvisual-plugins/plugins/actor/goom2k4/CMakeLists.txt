SET(GOOM2K4_DATA_DIR ${LV_PLUGIN_DATA_DIR}/actor/actor_goom2k4)

SET(SOURCES
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
#  LIST(APPEND SOURCES
#    jitc_x86.c
#    filters_mmx.s
#    mmx.c
#  )
#ELSEIF(VISUAL_ARCH_POWERPC)
#  LIST(APPEND SOURCES
#    ppc_zoom_ultimate.s
#    ppc_zoom_drawings.s
#  )
#ENDIF()

FLEX_TARGET(goomsl_lex goomsl_lex.l ${CMAKE_CURRENT_BINARY_DIR}/goomsl_lex.c)
BISON_TARGET(goomsl_yacc goomsl_yacc.y ${CMAKE_CURRENT_BINARY_DIR}/goomsl_yacc.c)
ADD_FLEX_BISON_DEPENDENCY(goomsl_lex goomsl_yacc)
LIST(APPEND SOURCES
  ${FLEX_goomsl_lex_OUTPUTS}
  ${BISON_goomsl_yacc_OUTPUTS}
)

LV_BUILD_ACTOR_PLUGIN(goom2k4
  SOURCES         ${SOURCES}
  COMPILE_OPTIONS -Wno-missing-braces # Needed to kill massive warnings with *motif.h
  LINK_LIBS       m
)
