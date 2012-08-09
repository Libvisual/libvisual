INCLUDE(LVBuildPlugin)

SET(NEBULUS_DATA_DIR ${LV_PLUGINS_DATA_DIR}/actor/actor_nebulus)
SET(NEBULUS_DATA_DIR_FULL ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_nebulus)

SET(SOURCES
  child.c
  child_data.c
  draw_gl.c
  energy.c
  face.c
  face_data.c
  glthreads.c
  knot.c
  nebulus.c
  spectrum.c
  tentacles.c
  tunnel.c
  twist.c
)

LV_BUILD_ACTOR_PLUGIN(nebulus
  SOURCES      ${SOURCES}
  COMPILE_DEFS -DBITMAP_DIR="${NEBULUS_DATA_DIR_FULL}"
  INCLUDE_DIRS ${OPENGL_INCLUDE_DIR}
  LINK_LIBS    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} m
)

FILE(GLOB actor_nebulus_IMAGES "*.bmp")
INSTALL(FILES ${actor_nebulus_IMAGES} DESTINATION ${NEBULUS_DATA_DIR})
