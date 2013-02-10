SET(DANCINGPARTICLES_DATA_DIR ${LV_PLUGIN_DATA_DIR}/actor/actor_dancingparticles)

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
  COMPILE_DEFS -DDATA_DIR="${DANCINGPARTICLES_DATA_DIR}"
  INCLUDE_DIRS ${OPENGL_INCLUDE_DIR} ${T1LIB_INCLUDE_DIR}
  LINK_LIBS    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${T1LIB_LIBRARY} m
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
