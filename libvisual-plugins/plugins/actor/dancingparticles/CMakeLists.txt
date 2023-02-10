SET(DANCINGPARTICLES_DATA_DIR ${LV_PLUGIN_DATA_DIR}/actor/actor_dancingparticles)

SET(SOURCES
  actor_dancingparticles.cpp
  epic.cpp
  etoile.cpp
  gl.cpp
  parameters.cpp
  signal.cpp
  font.cpp
)

LV_BUILD_ACTOR_PLUGIN(dancingparticles
  SOURCES      ${SOURCES}
  COMPILE_DEFS DATA_DIR="${DANCINGPARTICLES_DATA_DIR}"
  LINK_LIBS    OpenGL::GL OpenGL::GLU Freetype::Freetype m
)

SET(DATA_FILES
  dance.epic
  dance.fig
  dancingparticles.conf
  font.pfb
  xmms.epic
  xmms.fig
)

INSTALL(FILES ${DATA_FILES} DESTINATION ${DANCINGPARTICLES_DATA_DIR})
