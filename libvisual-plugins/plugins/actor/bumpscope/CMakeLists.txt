SET(SOURCES
  actor_bumpscope.c
  actor_bumpscope.h
  bump_scope.c
  bump_scope.h
)

LV_BUILD_ACTOR_PLUGIN(bumpscope
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
