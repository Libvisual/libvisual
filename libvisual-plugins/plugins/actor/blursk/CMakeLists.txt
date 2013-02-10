SET(SOURCES
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
)

SET(EXTRA_SOURCES loop.c)
# FIXME: Factor this out into the top-level CMakeLists.txt
IF(TARGET_ARCH_X86)
  ENABLE_LANGUAGE(ASM-ATT)
  IF(CMAKE_ASM-ATT_COMPILER_WORKS)
	SET(EXTRA_SOURCES loopx86.s)
  ENDIF()
ENDIF()

LV_BUILD_PLUGIN(blursk actor
  SOURCES   ${SOURCES} ${EXTRA_SOURCES}
  LINK_LIBS m
)
