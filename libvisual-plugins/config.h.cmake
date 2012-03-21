#ifndef CONFIG_H
#define CONFIG_H

#cmakedefine ENABLE_NLS
#cmakedefine GETTEXT_PACKAGE
#cmakedefine LOCALE_DIR @LOCALE_DIR@
#cmakedefine HAVE_GETTEXT

#cmakedefine HAVE_DLFCN_H
#cmakedefine HAVE_FCNTL_H

#cmakedefine HAVE_LIBASOUND
#cmakedefine HAVE_0_9_X_ALSA
#cmakedefine HAVE_1_X_X_ALSA

#cmakedefine HAVE_MMAP
#cmakedefine HAVE_GETTIMEOFDAY
#cmakedefine HAVE_POW
#cmakedefine HAVE_SQRT
#cmakedefine HAVE_FLOOR

#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_XF86VMODE

#cmakedefine STDC_HEADERS

#ifndef __cplusplus
#cmakedefine HAVE_C_INLINE
#cmakedefine HAVE_C_CONST
#cmakedefine inline @INLINE@
#ifndef HAVE_C_CONST
#  define const
#endif
#endif

#endif // CONFIG_H
