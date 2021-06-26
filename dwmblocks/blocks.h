/* Modify this file to change what commands output to your statusbar, and
 * recompile using the make command.
 */

static const Block blocks[] = {
	/*Icon*/  /*Command*/                   /*Update Interval*/ /*Update Signal*/
	{"",      "brightness",                  0,                 11             },
	{"",      "volume",                      0,                 10             },
	{"",      "internet",                    5,                  4             },
	{"",      "memory",                     10,                 14             },
	{"",      "cpu",                        10,                 13             },
	{"",      "battery | tr \'\n\' \' \'",   5,                  3             },
	{"",      "clock",                       1,                  8             },
};

// sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim[] = "|";
static unsigned int delimLen = 2;
