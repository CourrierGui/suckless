/* See LICENSE file for copyright and license details. */
#pragma once

#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <X11/Xlib.h>

#include "dwm.h"

#define OPAQUE 0xffU

/* appearance */
static unsigned int borderpx        = 1;  /* border pixel of windows */
static unsigned int snap            = 32; /* snap pixel */
static const int    swallowfloating = 0;  /* 1 means swallow floating windows by default */
static unsigned int gappih          = 10; /* horiz inner gap between windows */
static unsigned int gappiv          = 10; /* vert inner gap between windows */
static unsigned int gappoh          = 10; /* horiz outer gap between windows and screen edge */
static unsigned int gappov          = 10; /* vert outer gap between windows and screen edge */
static int          smartgaps       = 0;  /* 1 means no outer gap when there is only one window */
static int          showbar         = 1;  /* 0 means no bar */
static int          topbar          = 1;  /* 0 means bottom bar */
static const int    vertpad         = 9; /* vertical padding of bar */
static const int    sidepad         = 9; /* horizontal padding of bar */
static char         main_font[]     = "Source Code Pro:size=12";
static char         second_font[]   = "JoyPixels:pixelsize=14:antialias=true:autohint=true";
static char*  fonts[]               = {
  main_font, second_font
};
static char dmenufont[]       = "Source Code Pro:size=12";
static char normbgcolor[]     = "#222222"; // tags and status bar font color
static char normfgcolor[]     = "#444444"; // border of clients ?
static char normbordercolor[] = "#444444"; // tags and status bar background color
static char selfgcolor[]      = "#004466"; // selected tag and window title font color
static char selbgcolor[]      = "#aaaaaa"; // selected tag and window title font color
static char selbordercolor[]  = "#eeeeee"; // selected tag and window title font color
static char dmenusfg[]        = "#eeeeee"; // selected tag and window title font color
static char dmenunfg[]        = "#eeeeee"; // selected tag and window title font color

static const unsigned int baralpha    = 0x90;   // top bar opacity
static const unsigned int borderalpha = 0x100;
static unsigned int fontalpha   = OPAQUE;

static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char* tags[] = {
  "\u2160", "\u2161", "\u2162",
  "\u2163", "\u2164", "\u2165",
  "\u2166", "\u2167", "\u2168",
};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,    NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", NULL,    NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "st",      NULL,    NULL,           0,         0,          1,          -1,        -1 },
	{ "mpv",     NULL,    NULL,           0,         1,          0,           1,        -1 },
	/* xev */
	{ NULL,      NULL,    "Event Tester", 0,         1,          0,           1,        -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} }

#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_grave, ACTION##stack, {.i = PREVSEL } }, \
	{ MOD, XK_a,     ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_z,     ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_e,     ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_r,     ACTION##stack, {.i = -1 } }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {
    "dmenu_run", "-m", dmenumon,
    "-fn", dmenufont,
    "-nb", normbgcolor, "-nf", normfgcolor,
    "-sb", selbordercolor, "-sf", selfgcolor,
    "-l", "10", NULL
};
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
	{ "normbgcolor",     STRING,  &normbgcolor     },
	{ "normfgcolor",     STRING,  &normfgcolor     },
	{ "normbordercolor", STRING,  &normbordercolor },
	{ "selbgcolor",      STRING,  &selbgcolor      },
	{ "selfgcolor",      STRING,  &selfgcolor      },
	{ "selbordercolor",  STRING,  &selbordercolor  },
	{ "dmenusfg",        STRING,  &dmenusfg        },
	{ "dmenunfg",        STRING,  &dmenunfg        },

	{ "borderpx",        INTEGER, &borderpx        },
	{ "snap",            INTEGER, &snap            },
	{ "showbar",         INTEGER, &showbar         },
	{ "topbar",          INTEGER, &topbar          },
	{ "nmaster",         INTEGER, &nmaster         },
	{ "resizehints",     INTEGER, &resizehints     },
	{ "mfact",           FLOAT,   &mfact           },

	{ "gappih",          INTEGER, &gappih          },
	{ "gappiv",          INTEGER, &gappiv          },
	{ "gappoh",          INTEGER, &gappoh          },
	{ "gappov",          INTEGER, &gappov          },
};

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
static char *statuscmds[] = { "notify-send Mouse$BUTTON" };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

