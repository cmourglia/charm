#pragma once

#include "common.h"

struct Value;

typedef enum CellType
{
	CELL_STRING,
} CellType;

typedef struct Cell
{
	CellType type;
} Cell;

typedef struct String
{
	Cell cell;
	i32 len;
	char str[];
} String;

#define is_string(value) cell_is_of_type((value), CELL_STRING)

#define as_string(value) ((String *)(value).as.cell)
#define as_cstring(value) (as_string(value)->str)

bool cell_is_of_type(struct Value value, CellType type);

String *string_from(const char *str, i32 len);
