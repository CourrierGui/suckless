#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "dynamic-array.h"
#include "normalMode.h"

#include "st.h"
#include "win.h"
#include "x.h"

int rows();

extern Glyph const styleSearch, style[];
extern char const wDelS[], wDelL[], *nmKeys[];
extern unsigned int bg[], fg, currentBg, highlightBg, highlightFg, amountNmKeys;
extern Term term;
extern int histOp, histMode, histOff, insertOff, altToggle, *mark;
extern Line *buf;
extern Selection selection;
extern TCursor c[3];
extern TermWindow termwin;

typedef struct {
	int p[3];
} Pos;

typedef enum {
	visual     = 'v',
	visualLine = 'V',
	yank       = 'y'
} Op;

typedef enum {
	infix_none = '\0',
	infix_i    = 'i',
	infix_a    = 'a'
} Infix;

typedef enum {
	fw = '/',
	bw = '?'
} Search;

struct NormalModeState {
	struct OperationState {
		Op op;
		Infix infix;
	} cmd;
	struct MotionState {
		uint32_t c;
		int active;
		Pos searchPos;
		Search search;
	} m;
} defaultNormalMode, state;

struct vector searchStr = UTF8_ARRAY;
struct vector cCmd = UTF8_ARRAY;
struct vector lCmd = UTF8_ARRAY;

Glyph styleCmd;

char posBuffer[10];
char braces[6][3] = {
	{"()"}, {"<>"}, {"{}"}, {"[]"}, {"\"\""}, {"''"}
};

int exited=1, overlay=1;

static Rune cChar(Term *term)
{
	return term->line[term->c.y][term->c.x].u;
}

static int contains(const char *values, const size_t memSize, Rune l)
{
	for (uint32_t i = 0; i < memSize; ++i)
		if (l == values[i])
			return 1;

	return 0;
}

static void decodeTo(char const *cs, size_t len, struct vector *arr)
{
	char *var = da_expand(arr);

	if (!var)
		da_empty(arr);
	else
		utf8decode(cs, (Rune*)var, len);
}

static void applyPos(Pos p)
{
	term.c.x = p.p[0];
	term.c.y = p.p[1];

	if (!IS_SET(MODE_ALTSCREEN) && histOp)
		term.line = &buf[histOff = p.p[2]];
}

/* Find string in history buffer, and provide string-match-lookup for
 * highlighting matches
 */
static int highlighted(int x, int y)
{
	int const s = term.row * term.col;
	int const i = y * term.col + x;
	int const sz = da_size(&searchStr);
	
	return sz /* search string not empty               */
		/* index inside terminal                       */
		&& i < s
		/* character match                             */
		&& mark[i] != sz
		/* the end of the match is inside the terminal */
		&& i + mark[i] < s
		/* the end of the match is 0                   */
		&& !mark[i + mark[i]];
}

/* mark == sz: no match found -> highlighted returns 0
 * mark == 0:  end of the match and every character matched
 */
static void markSearchMatches(int all)
{
	int sz = da_size(&searchStr);
	int ox = 0, oy = 0, oi=0;

	/* mark highlighted rows as dirty so that they can be redrawn */
	for (int y=0; sz && all && y<term.row; ++y)
		for (int x=0; x<term.col; ++x)
			term.dirty[y] |= highlighted(x, y);

	/*
	 * wi starts at 0
	 * if the first value in the search string equals to the current one in the terminal,
	 * increment wi
	 * continue to do so while the values match
	 */
	for (int y = 0, wi=0, owi=0, i=0; sz && y < term.row; ++y) {
		for (int x=0; x<term.col; ++x, wi %= sz, ++i, owi = wi) {
			if (all || term.dirty[y]) {
				wi = (da_getu32(&searchStr, wi, 1) ==
					  term.line[y][x].u) ? wi + 1 : 0;
				mark[i] = sz - wi; /* mark: sz sz 4 3 2 1 0 sz sz with sz=5 */

				/* save the value of x, y and i on the first match */
				if (wi == 1) {
					ox = x;
					oy = y;
					oi = i;
				} else if (!wi && owi) {
					/* if the match fails before the end of the search string
					 * the test will be done on the next character
					 */
					x = ox;
					y = oy;
					i = oi;
				}
			}
		}
	}

    /* mark highlighted rows as dirty so that they can be redrawn */
	for (int y=0; sz &&all &&y<term.row; ++y)
		for (int x=0; x<term.col; ++x)
			term.dirty[y] |= highlighted(x, y);
}

