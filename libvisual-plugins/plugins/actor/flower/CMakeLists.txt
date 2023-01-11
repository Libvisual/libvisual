SET(SOURCES
  actor_flower.c
  main.c
  main.h
  notch.c
  notch.h
)

LV_BUILD_ACTOR_PLUGIN(flower
  SOURCES      ${SOURCES}
  LINK_LIBS    OpenGL::GL OpenGL::GLU m
)
