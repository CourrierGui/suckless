#include "dynamic-array.h"

int p_alloc(DynamicArray *s, uint32_t amount)
{
	uint32_t const diff = s->init + s->elSize * amount - s->alloc;
	uint32_t const nas = s->alloc + max(diff,15) * s->elSize;

	if (s->alloc < s->init + s->elSize * amount) {
		char* tmp = realloc(s->content, nas);

		if (!tmp)
			return 0;
		s->alloc = nas;
		s->content = tmp;
	}
	return 1;
}

char *view(DynamicArray * s, uint32_t i)
{
	return s->content + i * s->elSize;
}

char *end(DynamicArray *s, uint32_t i)
{
	return s->content + s->init - (i+1) * s->elSize;
}

uint32_t getU32(DynamicArray* s, uint32_t i, int b)
{
	return *((uint32_t*) (b ? view(s,i) : end(s,i)));
}

char *expand(DynamicArray *s)
{
	if (!p_alloc(s, 1))
		return NULL;

	s->init += s->elSize;

	return end(s, 0);
}

void pop(DynamicArray* s)
{
	s->init -= s->elSize;
}

void empty(DynamicArray* s)
{
	s->init = 0;
}

int size(DynamicArray const * s)
{
	return s->init / s->elSize;
}

void assign(DynamicArray* s, DynamicArray const *o)
{
	if (p_alloc(s, size(o)))
		memcpy(s->content, o->content, (s->init=o->init));
}
