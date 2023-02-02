LV_BUILD_INPUT_PLUGIN(portaudio
  SOURCES       input_portaudio.c
  LINK_LIBS     PkgConfig::PORTAUDIO
)
