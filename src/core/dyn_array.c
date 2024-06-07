#include "dyn_array.h"

#include <stdio.h>

#include "memory.h"

typedef struct
{
	usize capacity;
	usize length;
} Dyn_Array_Header;

static const usize INITIAL_CAPACITY = 8;
static const usize GROWTH_FACTOR = 2;

#define da_header(arr) \
	(((arr) != NULL) ? ((Dyn_Array_Header *)(arr) - 1) : NULL)

static Dyn_Array_Header *da_resize(Dyn_Array_Header *old, usize capacity,
								   usize stride)
{
	usize array_size = capacity * stride;
	usize total_size = array_size + sizeof(Dyn_Array_Header);
	usize length = old ? old->length : 0;
	Dyn_Array_Header *header = mem_realloc(old, total_size);
	header->capacity = capacity;
	header->length = length;

	return header;
}

void *_darray_push(void *arr, void *elem, usize stride)
{
	Dyn_Array_Header *header = da_header(arr);

	if (header == NULL)
	{
		header = da_resize(NULL, INITIAL_CAPACITY, stride);
	}
	else if (header->length + 1 >= header->capacity)
	{
		usize new_capacity = header->capacity * GROWTH_FACTOR;
		header = da_resize(header, new_capacity, stride);
	}

	byte *array = (byte *)header + sizeof(Dyn_Array_Header);

	mem_copy(array + stride * header->length, elem, stride);
	header->length += 1;

	return array;
}

void _darray_pop(void *arr)
{
	Dyn_Array_Header *header = da_header(arr);
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
	Dyn_Array_Header *header = da_header(arr);
	mem_free(header);
}
