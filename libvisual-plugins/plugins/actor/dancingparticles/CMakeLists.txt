INCLUDE(LVBuildPlugin)

SET(DANCINGPARTICLES_DATA_DIR ${LV_PLUGINS_DATA_DIR}/actor/actor_dancingparticles)
SET(DANCINGPARTICLES_DATA_DIR_FULL ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_dancingparticles)

SET(SOURCES
  actor_dancingparticles.cpp
  epic.cpp
  etoile.cpp
  gl.cpp
  parameters.cpp
  signal.cpp
  t1font.cpp
)

LV_BUILD_ACTOR_PLUGIN(dancingparticles
  SOURCES      ${SOURCES}
  COMPILE_DEFS -DDATA_DIR="${DANCINGPARTICLES_DATA_DIR_FULL}"
  INCLUDE_DIRS ${OPENGL_INCLUDE_DIR}
  LINK_LIBS    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} m
)

SET(SCRIPTS
  dance.epic
  dance.fig
)

INSTALL(FILES ${SCRIPTS} DESTINATION ${DANCINGPARTICLES_DATA_DIR})