static int findString(int s, int all)
{
	Pos p = (Pos) {
		.p = {
			term.c.x,
			term.c.y,
			IS_SET(MODE_ALTSCREEN) ? 0 : histOff
		}
	};

	historyMove(s, 0, 0);
	uint32_t strSz = da_size(&searchStr);
	uint32_t maxIter = rows() * term.col + strSz;
	uint32_t wIdx = 0;

	for (uint32_t i = 0, wi = 0;
		 wIdx < strSz && ++i <= maxIter;
		 historyMove(s, 0, 0), wi=wIdx) {
		wIdx = (da_getu32(&searchStr, wIdx, s>0) ==
				cChar(&term)) ? wIdx + 1 : 0;
		if (wi && !wIdx)
			historyMove(-(int)(s*wi), 0, 0);
	}

	if (wIdx == strSz && wIdx)
		historyMove(-(int)(s*strSz), 0, 0);
	else
		applyPos(p);

	markSearchMatches(all);
	return wIdx == strSz;
}

void normalMode()
{
	bool is_normal = (termwin.mode ^= MODE_NORMAL) & MODE_NORMAL;
	historyModeToggle(is_normal);
}

/* Execute series of normal-mode commands from char array / decoded from dynamic array */
ExitState pressKeys(char const* s, size_t e)
{
	ExitState x = success;
	for (size_t i=0;
		 i<e && (x=(!s[i] ? x : kPressHist(&s[i], 1, 0, NULL)));
		 ++i);
	return x;
}

static ExitState executeCommand(uint32_t *cs, size_t z)
{
	ExitState x = success;
	char dc [32];

	for (size_t i = 0;
		 i < z && (x = kPressHist(dc, utf8encode(cs[i], dc), 0, NULL));
		 ++i);

	return x;
}

/* Get character for overlay, if the overlay (st) has something to show, else
 * normal char.
 */
static void getChar(struct vector *st, Glyph *glyphChange,
					int y, int xEnd, int width, int x)
{
	if (x < xEnd - min(min(width, xEnd), da_size(st)))
		*glyphChange = term.line[y][x];
	else if (x < xEnd)
		glyphChange->u = *((Rune*) da_item_from_end(st, xEnd-x));
}

/*
 * Expand "infix" expression.
 *
 * For instance:
 * (w =>)       l     b     |   | v     e    |   | y
 * ({ =>)       l  ?  {  \n | l | v  /  } \n | h | y
 */
static ExitState expandExpression(char l)
{
	int a = (state.cmd.infix == infix_a);
	int yank = (state.cmd.op == 'y');
	int lc = tolower(l), found = 1;

	state.cmd.infix = infix_none;

	if (!yank && state.cmd.op != visual && state.cmd.op != visualLine)
		return failed;

	char mot[11] = {
		'l', 0, 'b', 0, 0, 'v', 0, 'e', 0, 0, (char)(yank ? 'y' : 0)
	};

	if (lc == 'w') {
		mot[2] = (char) ('b' - lc + l);
		mot[7] = (char) ((a ? 'w' : 'e') - lc + l);
		mot[9] = (char) (a ? 'h' : 0);

	} else {
		mot[1] = '?';
		mot[3] = mot[8] = '\n';
		mot[6] = '/';
		mot[4] = (char)(a ? 0 : 'l');
		mot[9] = (char)(a ? 0 : 'h');

		for (int i=found=0; !found && i < 6; ++i) {
			if ((found = contains(braces[i], 2, l))) {
				mot[2] = braces[i][0];
				mot[7] = braces[i][1];
			}
		}
	}

	if (!found)
		return failed;

	da_assign(&lCmd, &cCmd);
	da_empty(&cCmd);
	state.cmd = defaultNormalMode.cmd;
	return pressKeys(mot, 11);
}

