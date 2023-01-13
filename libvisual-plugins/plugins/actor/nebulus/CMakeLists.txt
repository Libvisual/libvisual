SET(NEBULUS_DATA_DIR ${LV_PLUGIN_DATA_DIR}/actor/actor_nebulus)

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
  COMPILE_DEFS BITMAP_DIR="${NEBULUS_DATA_DIR}"
  LINK_LIBS    OpenGL::GL OpenGL::GLU m
)

FILE(GLOB actor_nebulus_IMAGES "*.bmp")
INSTALL(FILES ${actor_nebulus_IMAGES} DESTINATION ${NEBULUS_DATA_DIR})
