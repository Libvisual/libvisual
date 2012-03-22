prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=@CMAKE_INSTALL_FULL_LIBDIR@
includedir=@LV_INCLUDE_DIR_FULL@
pluginsbasedir=@LV_PLUGIN_DIR_FULL@

Name: libvisual
Version: @LV_VERSION@
Description: An audio visualization abstraction library.
Requires: @LV_PKG_CONFIG_DEP@
Libs: -L${libdir} @LV_PKG_CONFIG_LIBS@
Cflags: -I${includedir} @LV_PKG_CONFIG_CFLAGS@
