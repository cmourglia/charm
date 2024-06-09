#include "dyn_array.h"

#include <stdio.h>

#include "memory.h"

typedef struct
{
	usize capacity;
	usize length;
} DynArrayHeader;

static const usize INITIAL_CAPACITY = 8;
static const usize GROWTH_FACTOR = 2;

#define da_header(arr) (((arr) != NULL) ? ((DynArrayHeader *)(arr) - 1) : NULL)

static DynArrayHeader *da_resize(DynArrayHeader *old, usize capacity,
								 usize stride)
{
	usize array_size = capacity * stride;
	usize total_size = array_size + sizeof(DynArrayHeader);
	usize length = old ? old->length : 0;
	DynArrayHeader *header = mem_realloc(old, total_size);
	header->capacity = capacity;
	header->length = length;

	return header;
}

void *_darray_push(void *arr, void *elem, usize stride)
{
	DynArrayHeader *header = da_header(arr);

	if (header == NULL)
	{
		header = da_resize(NULL, INITIAL_CAPACITY, stride);
	}
	else if (header->length + 1 >= header->capacity)
	{
		usize new_capacity = header->capacity * GROWTH_FACTOR;
		header = da_resize(header, new_capacity, stride);
	}

	byte *array = (byte *)header + sizeof(DynArrayHeader);

	mem_copy(array + stride * header->length, elem, stride);
	header->length += 1;

	return array;
}

void _darray_pop(void *arr)
{
	DynArrayHeader *header = da_header(arr);
	if (header == NULL || header->length == 0)
	{
		return;
	}

	header->length -= 1;
}

usize _darray_len(void *arr)
{
	if (arr == NULL)
	{
		return 0;
	}
	return da_header(arr)->length;
}

void _darray_free(void *arr)
{
	DynArrayHeader *header = da_header(arr);
	mem_free(header);
}
