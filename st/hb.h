#pragma once

#include <X11/Xft/Xft.h>
#include <hb.h>
#include <hb-ft.h>

#include "st.h"

void hbunloadfonts();
void hbtransform(XftGlyphFontSpec *, const Glyph *, size_t, int, int);
