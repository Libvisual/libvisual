INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(EXTRA_SOURCES loop.c)

# FIXME: Factor this out into the top-level CMakeLists.txt
IF(TARGET_ARCH_X86)
  ENABLE_LANGUAGE(ASM-ATT)
  IF(CMAKE_ASM-ATT_COMPILER_WORKS)
	SET(EXTRA_SOURCES loopx86.s)
  ENDIF(CMAKE_ASM-ATT_COMPILER_WORKS)
ENDIF(TARGET_ARCH_X86)

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
