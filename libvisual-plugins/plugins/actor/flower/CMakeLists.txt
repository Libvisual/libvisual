SET(SOURCES
  actor_flower.c
  main.c
  main.h
  notch.c
  notch.h
)

LV_BUILD_ACTOR_PLUGIN(flower
  SOURCES      ${SOURCES}
  INCLUDE_DIRS ${OPENGL_INCLUDE_DIR}
  LINK_LIBS    ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} m
)
