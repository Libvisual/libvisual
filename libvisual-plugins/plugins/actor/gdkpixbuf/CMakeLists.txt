SET(SOURCES
  actor_gdkpixbuf.c
)

LV_BUILD_ACTOR_PLUGIN(gdkpixbuf
  SOURCES      ${SOURCES}
  LINK_LIBS    PkgConfig::GTK
)
