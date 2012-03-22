INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/plugins/actor/oinksie
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_oinksie_SOURCES
  actor_oinksie.c
  oinksie.c
  oinksie.h
  config.c
  oink-config.h
  gfx-background.c
  gfx-background.h
  gfx-misc.c
  gfx-misc.h
  gfx-scope.c
  gfx-scope.h
  screen.c
  screen.h
  audio.c
  audio.h
  gfx-analyzer.c
  gfx-analyzer.h
  gfx-blur.c
  gfx-blur.h
  gfx-palette.c
  gfx-palette.h
  misc.c
  misc.h
  scene.c
  scene.h
  table.c
  table.h
  config-defines.h
  common.h
  actor_oinksie.c
)

ADD_LIBRARY(actor_oinksie MODULE ${actor_oinksie_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_oinksie
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_oinksie LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
