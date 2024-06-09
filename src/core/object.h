#pragma once

#include "common.h"

struct Value;

typedef enum OjectType
{
	OBJ_STRING,
} ObjType;

typedef struct Object
{
	ObjType type;
} Object;

typedef struct String
{
	Object object;
	i32 len;
	char *str;
} String;

#define is_string(value) object_is_of_type((value), OBJ_STRING)

#define as_string(value) ((String *)(value).as.object)
#define as_cstring(value) (as_string(value)->str)

bool object_is_of_type(struct Value value, ObjType type);

String *string_from(const char *str, i32 len);
