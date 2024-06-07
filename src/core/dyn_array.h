#pragma once

#include "common.h"

extern void *_darray_push(void *arr, void *elem, usize stride);
extern void _darray_pop(void *arr);
extern void _darray_free(void *arr);
extern usize _darray_len(void *arr);

#define darray_free(arr) _darray_free(arr)
#define darray_len(arr) _darray_len(arr)
#define darray_empty(arr) (darray_len(arr) == 0)
#define darray_push(arr, elem)                          \
	do                                                  \
	{                                                   \
		typeof(elem) _temp = elem;                      \
		arr = _darray_push(arr, &_temp, sizeof(_temp)); \
	} while (false)
#define darray_pop(arr) (_darray_pop(arr), (arr)[darray_len(arr)])
#define darray_last(arr) (arr)[darray_len(arr) - 1]
#define darray_back darray_last
