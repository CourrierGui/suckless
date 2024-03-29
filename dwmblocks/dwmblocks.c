#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

#include <sl-utils.h>

#include "dwmblocks.h"

#ifndef NO_X
#include<X11/Xlib.h>
#endif

#ifdef __OpenBSD__
#  define SIGPLUS  SIGUSR1+1
#  define SIGMINUS SIGUSR1-1
#else
#  define SIGPLUS  SIGRTMIN
#  define SIGMINUS SIGRTMIN
#endif

#define CMDLENGTH  50
#define STATUSLENGTH (LEN(blocks) * CMDLENGTH + 1)

#ifndef __OpenBSD__
void dummysighandler(int num);
#endif

void sighandler(int num);
void buttonhandler(int sig, siginfo_t *si, void *ucontext);
void getcmds(int time);
void getsigcmds(unsigned int signal);
void setupsignals();
void sighandler(int signum);
int getstatus(char *str, char *last);
void statusloop();
void termhandler();
void pstdout();
#ifndef NO_X
void setroot();
static void (*writestatus) () = setroot;
static int setupX();
static Display *dpy;
static int screen;
static Window root;
#else
static void (*writestatus) () = pstdout;
#endif


#include "blocks.h"

static char statusbar[LEN(blocks)][CMDLENGTH] = {0};
static char statusstr[2][STATUSLENGTH];
static char button[] = "\0";
static int statusContinue = 1;

/* opens process *cmd and stores output in *output */
void getcmd(const struct block *block, char *output)
{
	FILE *cmdf;
	int i;

	if (block->signal) {
		output[0] = block->signal;
		output++;
	}

	strcpy(output, block->icon);

	if (*button) {
		setenv("BUTTON", button, 1);
		cmdf = popen(block->command, "r");
		*button = '\0';
		unsetenv("BUTTON");
	} else {
		cmdf = popen(block->command,"r");
	}

	if (!cmdf)
		return;

	i = strlen(block->icon);
	fgets(output+i, CMDLENGTH-i-delimLen, cmdf);
	i = strlen(output);
	if (i == 0) {
		/* return if block and command output are both empty */
		pclose(cmdf);
		return;
	}

	if (delim[0] != '\0') {
		/* only chop off newline if one is present at the end */
		i = (output[i-1] == '\n') ? i-1 : i;
		strncpy(output+i, delim, delimLen); 
	} else {
		output[i++] = '\0';
	}

	pclose(cmdf);
}

void getcmds(int time)
{
	const struct block *current;

	for (unsigned int i = 0; i < LEN(blocks); i++) {
		current = blocks + i;
		if ((current->interval != 0 && time % current->interval == 0) || time == -1)
			getcmd(current, statusbar[i]);
	}
}

void getsigcmds(unsigned int signal)
{
	const struct block *current;

	for (unsigned int i = 0; i < LEN(blocks); i++) {
		current = blocks + i;

		if (current->signal == signal)
			getcmd(current,statusbar[i]);
	}
}

void setupsignals()
{
#ifndef __OpenBSD__
	    /* initialize all real time signals with dummy handler */
	for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
		signal(i, dummysighandler);
#endif

	struct sigaction sa;
	for (unsigned int i = 0; i < LEN(blocks); i++) {
		if (blocks[i].signal > 0) {
			signal(SIGMINUS+blocks[i].signal, sighandler);
			/* ignore signal when handling SIGUSR1 */
			sigaddset(&sa.sa_mask, SIGRTMIN+blocks[i].signal);
		}
		sa.sa_sigaction = buttonhandler;
		sa.sa_flags = SA_SIGINFO;
		sigaction(SIGUSR1, &sa, NULL);
	}

}

int getstatus(char *str, char *last)
{
	strcpy(last, str);
	str[0] = '\0';

	for (unsigned int i = 0; i < LEN(blocks); i++)
		strcat(str, statusbar[i]);

	str[strlen(str)-strlen(delim)] = '\0';

	return strcmp(str, last);
}

#ifndef NO_X
void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1]))
		/* Only set root if text has changed. */
		return;

	XStoreName(dpy, root, statusstr[0]);
	XFlush(dpy);
}

int setupX()
{
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		fprintf(stderr, "dwmblocks: failed to open display\n");
		return 0;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	return 1;
}
#endif

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1]))
		/* Only write out if text has changed. */
		return;

	printf("%s\n", statusstr[0]);
	fflush(stdout);
}


void statusloop()
{
	int i = 0;

	setupsignals();
	getcmds(-1);

	while (1) {
		getcmds(i++);
		writestatus();

		if (!statusContinue)
			break;

		sleep(1.0);
	}
}

#ifndef __OpenBSD__
/* this signal handler should do nothing */
void dummysighandler(int signum)
{
    return;
}

void buttonhandler(int sig, siginfo_t *si, void *ucontext)
{
	*button = ('0' + si->si_value.sival_int) & 0xff;
	getsigcmds(si->si_value.sival_int >> 8);
	writestatus();
}
#endif

void sighandler(int signum)
{
	getsigcmds(signum-SIGPLUS);
	writestatus();
}

void termhandler()
{
	statusContinue = 0;
}

int main(int argc, char** argv)
{
	for (int i = 0; i < argc; i++) {//Handle command line arguments
		if (!strcmp("-d",argv[i]))
			strncpy(delim, argv[++i], delimLen);
		else if (!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}

#ifndef NO_X
	if (!setupX())
		return 1;
#endif

	delimLen = MIN(delimLen, strlen(delim));
	delim[delimLen++] = '\0';
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();

#ifndef NO_X
	XCloseDisplay(dpy);
#endif

	return 0;
}
