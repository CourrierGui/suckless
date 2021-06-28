#pragma once

#include <limits.h>
#include <X11/Xlib.h>
#include "st.h"

/* types used in config.h */
typedef struct {
	uint mod;
	KeySym keysym;
	void (*func)(const Arg *);
	const Arg arg;
} Shortcut;

typedef struct {
	uint mod;
	uint button;
	void (*func)(const Arg *);
	const Arg arg;
	uint  release;
} MouseShortcut;

typedef struct {
	KeySym k;
	uint mask;
	char *s;
	/* three-valued logic variables: 0 indifferent, 1 on, -1 off */
	signed char appkey;    /* application keypad */
	signed char appcursor; /* application cursor */
} Key;

/* X modifiers */
#define XK_ANY_MOD    UINT_MAX
#define XK_NO_MOD     0
#define XK_SWITCH_MOD (1<<13)

/* function definitions used in config.h */
void clipcopy(const Arg *);
void clippaste(const Arg *);
void numlock(const Arg *);
void selpaste(const Arg *);
void zoom(const Arg *);
void zoomabs(const Arg *);
void zoomreset(const Arg *);
void ttysend(const Arg *);
void xsetsel(char *str);
void xclipcopy(void);
void xdrawline(Line line, int x1, int y1, int x2);
