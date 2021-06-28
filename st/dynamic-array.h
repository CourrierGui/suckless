#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "st.h"

#define UTF8_ARRAY {4, 0, 0, NULL}

/* Dynamic memory-chunk, with (1) datatype size, (2/3) initialized / allocated chunk, (4) content */
typedef struct {
	uint8_t const elSize;
	uint32_t init, alloc;
	char* content;
} DynamicArray;

int p_alloc(DynamicArray *s, uint32_t amount);
char *view(DynamicArray * s, uint32_t i);
char *end(DynamicArray *s, uint32_t i);
uint32_t getU32(DynamicArray* s, uint32_t i, int b);
char *expand(DynamicArray *s);
void pop(DynamicArray* s);
void empty(DynamicArray* s);
int size(DynamicArray const * s);
void assign(DynamicArray* s, DynamicArray const *o);
