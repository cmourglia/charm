#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "memory.h"

#define BEARD_REALLOC(ptr, size) mem_reallocate(ptr, size)
#define BEARD_FREE(ptr) mem_reallocate(ptr, 0)
#include "beard_lib.h"

#define UNUSED(x) (void)x

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define UNREACHABLE()                                                     \
	fprintf(stderr, "Unreachable code detected in function %s (%s:%d)\n", \
			__FUNCTION__, __FILE__, __LINE__);                            \
	exit(43);
