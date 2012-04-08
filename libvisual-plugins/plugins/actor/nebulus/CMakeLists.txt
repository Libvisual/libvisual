SET(NEBULUS_DATA_DIR ${LV_PLUGINS_DATA_DIR}/actor/actor_nebulus)
SET(NEBULUS_DATA_DIR_FULL ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_nebulus)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

ADD_DEFINITIONS(-DBITMAP_DIR="${NEBULUS_DATA_DIR_FULL}")

SET(actor_nebulus_SOURCES
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

FILE(GLOB actor_nebulus_IMAGES "*.bmp")

ADD_LIBRARY(actor_nebulus MODULE ${actor_nebulus_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_nebulus
  ${LIBVISUAL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
)

INSTALL(TARGETS actor_nebulus LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
INSTALL(FILES ${actor_nebulus_IMAGES} DESTINATION ${NEBULUS_DATA_DIR})