ExitState executeMotion(char const cs, KeySym const *const ks)
{
	state.m.c = state.m.c < 1u ? 1u : state.m.c;

	if      (ks && *ks == XK_d)      historyMove(0, 0, term.row / 2);
	else if (ks && *ks == XK_u)      historyMove(0, 0, -term.row / 2);
	else if (ks && *ks == XK_f)      historyMove(0, 0, term.row-1+(term.c.y=0));
	else if (ks && *ks == XK_b)      historyMove(0, 0, -(term.c.y=term.row-1));
	else if (ks && *ks == XK_h)      overlay = !overlay;
	else if (cs == 'K')              historyMove(0, 0, -(int)state.m.c);
	else if (cs == 'J')              historyMove(0, 0,  (int)state.m.c);
	else if (cs == 'k')              historyMove(0, -(int)state.m.c, 0);
	else if (cs == 'j')              historyMove(0,  (int)state.m.c, 0);
	else if (cs == 'h')              historyMove(-(int)state.m.c, 0, 0);
	else if (cs == 'l')              historyMove( (int)state.m.c, 0, 0);
	else if (cs == 'H')              term.c.y = 0;
	else if (cs == 'M')              term.c.y = term.bot / 2;
	else if (cs == 'L')              term.c.y = term.bot;
	else if (cs == 's' || cs == 'S') altToggle = cs == 's' ? !altToggle : 1;
	else if (cs == 'G' || cs == 'g') {
		if (cs == 'G')
			term.c = c[0] = c[IS_SET(MODE_ALTSCREEN)+1];
		if (!IS_SET(MODE_ALTSCREEN))
			term.line = &buf[histOff=insertOff];
	} else if (cs == '0') term.c.x = 0;
	else if (cs == '$')   term.c.x = term.col-1;
	else if (cs == 't')   selection.type = (selection.type == SEL_REGULAR) ? SEL_RECTANGULAR : SEL_REGULAR;
	else if (cs == 'n' || cs == 'N') {
		int const d = ((cs=='N') != (state.m.search==bw)) ? -1 : 1;
		for (uint32_t i = state.m.c; i && findString(d, 0); --i);
	} else if (contains("wWeEbB", 6, cs)) {
		int const low = cs <= 90;
		int const off = tolower(cs) != 'w';
		int const sgn = (tolower(cs) == 'b') ? -1 : 1;

		size_t const l = strlen(wDelL);
		size_t const s = strlen(wDelS);
		size_t const maxIt = rows() * term.col;

		for (int it=0, on=0; state.m.c > 0 && it < maxIt; ++it) {
			/* If an offset is to be performed in beginning or not in
			 * beginning, move in history. */
			if ((off || it)
				&& historyMove(sgn, 0, 0)) break;

			 /* Determine if the category of the current letter changed since
			  * last iteration. */
			int n = 1 << (contains(wDelS, s, cChar(&term)) ?
						  (2 - low) :
						  !contains(wDelL, l, cChar(&term)));
			int found = ((on |= n) ^ n) && ((off ? (on ^ n) : n) != 1);

			 /* If a reverse offset is to be performed and this is the last letter */
			if (found && off)
				historyMove(-sgn, 0, 0);

			/* Terminate iteration: reset #it and old n value #on and decrease
			 * operation count
			 */
			if (found) {
				it = -1;
				on = 0;
				--state.m.c;
			}
		}
	} else {
		return failed;
	}

	state.m.c = 0;

	return (state.cmd.op == yank) ? exitMotion : success;
}

