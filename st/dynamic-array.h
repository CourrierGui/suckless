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
struct vector {
	uint8_t element_size;
	uint32_t size, capacity;
	char *values;
};

#define UTF8_ARRAY {4, 0, 0, NULL}

/**
 * Allocate space for \p amount elements if
 * the capacity is not beg enough.
 */
int da_fit(struct vector *s, uint32_t amount);

/**
 * Returns a pointer to the \p i-th element of \p s.
 */
char *da_item_at(struct vector * s, uint32_t i);

/**
 * Returns a pointer to the \p i-th element of \p s
 * in reverse order.
 */
char *da_item_from_end(struct vector *s, uint32_t i);

/**
 * Get the \p i-th element of \p s and cast it to an uint32_t.
 * In reverse order if \p begin is false.
 */
uint32_t da_getu32(struct vector* s, uint32_t i, bool begin);


/**
 * Expand the size of \p s by one element and return the
 * last slot.
 */
char *da_expand(struct vector *s);

/**
 * Remove the last element of \p s.
 */
void da_pop(struct vector* s);

/**
 * Clear the content of \p s.
 */
void da_empty(struct vector* s);

/**
 * Return the number of elements in \p s.
 */
int da_size(struct vector const * s);

/**
 * Copy the content of \p o in \p s.
 */
void da_assign(struct vector* s, struct vector const *o);
