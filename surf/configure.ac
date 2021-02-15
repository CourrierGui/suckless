#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([surf], [2.0])
AC_CONFIG_SRCDIR([src/surf.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

PKG_CHECK_MODULES([GTK], [gtk+-3.0 gcr-3 webkit2gtk-4.0 webkit2gtk-web-extension-4.0])
AC_CHECK_LIB([X11], [XOpenDisplay])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/file.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([memset realpath regcomp strchr strstr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
