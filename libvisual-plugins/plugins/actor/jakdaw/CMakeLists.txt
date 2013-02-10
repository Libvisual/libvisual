SET(SOURCES
  actor_jakdaw.c
  actor_jakdaw.h
  feedback.c
  feedback.h
  plotter.c
  plotter.h
)

LV_BUILD_ACTOR_PLUGIN(jakdaw
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
