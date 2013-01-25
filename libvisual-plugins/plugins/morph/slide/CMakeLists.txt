FOREACH(DIRECTION left right up down)
  SET(NAME "slide_${DIRECTION}")
  LV_BUILD_MORPH_PLUGIN(${NAME}
    SOURCES morph_${NAME}.c common.c
  )
ENDFOREACH()
