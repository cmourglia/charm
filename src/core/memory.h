#pragma once

#include <string.h>

#include "common.h"

#define mem_malloc(size) mem_reallocate(NULL, size)
#define mem_realloc(ptr, size) mem_reallocate(ptr, size)
#define mem_free(ptr) mem_reallocate(ptr, 0)

#define mem_grow_capacity(capacity, min_capacity) \
	MAX((min_capacity + 1), ((capacity) < 8 ? 8 : (capacity) * 2))

#define mem_allocate(T, count) (T *)mem_reallocate(NULL, sizeof(T) * (count))

#define mem_free_array(T, buf) (T *)mem_reallocate(buf, 0)

#define mem_zero(array, T, count) memset(array, 0, sizeof(T) * (count))
#define mem_copy(dest, orig, size) memcpy(dest, orig, size)

void *mem_reallocate(void *buffer, usize new_capacity);
