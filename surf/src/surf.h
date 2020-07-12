#pragma once

#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>
#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <regex.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gcr/gcr.h>
#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit2.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include <glib.h>

#include "arg.h"
#include "common.h"

#define LENGTH(x)               (sizeof(x) / sizeof(x[0]))
#define CLEANMASK(mask)         (mask & (MODKEY|GDK_SHIFT_MASK))

enum { AtomFind, AtomGo, AtomUri, AtomLast };

enum {
	OnDoc   = WEBKIT_HIT_TEST_RESULT_CONTEXT_DOCUMENT,
	OnLink  = WEBKIT_HIT_TEST_RESULT_CONTEXT_LINK,
	OnImg   = WEBKIT_HIT_TEST_RESULT_CONTEXT_IMAGE,
	OnMedia = WEBKIT_HIT_TEST_RESULT_CONTEXT_MEDIA,
	OnEdit  = WEBKIT_HIT_TEST_RESULT_CONTEXT_EDITABLE,
	OnBar   = WEBKIT_HIT_TEST_RESULT_CONTEXT_SCROLLBAR,
	OnSel   = WEBKIT_HIT_TEST_RESULT_CONTEXT_SELECTION,
	OnAny   = OnDoc | OnLink | OnImg | OnMedia | OnEdit | OnBar | OnSel,
};

typedef enum {
	AcceleratedCanvas,
	AccessMicrophone,
	AccessWebcam,
	CaretBrowsing,
	Certificate,
	CookiePolicies,
	DiskCache,
	DefaultCharset,
	DNSPrefetch,
	FileURLsCrossAccess,
	FontSize,
	FrameFlattening,
	Geolocation,
	HideBackground,
	Inspector,
	Java,
	JavaScript,
	KioskMode,
	LoadImages,
	MediaManualPlay,
	Plugins,
	PreferredLanguages,
	RunInFullscreen,
	ScrollBars,
	ShowIndicators,
	SiteQuirks,
	SmoothScrolling,
	SpellChecking,
	SpellLanguages,
	StrictTLS,
	Style,
	WebGL,
	ZoomLevel,
	ParameterLast
} ParamName;

typedef union {
	int i;
	float f;
	const void *v;
} Arg;

typedef struct {
	Arg val;
	int prio;
} Parameter;

typedef struct Client {
	GtkWidget *win;
	WebKitWebView *view;
	WebKitWebInspector *inspector;
	WebKitFindController *finder;
	WebKitHitTestResult *mousepos;
	GTlsCertificate *cert, *failedcert;
	GTlsCertificateFlags tlserr;
	Window xid;
	unsigned long pageid;
	int progress, fullscreen, https, insecure, errorpage;
	const char *title, *overtitle, *targeturi;
	const char *needle;
	struct Client *next;
} Client;

typedef struct {
	guint mod;
	guint keyval;
	void (*func)(Client *c, const Arg *a);
	const Arg arg;
} Key;

typedef struct {
	unsigned int target;
	unsigned int mask;
	guint button;
	void (*func)(Client *c, const Arg *a, WebKitHitTestResult *h);
	const Arg arg;
	unsigned int stopevent;
} Button;

typedef struct {
	const char *uri;
	Parameter config[ParameterLast];
	regex_t re;
} UriParameters;

typedef struct {
	char *regex;
	char *file;
	regex_t re;
} SiteSpecific;

/* Surf */
static void usage(void);
static void setup(void);
static void sigchld(int unused);
static void sighup(int unused);
static char *buildfile(const char *path);
static char *buildpath(const char *path);
static char *untildepath(const char *path);
static const char *getuserhomedir(const char *user);
static const char *getcurrentuserhomedir(void);
static Client *newclient(Client *c);
static void loaduri(Client *c, const Arg *a);
static const char *geturi(Client *c);
static void setatom(Client *c, int a, const char *v);
static const char *getatom(Client *c, int a);
static void updatetitle(Client *c);
static void gettogglestats(Client *c);
static void getpagestats(Client *c);
static WebKitCookieAcceptPolicy cookiepolicy_get(void);
static char cookiepolicy_set(const WebKitCookieAcceptPolicy p);
static void seturiparameters(Client *c, const char *uri, ParamName *params);
static void setparameter(Client *c, int refresh, ParamName p, const Arg *a);
static const char *getcert(const char *uri);
static void setcert(Client *c, const char *file);
static const char *getstyle(const char *uri);
static void setstyle(Client *c, const char *file);
static void runscript(Client *c);
static void evalscript(Client *c, const char *jsstr, ...);
static void updatewinid(Client *c);
static void handleplumb(Client *c, const char *uri);
static void newwindow(Client *c, const Arg *a, int noembed);
static void spawn(Client *c, const Arg *a);
static void msgext(Client *c, char type, const Arg *a);
static void destroyclient(Client *c);
static void cleanup(void);

