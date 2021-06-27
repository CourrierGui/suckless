#pragma once

#include <stddef.h>

#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#define MAX(a, b)        ((a) < (b) ? (b) : (a))
#define LEN(a)           (sizeof(a) / sizeof(a[0]))
#define BETWEEN(x, a, b) ((a) <= (x) && (x) <= (b))
#define OUT(x, a, b)     ((a) <= (x) || (x) <= (b))

void die(const char *errstr, ...);
void pdie(const char *fmt, ...);
void *xmalloc(size_t len);
void *xrealloc(void *p, size_t len);
char *xstrdup(const char *s);
void *ecalloc(size_t nmemb, size_t size);
