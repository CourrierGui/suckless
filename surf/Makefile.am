AM_CFLAGS = $(GTK_CFLAGS)
AM_LIBS = $(GTK_LIBS)

bin_PROGRAMS = surf

LIBS += $(GTK_LIBS)

surf_SOURCES = \
	src/surf.c \
	include/surf.h \
	src/common.c \
	include/common.h \
	include/config.h \
	include/arg.h
surf_CPPFLAGS = \
	-DWEBEXTDIR=\"$(prefix)/lib/surf\" \
	-D_DEFAULT_SOURCE \
	-DGCR_API_SUBJECT_TO_CHANGE
