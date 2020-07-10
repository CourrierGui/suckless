//Modify this file to change what commands output to your statusbar, and recompile using the make command.

static const Block blocks[] = {
	/*Icon*/  /*Command*/                  /*Update Interval*/  /*Update Signal*/
	{"",      "taskbar",                  30,                   9},
	{"",      "volume",                    0,                  10},
	{"",      "internet",                  5,                   4},
	{"",      "battery | tr \'\n\' \' \'", 5,                   3},
	{"",      "clock",                     1,                   8},
	//{"",	"pacpackages",	0,	8},
	//{"",	"news",		0,	6},
	/* {"",	"crypto",	0,	13}, */
	//{"",	"torrent",	20,	7},
	/* {"",	"memory",	10,	14}, */
	/* {"",	"cpu",		10,	13}, */
	/* {"",	"moonphase",	18000,	5}, */
	//{"",	"weather",	18000,	5},
	//{"",	"mailbox",	180,	12},
	/* {"",	"nettraf",	1,	16}, */
	//{"",	"help-icon",	0,	15},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = '|';
