/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx = 1;  /* border pixel of windows */
static const unsigned int snap     = 32; /* snap pixel */
static const int swallowfloating   = 0;  /* 1 means swallow floating windows by default */
static const unsigned int gappih   = 10; /* horiz inner gap between windows */
static const unsigned int gappiv   = 10; /* vert inner gap between windows */
static const unsigned int gappoh   = 10; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov   = 10; /* vert outer gap between windows and screen edge */
static const int smartgaps         = 0;  /* 1 means no outer gap when there is only one window */
static const int showbar           = 1;  /* 0 means no bar */
static const int topbar            = 1;  /* 0 means bottom bar */
static const char *fonts[]         = {
	"Source Code Pro:size=12",
	"JoyPixels:pixelsize=12:antialias=true:autohint=true"
};
static const char dmenufont[]      = "Source Code Pro:size=12";
static const char col_gray1[]      = "#222222";
static const char col_gray2[]      = "#444444";
static const char col_gray3[]      = "#bbbbbb";
static const char col_gray4[]      = "#eeeeee";
static const char col_cyan[]       = "#005577";
static const unsigned int baralpha = 0x70;
static const unsigned int borderalpha = OPAQUE;
static const char *colors[][3]     = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ "mpv",     NULL,     NULL,           0,         1,          0,           1,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         1,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY, view,       {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY, toggleview, {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY, tag,        {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY, toggletag,  {.ui = 1 << TAG} }

#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j, ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k, ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_a, ACTION##stack, {.i = 0 } }, \
	{ MOD, XK_z, ACTION##stack, {.i = 1 } }, \
	{ MOD, XK_e, ACTION##stack, {.i = 2 } }, \
	{ MOD, XK_r, ACTION##stack, {.i = 3 } }, \
	{ MOD, XK_x, ACTION##stack, {.i = -1 } }, \
	{ MOD, XK_p, ACTION##stack, {.i = PREVSEL } }

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

/* commands spawned when clicking statusbar, the mouse button pressed is exported as BUTTON */
/* static char *statuscmds[] = { "notify-send Mouse$BUTTON" }; */
static char *statuscmds[] = { "taskbar", "volume", "internet", "battery", "clock", "cpu", "memory" };
static char *statuscmd[] = { "/bin/sh", "-c", NULL, NULL };

static Key keys[] = {
	/* modifier         key        function        argument */
	STACKKEYS(MODKEY,           focus), // j, k to move cursor up and down, a, z, e, and r to move to client 0, 1, 2 and 3, x to move to the last one and p to the previous
	STACKKEYS(MODKEY|ShiftMask, push ), // same as above but move the client inside

	/* { MODKEY,           XK_i,      incnmaster,     {.i = +1 } }, */
	/* { MODKEY|ShiftMask, XK_i,      incnmaster,     {.i = -1 } }, */
	/* { MODKEY,           XK_Return, zoom,           {0} }, */

	{ MODKEY, XK_Tab,       view,      {0} }, // switch between previous and current tag
	{ MODKEY, XK_semicolon, shiftview, { .i =  1 } },
	{ MODKEY, XK_comma,     shiftview, { .i = -1 } },

	{ MODKEY|ShiftMask, XK_c,      killclient,     {0} }, // close the current client
	{ MODKEY|ShiftMask, XK_q,      quit,           {0} }, // quit dwm

	// Layouts
	{ MODKEY,           XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,           XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,           XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,           XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask, XK_space,  togglefloating, {0} },

	{ MODKEY,             XK_agrave, view, {.ui = ~0 } }, // display all tags: all the clients in the screen
	{ MODKEY|ShiftMask,   XK_agrave, view, {.ui = ~0 } }, // put the client in all the tags

	{ MODKEY,           XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,           XK_Return, spawn,          {.v = termcmd } },
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

	{ MODKEY,                       XK_b,      togglebar,   {0} },          // toggle the top bar
	{ MODKEY,                       XK_l,      setmfact,    {.f = +0.05} }, // increase master size
	{ MODKEY,                       XK_h,      setmfact,    {.f = -0.05} }, // decrease master size

	{ MODKEY,                       XK_u,      togglegaps,  {0}        },   // toggle gaps around clients
	{ MODKEY|ShiftMask,             XK_u,      defaultgaps, {0}        },   // reset clients graps

	{ MODKEY|ControlMask,           XK_k,      incrgaps,    {.i = +3 } },   // increase gaps between clients
	{ MODKEY|ControlMask,           XK_j,      incrgaps,    {.i = -3 } },   // decrease gaps between clients
	{ MODKEY|ControlMask|ShiftMask, XK_k,      incrogaps,   {.i = +3 } },   // increase gaps around clients
	{ MODKEY|ControlMask|ShiftMask, XK_j,      incrogaps,   {.i = -3 } },   // decrease gaps around clients
	{ MODKEY|ControlMask|Mod1Mask,  XK_k,      incrigaps,   {.i = +3 } },   // increase gaps inside clients
	{ MODKEY|ControlMask|Mod1Mask,  XK_j,      incrigaps,   {.i = -3 } },   // decrease gaps inside clients

	{ MODKEY,                       XK_i,      incrihgaps,  {.i = +3 } },   // increase horizontal gaps inside clients
	{ MODKEY|ShiftMask,             XK_i,      incrihgaps,  {.i = -3 } },   // decrease horizontal gaps inside clients
	{ MODKEY|ControlMask,           XK_i,      incrivgaps,  {.i = +3 } },   // increase vertical gaps inside clients
	{ MODKEY|ShiftMask|ControlMask, XK_i,      incrivgaps,  {.i = -3 } },   // decrease vertical gaps inside clients

	{ MODKEY,                       XK_o,      incrohgaps,  {.i = +3 } },   // increase horizontal gaps around clients
	{ MODKEY|ShiftMask,             XK_o,      incrohgaps,  {.i = -3 } },   // decrease horizontal gaps around clients
	{ MODKEY|ControlMask,           XK_o,      incrovgaps,  {.i = +3 } },   // increase vertical gaps around clients
	{ MODKEY|ShiftMask|ControlMask, XK_o,      incrovgaps,  {.i = -3 } },   // decrease vertical gaps around clients

	{ 0, XK_Print,                 spawn, SHCMD("scrot ~/Pictures/screenshots/%Y-%m-%d-%T-screenshot.png") },
	{ 0, XF86XK_Sleep,             spawn, SHCMD("slock")                                                   },
	{ 0, XF86XK_AudioMute,         spawn, SHCMD("pamixer -t; kill -44 $(pidof dwmblocks)")                 },
	{ 0, XF86XK_AudioRaiseVolume,  spawn, SHCMD("pamixer --allow-boost -i 3; kill -44 $(pidof dwmblocks)") },
	{ 0, XF86XK_AudioLowerVolume,  spawn, SHCMD("pamixer --allow-boost -d 3; kill -44 $(pidof dwmblocks)") },
	{ 0, XF86XK_MonBrightnessUp,   spawn, SHCMD("xbacklight -inc 15")                                      },
	{ 0, XF86XK_MonBrightnessDown, spawn, SHCMD("xbacklight -dec 15")                                      },
	{ 0, XF86XK_TouchpadToggle,    spawn, SHCMD("toggle-touchpad")                                         },
	{ 0, XF86XK_ScreenSaver,       spawn, SHCMD("[ $(xbacklight -get | sed \"s/\\.[0-9]\\+//\") = \"0\" ] && xbacklight -set 100 || xbacklight -set 0") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = statuscmd } },
	{ ClkStatusText,        0,              Button3,        spawn,          {.v = statuscmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
