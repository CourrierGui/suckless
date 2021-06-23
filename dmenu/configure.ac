AC_PREREQ([2.71])
AC_INIT([dmenu], [5.0])
AC_CONFIG_SRCDIR([apps/dmenu.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
PKG_CHECK_MODULES([Fonts], [freetype2])
AC_CHECK_LIB([Xft], [XftDrawCreate])
AC_CHECK_LIB([fontconfig], [FcDefaultSubstitute])
AC_CHECK_LIB([X11], [XCloseDisplay])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([strings.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
PKG_CHECK_MODULES([Fonts], [freetype2])
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove setlocale strchr strdup strncasecmp strtol])

AC_CONFIG_FILES([Makefile])
CC_OPT="-Wall -Wextra -Weffc++ -Wpedantic -std=c++2a"
AC_OUTPUT
