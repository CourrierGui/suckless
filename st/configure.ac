#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([st], [0.8.4])
AC_CONFIG_SRCDIR([src/st.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES([Fonts], [freetype2])
AC_CHECK_LIB([util], [openpty])
AC_CHECK_LIB([Xft], [XftDrawCreate])
AC_CHECK_LIB([fontconfig], [FcDefaultSubstitute])
AC_CHECK_LIB([X11], [XCloseDisplay])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([fcntl.h limits.h locale.h stdint.h stdlib.h string.h sys/ioctl.h termios.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime dup2 memchr memmove memset setenv setlocale strchr strdup strerror strtol wcwidth])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
