SET(SOURCES
  compute.c
  compute.h
  display.c
  display.h
  renderer.c
  renderer.h
  file.c
  lv_infinite.c
  main.h
)

LV_BUILD_ACTOR_PLUGIN(infinite
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
