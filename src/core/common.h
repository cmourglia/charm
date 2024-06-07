#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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

#define UNUSED(x) (void)x

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define UNREACHABLE()                                                     \
	fprintf(stderr, "Unreachable code detected in function %s (%s:%d)\n", \
			__FUNCTION__, __FILE__, __LINE__);                            \
	exit(43);

#define NODISCARD __attribute__((warn_unused_result))
