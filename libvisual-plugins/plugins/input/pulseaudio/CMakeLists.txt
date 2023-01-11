LV_BUILD_INPUT_PLUGIN(pulseaudio
  SOURCES       input_pulseaudio.c
  LINK_LIBS     PkgConfig::PULSE
)
