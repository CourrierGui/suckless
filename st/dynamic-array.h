#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "st.h"

/* Dynamic array
 *
 * \p element_size datatype size
 * \p size         initialized
 * \p capacity     allocated chunk
 * \p values       content
 */
struct DynamicArray {
	uint8_t element_size;
	uint32_t size, capacity;
	char* values;
};
typedef struct DynamicArray DynamicArray;

#define UTF8_ARRAY {4, 0, 0, NULL}

/**
 * Allocate space for \p amount elements if
 * the capacity is not beg enough.
 */
int da_fit(DynamicArray *s, uint32_t amount);

/**
 * Returns a pointer to the \p i-th element of \p s.
 */
char *da_item_at(DynamicArray * s, uint32_t i);

/**
 * Returns a pointer to the \p i-th element of \p s
 * in reverse order.
 */
char *da_item_from_end(DynamicArray *s, uint32_t i);

/**
 * Get the \p i-th element of \p s and cast it to an uint32_t.
 * In reverse order if \p begin is false.
 */
uint32_t da_getu32(DynamicArray* s, uint32_t i, bool begin);


/**
 * Expand the size of \p s by one element and return the
 * last slot.
 */
char *da_expand(DynamicArray *s);

/**
 * Remove the last element of \p s.
 */
void da_pop(DynamicArray* s);

/**
 * Clear the content of \p s.
 */
void da_empty(DynamicArray* s);

/**
 * Return the number of elements in \p s.
 */
int da_size(DynamicArray const * s);

/**
 * Copy the content of \p o in \p s.
 */
void da_assign(DynamicArray* s, DynamicArray const *o);
