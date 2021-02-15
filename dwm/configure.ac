#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dwm], [6.2])
AC_CONFIG_SRCDIR([src/dwm.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES([Fonts], [freetype2])
AC_CHECK_LIB([Xrender], [XRenderFindVisualFormat])
AC_CHECK_LIB([xcb-res], [xcb_res_query_client_ids])
AC_CHECK_LIB([X11-xcb], [XGetXCBConnection])
AC_CHECK_LIB([Xft], [XftDrawCreate])
AC_CHECK_LIB([fontconfig], [FcDefaultSubstitute])
AC_CHECK_LIB([X11], [XCloseDisplay])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([locale.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([setlocale strstr strtoul])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
