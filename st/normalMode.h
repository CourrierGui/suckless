#pragma once

#include "st.h"
#include <X11/Xlib.h>
#include <stdbool.h>

typedef enum {
    failed = 0,
    success,
    exitMotion,
    exitOp,
    finish
} ExitState;

void normalMode();
void historyOverlay(int x, int y, Glyph* g);
void historyModeToggle(bool start);
void historyOpToggle(int, int);
void historyPreDraw();

ExitState kPressHist(char const *txt, size_t len, int ctrl, KeySym const *kSym);
ExitState pressKeys(char const *s, size_t e);
