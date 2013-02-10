SET(SOURCES
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

LV_BUILD_ACTOR_PLUGIN(oinksie
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
