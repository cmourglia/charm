#include "memory.h"

#include <stdlib.h>
#include <string.h>

void *mem_reallocate(void *buffer, int new_capacity)
{
	if (new_capacity == 0)
	{
		free(buffer);
		return NULL;
	}

	void *result = realloc(buffer, new_capacity);

	if (result == NULL)
	{
		exit(42);
	}

	return result;
}

void mem_copy(void *destination, const void *source, int size)
{
	memcpy(destination, source, size);
}
