INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

IF(ARCH_X86)
  ADD_DEFINITIONS(-DX86_ASM_OPT)
  SET(EXTRA_SOURCES loopx86.s)
ELSE(ARCH_X86)
  SET(EXTRA_SOURCES loop.c)
ENDIF(ARCH_X86)

SET(actor_blursk_SOURCES
  actor_blursk.c
  actor_blursk.h
  blur.c
  blursk.c
  blursk.h
  color.c
  config.c
  config.h
  img.c
  render.c
  bitmap.c
  paste.c
  text.c
  ${EXTRA_SOURCES}
)

ADD_LIBRARY(actor_blursk MODULE ${actor_blursk_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_blursk
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_blursk LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
