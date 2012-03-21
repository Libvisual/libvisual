#ifndef CONFIG_H
#define CONFIG_H

#cmakedefine PACKAGE           "@PACKAGE@"
#cmakedefine PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"
#cmakedefine PACKAGE_NAME      "@PACKAGE_NAME@"
#cmakedefine PACKAGE_STRING    "@PACKAGE_STRING@"
#cmakedefine PACKAGE_TARNAME   "@PACKAGE_TARNAME@"
#cmakedefine PACKAGE_URL       "@PACKAGE_URL@"
#cmakedefine PACKAGE_VERSION   "@PACKAGE_VERSION@"

#cmakedefine ENABLE_NLS
#cmakedefine GETTEXT_PACKAGE   "@GETTEXT_PACKAGE@"
#cmakedefine HAVE_GETTEXT
#cmakedefine HAVE_ICONV
#cmakedefine LOCALE_DIR        "@LOCALE_DIR@"

#cmakedefine HAVE_DIRENT_H
#cmakedefine HAVE_DLFCN_H
#cmakedefine HAVE_FCNTL_H
#cmakedefine HAVE_INTTYPES_H
#cmakedefine HAVE_SCHED_H
#cmakedefine HAVE_STDINT_H
#cmakedefine HAVE_STRDUP
#cmakedefine HAVE_STRNDUP
#cmakedefine HAVE_SYSCONF
#cmakedefine HAVE_SYS_SCHED_H
#cmakedefine HAVE_SYS_SELECT_H
#cmakedefine HAVE_SYS_SOCKET_H
#cmakedefine HAVE_SYS_STAT_H
#cmakedefine HAVE_SYS_TIME_H
#cmakedefine HAVE_SYS_TYPES_H
#cmakedefine HAVE_UNISTD_H

#cmakedefine HAVE_GETTIMEOFDAY
#cmakedefine HAVE_USLEEP
#cmakedefine HAVE_NANOSLEEP
#cmakedefine HAVE_SELECT
#cmakedefine HAVE_SQRT

#cmakedefine SIZEOF_INT    @SIZEOF_INT@
#cmakedefine SIZEOF_LONG   @SIZEOF_LONG@
#cmakedefine SIZEOF_SHORT  @SIZEOF_SHORT@
#cmakedefine SIZEOF_SIZE_T @SIZEOF_SIZE_T@

#cmakedefine LV_HAVE_GTHREAD2

#ifndef __cplusplus
#cmakedefine HAVE_C_INLINE
#cmakedefine HAVE_C_CONST
#cmakedefine inline @INLINE@
#ifndef HAVE_C_CONST
#  define const
#endif
#endif

#endif /* CONFIG_H */
