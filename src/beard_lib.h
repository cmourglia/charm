#pragma once

#include <stdint.h>

typedef unsigned char byte;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef intptr_t isize;
typedef uintptr_t usize;

typedef float f32;
typedef double f64;

#if defined(BEARD_REALLOC) && !defined(BEARD_FREE) || \
	defined(BEARD_FREE) && !defined(BEARD_REALLOC)
#error "Either both or none of BEARD_REALLOC and BEARD_FREE need to be defined"
#endif

#ifndef BEARD_REALLOC
#include <stdlib.h>
#define BEARD_REALLOC(ptr, size) realloc(ptr, size)
#define BEARD_FREE(ptr) free(ptr)
#endif

#ifndef BEARD_MEMCPY
#include <string.h>
#define BEARD_MEMCPY(dest, orig, size) memcpy(dest, orig, size)
#endif

#ifndef BEARD_ASSERT
#include <assert.h>
#define BEARD_ASSERT assert
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void *_beard_darray_push(void *arr, void *elem, usize stride);
extern void _beard_darray_free(void *arr);
extern usize _beard_darray_len(void *arr);

#define beard_darray_free(arr) _beard_darray_free(arr)
#define beard_darray_len(arr) _beard_darray_len(arr)
#define beard_darray_push(arr, elem)                          \
	do                                                        \
	{                                                         \
		typeof(elem) _temp = elem;                            \
		arr = _beard_darray_push(arr, &_temp, sizeof(_temp)); \
	} while (false)

#ifndef BEARD_NO_SHORT_NAMES
#define darray_push beard_darray_push
#define darray_len beard_darray_len
#define darray_free beard_darray_free
#endif

extern char *beard_read_whole_file(const char *filename);

#ifdef __cplusplus
}
#endif
