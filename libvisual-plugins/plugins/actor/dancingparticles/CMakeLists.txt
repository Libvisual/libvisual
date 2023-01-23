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

ADD_EXECUTABLE(ft2render
  ft2render.cpp
  font.cpp
# These are only need to fill all needed symbols for linking:
  actor_dancingparticles.cpp
  epic.cpp
  etoile.cpp
  gl.cpp
  parameters.cpp
  signal.cpp
)
TARGET_LINK_LIBRARIES(ft2render PRIVATE
  OpenGL::GL
  OpenGL::GLU
  Libvisual::Libvisual
  Freetype::Freetype
)
TARGET_INCLUDE_DIRECTORIES(ft2render PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)
TARGET_COMPILE_DEFINITIONS(ft2render PRIVATE
  DATA_DIR="${DANCINGPARTICLES_DATA_DIR}"
)
