LV_BUILD_INPUT_PLUGIN(mplayer
  SOURCES      input_mplayer.c
  COMPILE_DEFS -D_GNU_SOURCE # required for mremap()
)