/* GTK/WebKit */
static WebKitWebView *newview(Client *c, WebKitWebView *rv);
static void initwebextensions(WebKitWebContext *wc, Client *c);
static GtkWidget *createview(WebKitWebView *v, WebKitNavigationAction *a,
                             Client *c);
static gboolean buttonreleased(GtkWidget *w, GdkEvent *e, Client *c);
static GdkFilterReturn processx(GdkXEvent *xevent, GdkEvent *event,
                                gpointer d);
static gboolean winevent(GtkWidget *w, GdkEvent *e, Client *c);
static gboolean readpipe(GIOChannel *s, GIOCondition ioc, gpointer unused);
static void showview(WebKitWebView *v, Client *c);
static GtkWidget *createwindow(Client *c);
static gboolean loadfailedtls(WebKitWebView *v, gchar *uri,
                              GTlsCertificate *cert,
                              GTlsCertificateFlags err, Client *c);
static void loadchanged(WebKitWebView *v, WebKitLoadEvent e, Client *c);
static void progresschanged(WebKitWebView *v, GParamSpec *ps, Client *c);
static void titlechanged(WebKitWebView *view, GParamSpec *ps, Client *c);
static void mousetargetchanged(WebKitWebView *v, WebKitHitTestResult *h,
                               guint modifiers, Client *c);
static gboolean permissionrequested(WebKitWebView *v,
                                    WebKitPermissionRequest *r, Client *c);
static gboolean decidepolicy(WebKitWebView *v, WebKitPolicyDecision *d,
                             WebKitPolicyDecisionType dt, Client *c);
static void decidenavigation(WebKitPolicyDecision *d, Client *c);
static void decidenewwindow(WebKitPolicyDecision *d, Client *c);
static void decideresource(WebKitPolicyDecision *d, Client *c);
static void insecurecontent(WebKitWebView *v, WebKitInsecureContentEvent e,
                            Client *c);
static void downloadstarted(WebKitWebContext *wc, WebKitDownload *d,
                            Client *c);
static void responsereceived(WebKitDownload *d, GParamSpec *ps, Client *c);
static void download(Client *c, WebKitURIResponse *r);
static void webprocessterminated(WebKitWebView *v,
                                 WebKitWebProcessTerminationReason r,
                                 Client *c);
static void closeview(WebKitWebView *v, Client *c);
static void destroywin(GtkWidget* w, Client *c);

/* Hotkeys */
static void pasteuri(GtkClipboard *clipboard, const char *text, gpointer d);
static void reload(Client *c, const Arg *a);
static void print(Client *c, const Arg *a);
static void showcert(Client *c, const Arg *a);
static void clipboard(Client *c, const Arg *a);
static void zoom(Client *c, const Arg *a);
static void scrollv(Client *c, const Arg *a);
static void scrollh(Client *c, const Arg *a);
static void navigate(Client *c, const Arg *a);
static void stop(Client *c, const Arg *a);
static void toggle(Client *c, const Arg *a);
static void togglefullscreen(Client *c, const Arg *a);
static void togglecookiepolicy(Client *c, const Arg *a);
static void toggleinspector(Client *c, const Arg *a);
static void find(Client *c, const Arg *a);

/* Buttons */
static void clicknavigate(Client *c, const Arg *a, WebKitHitTestResult *h);
static void clicknewwindow(Client *c, const Arg *a, WebKitHitTestResult *h);
static void clickexternplayer(Client *c, const Arg *a, WebKitHitTestResult *h);