ExitState kPressHist(char const *cs, size_t len, int ctrl, KeySym const *kSym)
{
	historyOpToggle(1, 1);
	int const prevYOff = IS_SET(MODE_ALTSCREEN) ? 0 : histOff;
	int const search = state.m.search && state.m.active;
	int const prevAltToggle = altToggle;
	int const prevOverlay = overlay;
	int const noOp = !state.cmd.op && !state.cmd.infix;
	int const num = (len == 1 && BETWEEN(cs[0],48,57));
	int const esc = kSym && *kSym == XK_Escape;
	int const ret = (kSym && *kSym == XK_Return) || (len == 1 && cs[0] == '\n');
	int const quantifier = num && (cs[0] != '0' || state.m.c);
	int const ins = !search && noOp && len && cs[0]=='i';
	ExitState result = success;

	exited = 0;

	if (esc || ret || ins) {
		result = exitMotion;
		len = 0;

	} else if (kSym && *kSym == XK_BackSpace) {
		if ((search || state.m.c) && da_size(&cCmd))
			da_pop(&cCmd);

		if (search) {
			if (da_size(&searchStr))
				da_pop(&searchStr);
			else
				result = exitMotion;

			if (!da_size(&searchStr))
				tfulldirt();

			applyPos(state.m.searchPos);
			findString(state.m.search==fw ? 1 : -1, 1);
		} else if (state.m.c) {
			state.m.c /= 10;
		}
		len = 0;
	} else if (search) {
		if (len >= 1)
			decodeTo(cs, len, &searchStr);
		applyPos(state.m.searchPos);
		findString((state.m.search == fw) ? 1 : -1, 1);
	} else if (len == 0) {
		result = failed;
	} else if (quantifier) {
		state.m.c = min(SHRT_MAX, (int)state.m.c*10+cs[0]-48);
	} else if (state.cmd.infix
			   && state.cmd.op && (result = expandExpression(cs[0]), len=0)) {
	} else if (cs[0] == 'd') {
		state = defaultNormalMode;
		result = exitMotion; state.m.active = 1;
	} else if (cs[0] == '.') {
		if (da_size(&cCmd))
			da_assign(&lCmd, &cCmd);
		da_empty(&cCmd);
		executeCommand((uint32_t*) da_item_at(&lCmd, 0), da_size(&lCmd));
		da_empty(&cCmd);
		len = 0;
	} else if (cs[0] == 'r') {
		tfulldirt();
	} else if (cs[0] == 'c') {
		da_empty(&lCmd);
		da_empty(&cCmd);
		da_empty(&searchStr);
		tfulldirt();
		len = 0;
	} else if (cs[0] == fw || cs[0] == bw) {
		da_empty(&searchStr);
		state.m.search = (Search) cs[0];
		state.m.searchPos = (Pos){.p={term.c.x, term.c.y, prevYOff}};
		state.m.active = 1;
	} else if (cs[0]==infix_i || cs[0]==infix_a) {
		state.cmd.infix =( Infix) cs[0];
	} else if (cs[0] == 'y') {
		if (state.cmd.op) {
			result = (state.cmd.op == yank
					  || state.cmd.op == visualLine) ? exitOp : exitMotion;
			if (state.cmd.op == yank)
				selstart(0, term.c.y, 0);
		} else {
			selstart(term.c.x, term.c.y, 0);
		}
		state.cmd.op = yank;
	} else if (cs[0] == visual || cs[0] == visualLine) {
		if (state.cmd.op != (Op) cs[0]) {
			state.cmd = defaultNormalMode.cmd;
			state.cmd.op = (Op) cs[0];
			selstart(cs[0] == visualLine ?0 :term.c.x, term.c.y, 0);
		} else {
			result = exitOp;
		}
	} else if (!(result = executeMotion((char) (len ? cs[0] : 0),
										ctrl ? kSym : NULL))) {
		result = failed;

		for (size_t i = 0; !ctrl && i < amountNmKeys; ++i)
			if (cs[0] == nmKeys[i][0] &&
				failed != (result = pressKeys(&nmKeys[i][1], strlen(nmKeys[i])-1)))
				goto end;
	} /* Operation/Motion finished if valid: update cmd string, extend
		 selection, update search */

	if (result != failed) {
		if (len == 1 && !ctrl)
			decodeTo(cs, len, &cCmd);

		if ((state.cmd.op == visualLine)
			|| ((state.cmd.op == yank) && (result == exitOp))) {
			/* Selection start below end. */
			int const off = term.c.y
				+ (IS_SET(MODE_ALTSCREEN) ? 0 : histOff) < selection.ob.y;
			selection.ob.x = off ? term.col - 1 : 0;
			selextend(off ? 0 : term.col-1, term.c.y, selection.type, 0);
		} else if (selection.oe.x != -1) {
			selextend(term.c.x, term.c.y, selection.type, 0);
		}
	} // Set repaint for motion or status bar

	if (!IS_SET(MODE_ALTSCREEN) && prevYOff != histOff)
		tfulldirt();

	/* Terminate Motion / operation if thus indicated */
	if (result == exitMotion) {
		if (!state.m.active)
			result = (exited = noOp) ? finish : exitOp;

		state.m.active = (int) (state.m.c = 0u);
	}

	if (result == exitOp || result == finish) {
		if (state.cmd.op == yank) {
			xsetsel(getsel());
			xclipcopy();
		}
		state = defaultNormalMode;
		selclear();

		if (!esc)
			da_assign(&lCmd, &cCmd);

		da_empty(&cCmd);
	} // Update the content displayed in the history overlay

	styleCmd = style[state.cmd.op==yank ? 1 : (state.cmd.op==visual ? 2 :
											   (state.cmd.op==visualLine ? 3 :0))];
	int const posLin = !IS_SET(MODE_ALTSCREEN) ? rangeY(insertOff-histOff) : 0, h=rows()-term.row;

	if (!posLin || posLin == h || !h)
		strcpy(posBuffer, posLin ? " [BOT] " : " [TOP] ");
	else
		sprintf(posBuffer, " % 3d%c  ", min(100, max(0, (int)(.5 + posLin * 100. / h))),'%');

	if ((overlay || overlay!=prevOverlay) && term.col>9 && term.row>4) {
		if (!term.dirty[term.row-1])
			xdrawline(term.line[term.row-1], term.col*2/3, term.row-1, term.col-1);

		if (!term.dirty[term.row-2])
			xdrawline(term.line[term.row-2], term.col*2/3, term.row-2, term.col-1);
	}
	if (result==finish)
		altToggle = 0;

	if (altToggle != prevAltToggle)
		tswapscreen();

end:
	historyOpToggle(-1, 1);
	return result;
}

