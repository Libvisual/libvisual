SET(DANCINGPARTICLES_DATA_DIR ${LV_PLUGINS_DATA_DIR}/actor/actor_dancingparticles)
SET(DANCINGPARTICLES_DATA_DIR_FULL ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_dancingparticles)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

ADD_DEFINITIONS(-DDATA_DIR="${DANCINGPARTICLES_DATA_DIR_FULL}")

SET(actor_dancingparticles_SOURCES
  actor_dancingparticles.cpp
  epic.cpp
  etoile.cpp
  gl.cpp
  parameters.cpp
  signal.cpp
  t1font.cpp
)

SET(actor_dancingparticles_SCRIPTS
  dance.epic
  dance.fig
)

ADD_LIBRARY(actor_dancingparticles MODULE ${actor_dancingparticles_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_dancingparticles
  ${LIBVISUAL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
)

INSTALL(TARGETS actor_dancingparticles LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
INSTALL(FILES ${actor_dancingparticles_SCRIPTS} DESTINATION ${DANCINGPARTICLES_DATA_DIR})
