#pragma once

#include "slock.h"

/* user and group to drop privileges to */
static const char *user  = "guillaume";
static const char *group = "guillaume";

static const char *colorname[NUMCOLS] = {
	[INIT] =   "black",     /* after initialization */
	[INPUT] =  "#005577",   /* during input */
	[FAILED] = "#CC3333",   /* wrong password */
};

/* treat a cleared input like a wrong password (color) */
static const int failonclear = 1;

/*Set Blur radius*/
static const int blurRadius=1;
static const int pixelSize=5;
