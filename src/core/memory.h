#pragma once

#define MEM_GROW_CAPACITY(capacity, min_capacity) \
	MAX((min_capacity + 1), ((capacity) < 8 ? 8 : (capacity) * 2))

#define MEM_ALLOCATE(T, count) (T *)mem_reallocate(NULL, sizeof(T) * (count))

#define MEM_FREE_ARRAY(T, buf) (T *)mem_reallocate(buf, 0)

#define MEM_ZERO(array, T, count) memset(array, 0, sizeof(T) * (count))

void *mem_reallocate(void *buffer, int new_capacity);

void mem_copy(void *destination, const void *source, int size);
