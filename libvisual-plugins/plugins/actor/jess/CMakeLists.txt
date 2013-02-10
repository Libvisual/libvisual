SET(SOURCES
  analyser.c
  analyser.h
  analyser_struct.h
  distorsion.c
  distorsion.h
  draw.c
  draw.h
  draw_low_level.c
  draw_low_level.h
  jess.c
  jess.h
  pal.c
  pal.h
  projection.c
  projection.h
  renderer.c
  renderer.h
  def.h
  struct.h
)

LV_BUILD_ACTOR_PLUGIN(jess
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
