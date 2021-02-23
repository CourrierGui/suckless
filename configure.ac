AC_PREREQ([2.68])
AC_INIT([suckless], [1.0])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 -Wall -Werror foreign])
# LT_PREREQ([2.2])

AC_CONFIG_SUBDIRS([dwm st surf dmenu])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

echo \
"-------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'.
Compiler: '${CC} ${CFLAGS} ${CPPFLAGS}'
Libraries: '${LIBS}'

Package features:

Now type 'make @<:@<target>@:>@'
where the optional <target> is:
all                - build all binaries
install            - install everything
--------------------------------------------------"
