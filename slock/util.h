#pragma once

#include <stddef.h>

#undef explicit_bzero
void explicit_bzero(void *, size_t);
