#pragma once

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <sl-draw.h>

/* macros */
#define INTERSECT(x,y,w,h,r)  (MAX(0, MIN((x)+(w),(r).x_org+(r).width)  - MAX((x),(r).x_org)) \
                             * MAX(0, MIN((y)+(h),(r).y_org+(r).height) - MAX((y),(r).y_org)))
#define LENGTH(X)             (sizeof X / sizeof X[0])
#define TEXTW(X)              (drw_fontset_getwidth(drw, (X)) + lrpad)
#define NUMBERSMAXDIGITS      100
#define NUMBERSBUFSIZE        (NUMBERSMAXDIGITS * 2) + 1

#define OPAQUE                0xffU

enum { /* color schemes */
	SchemeNorm,
	SchemeSel,
	SchemeNormHighlight,
	SchemeSelHighlight,
	SchemeOut,
	SchemeLast
};

struct item {
	char *text;
	struct item *left, *right;
	int out;
	double distance;
};

typedef struct {
	FILE  *fp;    /* pointer to the history file */
	char **items; /* names of the items in the history */
	size_t size;  /* number of items in the history */
	size_t pos;   /* position of the cursor in the history */
} History;