static Key keys[] = {
	/* modifier                     key        function        argument */

	/* j, k to move cursor up and down, a, z, e, and r to move to client 0, 1, 2
	 * and 3, x to move to the last one and p to the previous */
	STACKKEYS(MODKEY,           focus),
	// same as above but move the client inside
	STACKKEYS(MODKEY|ShiftMask, push),

	{ MODKEY, XK_Tab,       view,      {0}       }, // switch between previous and current tag
	{ MODKEY, XK_semicolon, shiftview, { .i= 1 } }, // increase all tags by one
	{ MODKEY, XK_comma,     shiftview, { .i=-1 } }, // decrease all tags by one

	{ MODKEY|ShiftMask, XK_c,      killclient,     {0} }, // close the current client
	{ MODKEY|ShiftMask, XK_q,      quit,           {0} }, // quit dwm

	// Layouts
	// tile layout
	{ MODKEY,           XK_t,      setlayout,      {.v = &layouts[0]} },
	// floating layout
	{ MODKEY,           XK_f,      setlayout,      {.v = &layouts[1]} },
	// monocle layout
	{ MODKEY,           XK_m,      setlayout,      {.v = &layouts[2]} },
	// toggle between the last 2 layouts
	{ MODKEY,           XK_space,  setlayout,      {0} },
	// toggle current client between floating or not
	{ MODKEY|ShiftMask, XK_space,  togglefloating, {0} },
	{ MODKEY|ShiftMask, XK_f,      togglefullscr,  {0} },

	// display all tags: all the clients in the screen
	{ MODKEY,             XK_agrave, view, {.ui = ~0 } },
	// put the client in all the tags
	{ MODKEY|ShiftMask,   XK_agrave, tag,  {.ui = ~0 } },

	{ MODKEY,           XK_d,      spawn,          {.v = dmenucmd }  },
	{ MODKEY,           XK_Return, spawn,          {.v = termcmd  }  },
	{ MODKEY,           XK_w,      spawn,          SHCMD("$BROWSER") },
	{ MODKEY,           XK_c,      spawn,          SHCMD("st -e calcurse") },

	TAGKEYS(XK_ampersand,  0), // super + (&|é|"|'|(|-|è|_|ç) display corresponding tag
	TAGKEYS(XK_eacute,     1), // if you add one of the following:
	TAGKEYS(XK_quotedbl,   2), // * shift: move client to corresponding tag
	TAGKEYS(XK_quoteright, 3), // * control: add corresponding tag to display
	TAGKEYS(XK_parenleft,  4), // * control+shift: add or remove client from corresponding tag
	TAGKEYS(XK_minus,      5),
	TAGKEYS(XK_egrave,     6),
	TAGKEYS(XK_underscore, 7),
	TAGKEYS(XK_ccedilla,   8),

	// toggle the top bar
	{ MODKEY,                       XK_b,      togglebar,   {0} },
	// increase master size
	{ MODKEY,                       XK_l,      setmfact,    {.f = +0.05} },
	// decrease master size
	{ MODKEY,                       XK_h,      setmfact,    {.f = -0.05} },

	// toggle gaps around clients
	{ MODKEY,                       XK_u,      togglegaps,  {0}        },
	// reset clients graps
	{ MODKEY|ShiftMask,             XK_u,      defaultgaps, {0}        },

	// increase gaps between clients
	{ MODKEY|ControlMask,           XK_k,      incrgaps,    {.i = +3 } },
	// decrease gaps between clients
	{ MODKEY|ControlMask,           XK_j,      incrgaps,    {.i = -3 } },
	// increase gaps around clients
	{ MODKEY|ControlMask|ShiftMask, XK_k,      incrogaps,   {.i = +3 } },
	// decrease gaps around clients
	{ MODKEY|ControlMask|ShiftMask, XK_j,      incrogaps,   {.i = -3 } },
	// increase gaps inside clients
	{ MODKEY|ControlMask|Mod1Mask,  XK_k,      incrigaps,   {.i = +3 } },
	// decrease gaps inside clients
	{ MODKEY|ControlMask|Mod1Mask,  XK_j,      incrigaps,   {.i = -3 } },

	// increase horizontal gaps inside clients
	{ MODKEY,                       XK_i,      incrihgaps,  {.i = +3 } },
	// decrease horizontal gaps inside clients
	{ MODKEY|ShiftMask,             XK_i,      incrihgaps,  {.i = -3 } },
	// increase vertical gaps inside clients
	{ MODKEY|ControlMask,           XK_i,      incrivgaps,  {.i = +3 } },
	// decrease vertical gaps inside clients
	{ MODKEY|ShiftMask|ControlMask, XK_i,      incrivgaps,  {.i = -3 } },

	// increase horizontal gaps around clients
	{ MODKEY,                       XK_o,      incrohgaps,  {.i = +3 } },
	// decrease horizontal gaps around clients
	{ MODKEY|ShiftMask,             XK_o,      incrohgaps,  {.i = -3 } },
	// increase vertical gaps around clients
	{ MODKEY|ControlMask,           XK_o,      incrovgaps,  {.i = +3 } },
	// decrease vertical gaps around clients
	{ MODKEY|ShiftMask|ControlMask, XK_o,      incrovgaps,  {.i = -3 } },

	{ 0,         XK_Print,                 spawn, SHCMD("flameshot full -p ~/Pictures/screenshots")                },
	{ ShiftMask, XK_Print,                 spawn, SHCMD("flameshot gui -p ~/Pictures/screenshots")                 },
	{ 0,         XF86XK_Sleep,             spawn, SHCMD("slock")                                                   },
	{ 0,         XF86XK_AudioMute,         spawn, SHCMD("sound toggle")                 },
	{ 0,         XF86XK_AudioRaiseVolume,  spawn, SHCMD("sound inc 3%") },
	{ 0,         XF86XK_AudioLowerVolume,  spawn, SHCMD("sound dec 3%") },
	{ 0,         XF86XK_MonBrightnessUp,   spawn, SHCMD("xbacklight -inc 15; kill -45 $(pidof dwmblocks)")         },
	{ 0,         XF86XK_MonBrightnessDown, spawn, SHCMD("xbacklight -dec 15; kill -45 $(pidof dwmblocks)")         },
	{ 0,         XF86XK_TouchpadToggle,    spawn, SHCMD("toggle-touchpad")                                         },
	{ 0,         XF86XK_ScreenSaver,       spawn, SHCMD("toggle-backlight")                                        },
	{ MODKEY,    XK_F1,                    spawn, SHCMD("bootmenu")                                                },
};


/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */

	/* Handle clicks on the layout text */
	// swap between the 2 previous layouts
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	// set the floating layout
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[1]} },

	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },

	/* Handle clicks on the status bar */
	{ ClkStatusText,        0,              Button1,        sigdwmblocks,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigdwmblocks,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigdwmblocks,   {.i = 3} },
	{ ClkStatusText,        0,              Button4,        sigdwmblocks,   {.i = 4} },
	{ ClkStatusText,        0,              Button5,        sigdwmblocks,   {.i = 5} },
	{ ClkStatusText,        ShiftMask,      Button1,        sigdwmblocks,   {.i = 6} },

	// drag the client with the mouse
	{ ClkClientWin,         MODKEY,           Button1,      movemouse,      {0} },
	// toggle floating for the clicked client
	{ ClkClientWin,         MODKEY,           Button2,      togglefloating, {0} },
	// resize the window
	{ ClkClientWin,         MODKEY|ShiftMask, Button1,      resizemouse,    {0} },

	// display corresponding tag
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	// toggle the display of the corresponding tag
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	// move the current client to the corresponding tag
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	// toggle the corresponding tag for the current client
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
