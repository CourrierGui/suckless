#pragma once

#include <stddef.h>
#include <stdint.h>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;

#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#define MAX(a, b)        ((a) < (b) ? (b) : (a))
#define LEN(a)           (sizeof(a) / sizeof(a[0]))
#define BETWEEN(x, a, b) ((a) <= (x) && (x) <= (b))
#define OUT(x, a, b)     ((a) <= (x) || (x) <= (b))

typedef union {
	int i;
	uint ui;
	float f;
	const void *v;
	const char *s;
} Arg;

void die(const char *errstr, ...);
void pdie(const char *fmt, ...);
void *xmalloc(size_t len);
void *xrealloc(void *p, size_t len);
char *xstrdup(const char *s);
void *ecalloc(size_t nmemb, size_t size);
