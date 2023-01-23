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
  COMPILE_DEFS DATA_DIR="${DANCINGPARTICLES_DATA_DIR}"
  LINK_LIBS    OpenGL::GL OpenGL::GLU T1lib::T1lib m
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

ADD_EXECUTABLE(t1render
  t1font.cpp
  t1render.cpp
# These are only need to fill all needed symbols for linking:
  actor_dancingparticles.cpp
  epic.cpp
  etoile.cpp
  gl.cpp
  parameters.cpp
  signal.cpp
)
TARGET_LINK_LIBRARIES(t1render PRIVATE
  OpenGL::GL
  OpenGL::GLU
  Libvisual::Libvisual
  T1lib::T1lib
)
TARGET_INCLUDE_DIRECTORIES(t1render PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)
TARGET_COMPILE_DEFINITIONS(t1render PRIVATE
  DATA_DIR="${DANCINGPARTICLES_DATA_DIR}"
)
