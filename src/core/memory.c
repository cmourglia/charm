#include "memory.h"

#include <stdlib.h>
#include <string.h>

void *mem_reallocate(void *buffer, usize new_capacity)
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
