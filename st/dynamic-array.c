#include "dynamic-array.h"

int da_fit(DynamicArray *s, uint32_t amount)
{
	uint32_t const diff = s->size + s->element_size * amount - s->capacity;
	uint32_t const nas = s->capacity + max(diff,15) * s->element_size;

	if (s->capacity < s->size + s->element_size * amount) {
		char* tmp = realloc(s->values, nas);
		if (!tmp)
			return 0;

		s->capacity = nas;
		s->values = tmp;
	}
	return 1;
}

char *da_item_at(DynamicArray * s, uint32_t i)
{
	return s->values + i * s->element_size;
}

char *da_item_from_end(DynamicArray *s, uint32_t i)
{
	return s->values + s->size - (i+1) * s->element_size;
}

uint32_t  da_getu32(DynamicArray* s, uint32_t i, bool begin)
{
	return *((uint32_t*) (begin ? da_item_at(s, i) : da_item_from_end(s, i)));
}

char *da_expand(DynamicArray *s)
{
	if (!da_fit(s, 1))
		return NULL;

	s->size += s->element_size;

	return da_item_from_end(s, 0);
}

void da_pop(DynamicArray* s)
{
	s->size -= s->element_size;
}

void da_empty(DynamicArray* s)
{
	s->size = 0;
}

int da_size(DynamicArray const * s)
{
	return s->size / s->element_size;
}

void da_assign(DynamicArray* s, DynamicArray const *o)
{
	if (da_fit(s, da_size(o)))
		memcpy(s->values, o->values, (s->size = o->size));
}
