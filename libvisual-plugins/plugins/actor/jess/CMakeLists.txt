INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_jess_SOURCES
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

ADD_LIBRARY(actor_jess MODULE ${actor_jess_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_jess
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_jess LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