void historyOverlay(int x, int y, Glyph* g)
{
	if (!histMode)
		return;

	TCursor const *cHist = histOp ? &term.c : &c[0];

	if(overlay
	   && term.col > 9
	   && term.row > 4
	   && (x > (2*term.col/3))
	   && (y >= (term.row-2))) {
		*g = (y == term.row - 2) ? styleSearch : styleCmd;

		if (y == term.row-2)
			getChar(&searchStr, g, term.row-2, term.col-2, term.col/3, x);
		else if (x > term.col - 7)
			g->u = (Rune)(posBuffer[x - term.col + 7]);
		else
			getChar(da_size(&cCmd) ? &cCmd : &lCmd,
					g, term.row-1, term.col-7, term.col/3-6, x);

	} else if (highlighted(x, y)) {
		g->bg = highlightBg, g->fg = highlightFg;

	} else if ((x == cHist->x) ^ (y == cHist->y)) {
		g->bg = currentBg;

	} else if (x == cHist->x) {
		g->mode ^= ATTR_REVERSE;
	}
}

void historyPreDraw()
{
	static Pos op = { .p = {0, 0, 0} };
	historyOpToggle(1, 0);

	// Draw the cursor cross if changed
	if (term.c.y >= term.row || op.p[1] >= term.row)
		tfulldirt();
	else if (exited || (op.p[1] != term.c.y))
		term.dirty[term.c.y] = term.dirty[op.p[1]] = 1;

	for (int i = 0; (exited || term.c.x != op.p[0]) && i<term.row; ++i) {
		if (!term.dirty[i]) {
			xdrawline(term.line[i], term.c.x, i, term.c.x + 1);
			xdrawline(term.line[i], op.p[0], i, op.p[0] + 1);
		}
	}

	/* Update search results either only for lines with new content or all
	 * results if exiting */
	markSearchMatches(exited);
	op = (Pos){
		.p = {
			term.c.x, term.c.y, 0
		}
	};
	historyOpToggle(-1, 0);
}
